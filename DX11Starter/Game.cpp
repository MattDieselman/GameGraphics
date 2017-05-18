#include "Game.h"
#include <WICTextureLoader.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore( 
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	speedBoost = true;
	boostDuration = 1.f;
	boostTime = 0.f;
	scrollSpeed = -5;

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class
	
	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	int length = gameObjects.size();
	for (int i = 0; i < length; i++)
	{
		delete gameObjects[i];
	}
	if (cam) delete cam;

	/*for (Obstacle * i : objects) {
		delete i;
	}*/

	length = meshes.size();
	for (int i = 0; i < length; i++)
	{
		meshes[i]->release();
	}

	length = materials.size();
	for (int i = 0; i < length; i++)
	{
		materials[i]->release();
	}
	for (int i = 0; i < emitters.size(); i++) {
		delete emitters[i];
	}

}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	dirLight.ambientColor = XMFLOAT4(.6f, .6f, .6f, 1.f);
	dirLight.diffuseColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.f);
	dirLight.direction = XMFLOAT3(0.f, -1.f, 0.f);
	//renderManager.rotateDirLight(0, 0, 1, XM_PI / 90, &dirLight);
	dirLight2.ambientColor = XMFLOAT4(.0f, .0f, .0f, 1.f);
	dirLight2.diffuseColor = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.f);
	dirLight2.direction = XMFLOAT3(0.f, 0.f, 1.f);

	pointLight.ambientColor = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	pointLight.diffuseColor = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	pointLight.location = XMFLOAT3(0.f, 0.f, 0.f);

	spotLight.ambientColor = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	spotLight.diffuseColor = XMFLOAT4(.4f, 1.f, 1.f, 1.f);
	spotLight.location = XMFLOAT3(22.5f, 2.2f, 1.9f);
	spotLight.direction = XMFLOAT3(0.f, -1.f, 0.f);
	spotLight.angle = XM_PI / 8;

	spotLight2.ambientColor = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
	spotLight2.diffuseColor = XMFLOAT4(.4f, 1.f, 1.f, 1.f);
	spotLight2.location = XMFLOAT3(32.4f, 2.2f, 1.9f);
	spotLight2.direction = XMFLOAT3(0.f, -1.f, 0.f);
	spotLight2.angle = XM_PI / 8;

	renderManager.rotateSpotLights(1, 0, 0, XM_PI / 30, &spotLight, &spotLight2);

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	//LoadShaders();
	renderManager.LoadShaders(device, context, width, height);
	renderManager.InitShadows(device, context, &spotLight, &spotLight2, &dirLight);
	renderManager.DefaultLastTime();

	materials = renderManager.getMaterials();

	CreateMatrices();
	CreateBasicGeometry();

	renderManager.screen = meshes[1]->copy();

	// Create Game Objects
	gameObjects.push_back(new Entity(meshes[7]->copy(), materials[3]->copy()));
	gameObjects[0]->init(ColliderType::SPHERE, 1.f);
	gameObjects.push_back(new Entity(meshes[4]->copy(), materials[7]->copy()));
	gameObjects[1]->init(ColliderType::AABB, 1.f);
	gameObjects[1]->setPosition(XMFLOAT3(5, 2.5, 0));
	gameObjects[1]->setScale(XMFLOAT3(2, 2, 1));
	gameObjects.push_back(new Entity(meshes[4]->copy(), materials[7]->copy()));
	gameObjects[2]->init(ColliderType::AABB, 1.f);
	gameObjects[2]->setPosition(XMFLOAT3(-5, -2.5, 0));
	gameObjects[2]->setScale(XMFLOAT3(2, 2, 1));
	gameObjects.push_back(new Enemy(meshes[7]->copy(), materials[2]->copy()));
	gameObjects[3]->init(ColliderType::SPHERE, 1.f);
	gameObjects[3]->setPosition(XMFLOAT3(10, 0, 0));

	// Environment
	// Wall
	gameObjects.push_back(new Entity(meshes[1]->copy(), materials[5]->copy()));
	gameObjects[4]->init(ColliderType::SPHERE, 1.f);
	gameObjects[4]->setPosition(XMFLOAT3(13, 0.5, 2));
	gameObjects[4]->setScale(XMFLOAT3(20, 4, 1));
	// Ceiling
	gameObjects.push_back(new Entity(meshes[1]->copy(), materials[4]->copy()));
	gameObjects[5]->init(ColliderType::SPHERE, 1.f);
	gameObjects[5]->setPosition(XMFLOAT3(13, 4.5, 2));
	gameObjects[5]->setRotation(XMFLOAT3(XM_PI / 2, 0, 0));
	gameObjects[5]->setScale(XMFLOAT3(20, 5, 1));
	// Floor
	gameObjects.push_back(new Entity(meshes[1]->copy(), materials[6]->copy()));
	gameObjects[6]->init(ColliderType::SPHERE, 1.f);
	gameObjects[6]->setPosition(XMFLOAT3(13, -3.5, 2));
	gameObjects[6]->setRotation(XMFLOAT3(-XM_PI / 2, 0, 0));
	gameObjects[6]->setScale(XMFLOAT3(20, 5, 1));
	// Wall2
	gameObjects.push_back(new Entity(meshes[1]->copy(), materials[5]->copy()));
	gameObjects[7]->init(ColliderType::SPHERE, 1.f);
	gameObjects[7]->setPosition(XMFLOAT3(53, 0.5, 2));
	gameObjects[7]->setScale(XMFLOAT3(20, 4, 1));
	// Ceiling2
	gameObjects.push_back(new Entity(meshes[1]->copy(), materials[4]->copy()));
	gameObjects[8]->init(ColliderType::SPHERE, 1.f);
	gameObjects[8]->setPosition(XMFLOAT3(53, 4.5, 2));
	gameObjects[8]->setRotation(XMFLOAT3(XM_PI / 2, 0, 0));
	gameObjects[8]->setScale(XMFLOAT3(20, 5, 1));
	// Floor2
	gameObjects.push_back(new Entity(meshes[1]->copy(), materials[6]->copy()));
	gameObjects[9]->init(ColliderType::SPHERE, 1.f);
	gameObjects[9]->setPosition(XMFLOAT3(53, -3.5, 2));
	gameObjects[9]->setRotation(XMFLOAT3(-XM_PI / 2, 0, 0));
	gameObjects[9]->setScale(XMFLOAT3(20, 5, 1));
	// Background
	gameObjects.push_back(new Entity(meshes[1]->copy(), materials[9]->copy()));
	gameObjects[10]->init(ColliderType::AABB, 1.f);
	gameObjects[10]->setPosition(XMFLOAT3(0.f, 3.f, 2.1f));
	gameObjects[10]->setScale(XMFLOAT3(8.9f, 7.f, 1.f));

	// Coins
	gameObjects.push_back(new Entity(meshes[9]->copy(), materials[8]->copy()));
	gameObjects[11]->init(ColliderType::SPHERE, 1.f);
	gameObjects[11]->setPosition(XMFLOAT3(15, 2, 0));
	gameObjects[11]->setScale(XMFLOAT3(3, 3, 3));
	gameObjects.push_back(new Entity(meshes[9]->copy(), materials[8]->copy()));
	gameObjects[12]->init(ColliderType::SPHERE, 1.f);
	gameObjects[12]->setPosition(XMFLOAT3(16, 2, 0));
	gameObjects[12]->setScale(XMFLOAT3(3, 3, 3));
	gameObjects.push_back(new Entity(meshes[9]->copy(), materials[8]->copy()));
	gameObjects[13]->init(ColliderType::SPHERE, 1.f);
	gameObjects[13]->setPosition(XMFLOAT3(17, 2, 0));
	gameObjects[13]->setScale(XMFLOAT3(3, 3, 3));
	gameObjects.push_back(new Entity(meshes[9]->copy(), materials[8]->copy()));
	gameObjects[14]->init(ColliderType::SPHERE, 1.f);
	gameObjects[14]->setPosition(XMFLOAT3(15, 1, 0));
	gameObjects[14]->setScale(XMFLOAT3(3, 3, 3));
	gameObjects.push_back(new Entity(meshes[9]->copy(), materials[8]->copy()));
	gameObjects[15]->init(ColliderType::SPHERE, 1.f);
	gameObjects[15]->setPosition(XMFLOAT3(16, 1, 0));
	gameObjects[15]->setScale(XMFLOAT3(3, 3, 3));
	gameObjects.push_back(new Entity(meshes[9]->copy(), materials[8]->copy()));
	gameObjects[16]->init(ColliderType::SPHERE, 1.f);
	gameObjects[16]->setPosition(XMFLOAT3(17, 1, 0));
	gameObjects[16]->setScale(XMFLOAT3(3, 3, 3));
	gameObjects.push_back(new Entity(meshes[9]->copy(), materials[8]->copy()));
	gameObjects[17]->init(ColliderType::SPHERE, 1.f);
	gameObjects[17]->setPosition(XMFLOAT3(15, 0, 0));
	gameObjects[17]->setScale(XMFLOAT3(3, 3, 3));
	gameObjects.push_back(new Entity(meshes[9]->copy(), materials[8]->copy()));
	gameObjects[18]->init(ColliderType::SPHERE, 1.f);
	gameObjects[18]->setPosition(XMFLOAT3(16, 0, 0));
	gameObjects[18]->setScale(XMFLOAT3(3, 3, 3));
	gameObjects.push_back(new Entity(meshes[9]->copy(), materials[8]->copy()));
	gameObjects[19]->init(ColliderType::SPHERE, 1.f);
	gameObjects[19]->setPosition(XMFLOAT3(17, 0, 0));
	gameObjects[19]->setScale(XMFLOAT3(3, 3, 3));

	for (int i = 11; i < 20; i++)
	{
		std::bitset<1> temp;
		coinCollected.push_back(temp);
	}


	inputManager = InputManager(&hWnd, gameObjects[0], cam, &worldUp);

	/*objects.push_back(new Obstacle(meshes[8]->copy(),materials[2]->copy(), ColliderType::AABB));
	objects[0]->setPosition(XMFLOAT3(5, 2, 0));
	objects[0]->setScale(XMFLOAT3(1, 2, 1));
	objects.push_back(new Obstacle(meshes[8]->copy(), materials[2]->copy(), ColliderType::AABB));
	objects[1]->setPosition(XMFLOAT3(-5, -2, 0));
	objects[1]->setScale(XMFLOAT3(1, 2, 1));*/
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"

	Emitter* emitter;
	Emitter* emitter2;
	//Fire & smoke
	emitter = new Emitter(
		1000,							// Max particles
		100,							// Particles per second
		.4,								// Particle lifetime
		0.1f,							// Start size
		5.0f,							// End size
		XMFLOAT4(1, 0.1f, 0.1f, 0.2f),	// Start color
		XMFLOAT4(1, 0.6f, 0.1f, 0),		// End color
		XMFLOAT3(-.1, -.8, 0),				// Start velocity
		gameObjects[0]->getPosition(),				// Start position
		XMFLOAT3(0, 3, 0),				// Start acceleration
		device,
		renderManager.getPartVert(),
		renderManager.getPartPix(),
		renderManager.getPartText(0));
	emitter2 = new Emitter(
		1000,							// Max particles
		100,							// Particles per second
		.5,								// Particle lifetime
		.5f,							// Start size
		4.0f,							// End size
		XMFLOAT4(0.1f, 0.1f, 0.1f, .40f),	// Start color
		XMFLOAT4(.1f, .1f, .1f, .60f),		// End color
		XMFLOAT3(-.1, -.1, 0),				// Start velocity
		gameObjects[0]->getPosition(),				// Start position
		XMFLOAT3(-.1, .05, 0),				// Start acceleration
		device,
		renderManager.getPartVert(),
		renderManager.getPartPix(),
		renderManager.getPartText(1));

	//Create random spread

	emitters.push_back(emitter);
	emitters.push_back(emitter2);

	for (int i = 0; i < 9; i++) {
		//float randX = (((float)rand() / RAND_MAX) * 2 - 1);
		//float randY = (((float)rand() / RAND_MAX) * 2 - 1);
		Emitter * emitterCoin;
		emitterCoin = new Emitter(15, 5, 1, .05f, 1.0f, XMFLOAT4(.1f, .1f, .1f, 1), XMFLOAT4(.6, .6, .6, 0), XMFLOAT3(0 , 0, 0), gameObjects[11]->getPosition(), XMFLOAT3(0, 0, 0), device, renderManager.getPartVert(), renderManager.getPartPix(), renderManager.getPartText(2));
		emitterCoin->isWorld = false;
		emitterCoin->randomizeVelocity();
		emitters.push_back(emitterCoin);
	}
	//#pragma region CoinEmitters
	//	Emitter * emitterCoin2;
	//	emitterCoin2 = new Emitter(3000, 1000, .5, .05f, 1.0f, XMFLOAT4(.1f, .1f, .1f, .05f), XMFLOAT4(.6, .6, .6, .09), XMFLOAT3(randX - .05, randY, 0), gameObjects[12]->getPosition(), XMFLOAT3(0, 0, 0), device, renderManager.getPartVert(), renderManager.getPartPix(), renderManager.getPartText(2));
	//	Emitter * emitterCoin3;
	//	emitterCoin3 = new Emitter(3000, 1000, .5, .05f, 1.0f, XMFLOAT4(.1f, .1f, .1f, .05f), XMFLOAT4(.6, .6, .6, .09), XMFLOAT3(randX - .05, randY, 0), gameObjects[13]->getPosition(), XMFLOAT3(0, 0, 0), device, renderManager.getPartVert(), renderManager.getPartPix(), renderManager.getPartText(2));
	//	Emitter * emitterCoin4;
	//	emitterCoin4 = new Emitter(3000, 1000, .5, .05f, 1.0f, XMFLOAT4(.1f, .1f, .1f, .05f), XMFLOAT4(.6, .6, .6, .09), XMFLOAT3(randX - .05, randY, 0), gameObjects[14]->getPosition(), XMFLOAT3(0, 0, 0), device, renderManager.getPartVert(), renderManager.getPartPix(), renderManager.getPartText(2));
	//	Emitter * emitterCoin5;
	//	emitterCoin5 = new Emitter(3000, 1000, .5, .05f, 1.0f, XMFLOAT4(.1f, .1f, .1f, .05f), XMFLOAT4(.6, .6, .6, .09), XMFLOAT3(randX - .05, randY, 0), gameObjects[15]->getPosition(), XMFLOAT3(0, 0, 0), device, renderManager.getPartVert(), renderManager.getPartPix(), renderManager.getPartText(2));
	//	Emitter * emitterCoin6;
	//	emitterCoin6 = new Emitter(3000, 1000, .5, .05f, 1.0f, XMFLOAT4(.1f, .1f, .1f, .05f), XMFLOAT4(.6, .6, .6, .09), XMFLOAT3(randX - .05, randY, 0), gameObjects[16]->getPosition(), XMFLOAT3(0, 0, 0), device, renderManager.getPartVert(), renderManager.getPartPix(), renderManager.getPartText(2));
	//	Emitter * emitterCoin7;
	//	emitterCoin7 = new Emitter(3000, 1000, .5, .05f, 1.0f, XMFLOAT4(.1f, .1f, .1f, .05f), XMFLOAT4(.6, .6, .6, .09), XMFLOAT3(randX - .05, randY, 0), gameObjects[17]->getPosition(), XMFLOAT3(0, 0, 0), device, renderManager.getPartVert(), renderManager.getPartPix(), renderManager.getPartText(2));
	//	Emitter * emitterCoin8;
	//	emitterCoin8 = new Emitter(3000, 1000, .5, .05f, 1.0f, XMFLOAT4(.1f, .1f, .1f, .05f), XMFLOAT4(.6, .6, .6, .09), XMFLOAT3(randX - .05, randY, 0), gameObjects[18]->getPosition(), XMFLOAT3(0, 0, 0), device, renderManager.getPartVert(), renderManager.getPartPix(), renderManager.getPartText(2));
	//	Emitter * emitterCoin9;
	//	emitterCoin9 = new Emitter(3000, 1000, .5, .05f, 1.0f, XMFLOAT4(.1f, .1f, .1f, .05f), XMFLOAT4(.6, .6, .6, .09), XMFLOAT3(randX - .05, randY, 0), gameObjects[19]->getPosition(), XMFLOAT3(0, 0, 0), device, renderManager.getPartVert(), renderManager.getPartPix(), renderManager.getPartText(2));

	//#pragma endregion

	//emitters.push_back(emitterCoin2);
	//emitters.push_back(emitterCoin3);
	//emitters.push_back(emitterCoin4);
	//emitters.push_back(emitterCoin5);
	//emitters.push_back(emitterCoin6);
	//emitters.push_back(emitterCoin7);
	//emitters.push_back(emitterCoin8);
	//emitters.push_back(emitterCoin9);


	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------

// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{
	cam = new Camera(width, height);
	worldUp = XMFLOAT3(0, 1, 0);
	// Set up world matrix
	// - In an actual game, each object will need one of these and they should
	//   update when/if the object moves (every frame)
	// - You'll notice a "transpose" happening below, which is redundant for
	//   an identity matrix.  This is just to show that HLSL expects a different
	//   matrix (column major vs row major) than the DirectX Math library
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W)); // Transpose for HLSL!

	// Create the View matrix
	// - In an actual game, recreate this matrix every time the camera 
	//    moves (potentially every frame)
	// - We're using the LOOK TO function, which takes the position of the
	//    camera and the direction vector along which to look (as well as "up")
	// - Another option is the LOOK AT function, to look towards a specific
	//    point in 3D space
	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//   the window resizes (which is already happening in OnResize() below)
 // Transpose for HLSL!
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{

	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red	= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green	= XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue	= XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 white = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	Vertex vertices[] = 
	{
		{ XMFLOAT3(+0.0f, +1.0f, +0.0f) },
		{ XMFLOAT3(+1.5f, -1.0f, +0.0f) },
		{ XMFLOAT3(-1.5f, -1.0f, +0.0f) },
	};

	// Set up the indices, which tell us which vertices to use and in which order
	// - This is somewhat redundant for just 3 vertices (it's a simple example)
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2 };
	meshes.push_back(new Mesh(vertices, 3, indices, 3, device));

	Vertex vertices2[] =
	{
		{ XMFLOAT3(-1.0f, +1.0f, +0.0f), XMFLOAT2(0, 0), XMFLOAT3(0, 0, -1) },
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), XMFLOAT2(1, 0), XMFLOAT3(0, 0, -1) },
		{ XMFLOAT3(+1.0f, -1.0f, +0.0f), XMFLOAT2(1, 1), XMFLOAT3(0, 0, -1) },
		{ XMFLOAT3(-1.0f, -1.0f, +0.0f), XMFLOAT2(0, 1), XMFLOAT3(0, 0, -1) },
	};
	unsigned int indices2[] = { 0, 1, 2 ,0,2,3};
	meshes.push_back(new Mesh(vertices2, 4, indices2, 6, device));

	Vertex vertices3[] =
	{
		{ XMFLOAT3(-1.0f, +1.0f, +1.0f) },
		{ XMFLOAT3(-1.0f, +1.0f, -1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, +1.0f) },
		{ XMFLOAT3(-1.0f, +0.0f, -1.0f) },
		{ XMFLOAT3(+1.0f, +1.0f, +1.0f) },
		{ XMFLOAT3(+1.0f, +1.0f, -1.0f) },
		{ XMFLOAT3(+1.0f, -1.0f, +1.0f) },
		{ XMFLOAT3(+1.0f, +.0f, -1.0f)},
	};
	unsigned int indices3[] = {
		0,1,2,
		1,3,2,
		0,2,4,
		4,2,6,
		4,6,7,
		4,7,5,
		0,4,1,
		1,4,5,
		1,7,3,
		1,5,7,
		2,7,6,
		3,7,2
	};
	meshes.push_back(new Mesh(vertices3, 8, indices3, 36, device));
	meshes.push_back(new Mesh("OBJ Files/cone.obj",device));
	meshes.push_back(new Mesh("OBJ Files/cube.obj", device));
	meshes.push_back(new Mesh("OBJ Files/cylinder.obj", device));
	meshes.push_back(new Mesh("OBJ Files/helix.obj", device));
	meshes.push_back(new Mesh("OBJ Files/sphere.obj", device));
	meshes.push_back(new Mesh("OBJ Files/torus.obj", device));
	meshes.push_back(new Mesh("OBJ Files/coin.obj", device));
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	cam->ResizeCam(width, height);
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	cam->Update();
	inputManager.update(deltaTime, emitters);
	
	emitters[0]->setPosition(gameObjects[0]->getPosition());
	emitters[1]->setPosition(gameObjects[0]->getPosition());

	for (Emitter * e : emitters)
	{
		e->Update(deltaTime);
	}

	/*if (gameObjects[0]->getPosition().x < -10)
	{
		gameObjects[0]->setPosition(XMFLOAT3(10, 2, 0));
	}
	if (gameObjects[1]->getPosition().x < -10)
	{
		gameObjects[1]->setPosition(XMFLOAT3(10, -2, 0));
	}*/

	// Move Environment
	renderManager.moveSpotLights(scrollSpeed, deltaTime, &spotLight, &spotLight2);
	gameObjects[4]->Move(scrollSpeed, XMFLOAT3(1, 0, 0));
	gameObjects[5]->Move(scrollSpeed, XMFLOAT3(1, 0, 0));
	gameObjects[6]->Move(scrollSpeed, XMFLOAT3(1, 0, 0));
	gameObjects[7]->Move(scrollSpeed, XMFLOAT3(1, 0, 0));
	gameObjects[8]->Move(scrollSpeed, XMFLOAT3(1, 0, 0));
	gameObjects[9]->Move(scrollSpeed, XMFLOAT3(1, 0, 0));
	if (gameObjects[4]->getPosition().x < -30.0f)
	{
		gameObjects[4]->setPosition(XMFLOAT3(gameObjects[7]->getPosition().x + 40.f, gameObjects[4]->getPosition().y, gameObjects[4]->getPosition().z));
		gameObjects[5]->setPosition(XMFLOAT3(gameObjects[8]->getPosition().x + 40.f, gameObjects[5]->getPosition().y, gameObjects[5]->getPosition().z));
		gameObjects[6]->setPosition(XMFLOAT3(gameObjects[9]->getPosition().x + 40.f, gameObjects[6]->getPosition().y, gameObjects[6]->getPosition().z));
		renderManager.loopSpotLights(&spotLight, &spotLight2, gameObjects[7]->getPosition().x);
	}
	if (gameObjects[7]->getPosition().x < -30.0f)
	{
		gameObjects[7]->setPosition(XMFLOAT3(gameObjects[4]->getPosition().x + 40.f, gameObjects[7]->getPosition().y, gameObjects[7]->getPosition().z));
		gameObjects[8]->setPosition(XMFLOAT3(gameObjects[5]->getPosition().x + 40.f, gameObjects[8]->getPosition().y, gameObjects[8]->getPosition().z));
		gameObjects[9]->setPosition(XMFLOAT3(gameObjects[6]->getPosition().x + 40.f, gameObjects[9]->getPosition().y, gameObjects[9]->getPosition().z));
		renderManager.loopSpotLights(&spotLight, &spotLight2, gameObjects[4]->getPosition().x);
	}
	
	gameObjects[1]->Move(scrollSpeed, XMFLOAT3(1,0,0));
	if (gameObjects[1]->getPosition().x < -10)
	{
		gameObjects[1]->setPosition(XMFLOAT3(10, gameObjects[1]->getPosition().y, 0));
	}

	gameObjects[2]->Move(scrollSpeed, XMFLOAT3(1, 0, 0));
	if (gameObjects[2]->getPosition().x < -10)
	{
		gameObjects[2]->setPosition(XMFLOAT3(10, gameObjects[2]->getPosition().y, 0));
	}

	if (CollisionCheck[gameObjects[0]->getCollider().colliderType][gameObjects[1]->getCollider().colliderType](*gameObjects[0], *gameObjects[1]) ||
		CollisionCheck[gameObjects[0]->getCollider().colliderType][gameObjects[2]->getCollider().colliderType](*gameObjects[0], *gameObjects[2]))
	{
 		gameObjects[0]->setPosition(XMFLOAT3(0, 0, 0));
	}

	if (gameObjects[0]->getPosition().y > 3)
	{
		gameObjects[0]->setPosition(XMFLOAT3(gameObjects[0]->getPosition().x, 3, 0));
	}
	if (gameObjects[0]->getPosition().y < -3)
	{
		gameObjects[0]->setPosition(XMFLOAT3(gameObjects[0]->getPosition().x, -3, 0));
	}

	static_cast<Enemy*>(gameObjects[3])->update(*gameObjects[0], 7.5f, deltaTime);
	if (CollisionCheck[gameObjects[0]->getCollider().colliderType][gameObjects[3]->getCollider().colliderType](*gameObjects[0], *gameObjects[3]) ||
		gameObjects[3]->getPosition().x < -10)
	{
		gameObjects[3]->setPosition(XMFLOAT3(10, 0, 0));
	}

	// Move Coins
	for (int i = 11; i < 20; i++)
	{
		gameObjects[i]->Move(-5 - (5 * speedBoost), XMFLOAT3(1, 0, 0));
		emitters[i - 9]->setPosition(gameObjects[i]->getPosition());
	} 
	// Coin Animations
	for (int i = 11; i < 20; i++)
	{
		if (gameObjects[i]->getScale().x < 0.1f)
		{
			coinCollected[i - 11].set();
		}
	}
	// Coin Pickups
	for (int i = 11; i < 20; i++)
	{
		if (coinCollected[i - 11].to_ulong() == 0)
		{
			if (CollisionCheck[gameObjects[0]->getCollider().colliderType][gameObjects[i]->getCollider().colliderType](*gameObjects[0], *gameObjects[i]))
			{
				coinCollected[i - 11].set();
				emitters[i-9]->setPosition(gameObjects[i]->getPosition());
				emitters[i-9]->shouldDraw = false;
				speedBoost = true;
				scrollSpeed = -10;
			}
		}
	}
	// Loop Coins
	if (gameObjects[13]->getPosition().x < -7.87)
	{
		// Reset Position
		for (int i = 11; i < 20; i++)
		{
			gameObjects[11]->setPosition(XMFLOAT3(10, gameObjects[11]->getPosition().y, 0));
			gameObjects[12]->setPosition(XMFLOAT3(11, gameObjects[12]->getPosition().y, 0));
			gameObjects[13]->setPosition(XMFLOAT3(12, gameObjects[13]->getPosition().y, 0));
			gameObjects[14]->setPosition(XMFLOAT3(10, gameObjects[14]->getPosition().y, 0));
			gameObjects[15]->setPosition(XMFLOAT3(11, gameObjects[15]->getPosition().y, 0));
			gameObjects[16]->setPosition(XMFLOAT3(12, gameObjects[16]->getPosition().y, 0));
			gameObjects[17]->setPosition(XMFLOAT3(10, gameObjects[17]->getPosition().y, 0));
			gameObjects[18]->setPosition(XMFLOAT3(11, gameObjects[18]->getPosition().y, 0));
			gameObjects[19]->setPosition(XMFLOAT3(12, gameObjects[19]->getPosition().y, 0));
		

		// Reset Collected

			if (coinCollected[i - 11].to_ulong() == 1)
			{
				coinCollected[i - 11].set(0, 0);
				emitters[i - 9]->setPosition(gameObjects[i]->getPosition());
				emitters[i - 9]->shouldDraw = true;
			}
		}
	}

	for (Entity * gameObject : gameObjects)
	{
		gameObject->update(deltaTime);
	}

	if (speedBoost)
	{
		boostTime += deltaTime;
		if (boostTime >= boostDuration)
		{
			boostTime = 0;
			speedBoost = false;
			scrollSpeed = -5;
		}
	}
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Set Data that is the same for the entire scene

	renderManager.RenderSpot1ShadowMap(context, &gameObjects, backBufferRTV, depthStencilView, &width, &height);
	renderManager.RenderSpot2ShadowMap(context, &gameObjects, backBufferRTV, depthStencilView, &width, &height);
	renderManager.RenderDirShadowMap(context, &gameObjects, backBufferRTV, depthStencilView, &width, &height);

	renderManager.setSceneData(cam, dirLight, dirLight2, pointLight, spotLight, spotLight2);

	renderManager.DrawAll(context, gameObjects, cam, emitters, backBufferRTV, depthStencilView, width, height, coinCollected, speedBoost);

	// Unbind the shadow map so we don't have resource conflicts
	// at the start of the next frame
	gameObjects[0]->getMat()->getPixelShader()->SetShaderResourceView("ShadowMap", 0);

	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	inputManager.onMouseDown(buttonState, x, y, &hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	inputManager.onMouseUp(buttonState, x, y, cam);
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	inputManager.onMouseMove(buttonState, x, y, cam);
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	inputManager.onMouseWheel(wheelDelta, x, y);
}
#pragma endregion