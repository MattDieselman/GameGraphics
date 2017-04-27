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
	dirLight.ambientColor = XMFLOAT4(0.1, 0.1, 0.1, 1.0);
	dirLight.diffuseColor = XMFLOAT4(1, 0, 0, 1);
	dirLight.direction = XMFLOAT3(-1, -1, 0);
	
	pointLight.ambientColor = XMFLOAT4(0.1, 0.1, 0.1, 1.0);
	pointLight.diffuseColor = XMFLOAT4(0, 0, 1, 1);
	pointLight.location = XMFLOAT3(0, 0, 0);

	spotLight.ambientColor = XMFLOAT4(0.1, 0.1, 0.1, 1.0);
	spotLight.diffuseColor = XMFLOAT4(1, 1, 1, 1);
	spotLight.location = XMFLOAT3(0, 3.5, 0);
	spotLight.direction = XMFLOAT3(0, -1, 0);
	spotLight.angle = XM_PI / 8;

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	//LoadShaders();
	renderManager.LoadShaders(device, context, width, height);
	renderManager.InitShadows(device, context);

	materials = renderManager.getMaterials();

	CreateMatrices();
	CreateBasicGeometry();

	renderManager.screen = meshes[1]->copy();

	// Create Game Objects
	gameObjects.push_back(new Entity(meshes[7]->copy(), materials[3]->copy()));
	gameObjects[0]->init(ColliderType::SPHERE, 1.f);
	gameObjects.push_back(new Entity(meshes[4]->copy(), materials[0]->copy()));
	gameObjects[1]->init(ColliderType::AABB, 1.f);
	gameObjects[1]->setPosition(XMFLOAT3(5, 2, 0));
	gameObjects[1]->setScale(XMFLOAT3(1, 2, 1));
	gameObjects.push_back(new Entity(meshes[4]->copy(), materials[1]->copy()));
	gameObjects[2]->init(ColliderType::AABB, 1.f);
	gameObjects[2]->setPosition(XMFLOAT3(-5, -2, 0));
	gameObjects[2]->setScale(XMFLOAT3(1, 2, 1));
	gameObjects.push_back(new Enemy(meshes[7]->copy(), materials[2]->copy()));
	gameObjects[3]->init(ColliderType::SPHERE, 1.f);
	gameObjects[3]->setPosition(XMFLOAT3(10, 0, 0));

	// Environment
	gameObjects.push_back(new Enemy(meshes[1]->copy(), materials[1]->copy()));
	gameObjects[4]->init(ColliderType::SPHERE, 1.f);
	gameObjects[4]->setPosition(XMFLOAT3(0, 0, 2));
	gameObjects[4]->setScale(XMFLOAT3(10, 3, 1));
	gameObjects.push_back(new Enemy(meshes[1]->copy(), materials[1]->copy()));
	gameObjects[5]->init(ColliderType::SPHERE, 1.f);
	gameObjects[5]->setPosition(XMFLOAT3(0, 2.5, 2));
	gameObjects[5]->setRotation(XMFLOAT3(XM_PI / 3, 0, 0));
	gameObjects[5]->setScale(XMFLOAT3(10, 5, 1));
	gameObjects.push_back(new Enemy(meshes[1]->copy(), materials[1]->copy()));
	gameObjects[6]->init(ColliderType::SPHERE, 1.f);
	gameObjects[6]->setPosition(XMFLOAT3(0, -3, 2));
	gameObjects[6]->setRotation(XMFLOAT3(-XM_PI / 3, 0, 0));
	gameObjects[6]->setScale(XMFLOAT3(10, 5, 1));

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
	emitter = new Emitter(
		1000,							// Max particles
		100,							// Particles per second
		1,								// Particle lifetime
		0.1f,							// Start size
		5.0f,							// End size
		XMFLOAT4(1, 0.1f, 0.1f, 0.2f),	// Start color
		XMFLOAT4(1, 0.6f, 0.1f, 0),		// End color
		XMFLOAT3(0, -1, 0),				// Start velocity
		gameObjects[0]->getPosition(),				// Start position
		XMFLOAT3(0, -10, 0),				// Start acceleration
		device,
		renderManager.getPartVert(),
		renderManager.getPartPix(),
		renderManager.getPartText(0));
	emitter2 = new Emitter(
		1000,							// Max particles
		100,							// Particles per second
		5,								// Particle lifetime
		0.1f,							// Start size
		5.0f,							// End size
		XMFLOAT4(0.1f, 0.1f, 0.1f, .10f),	// Start color
		XMFLOAT4(.1f, .1f, .1f, 0),		// End color
		XMFLOAT3(0, -1, 0),				// Start velocity
		gameObjects[0]->getPosition(),				// Start position
		XMFLOAT3(0, -.1, 0),				// Start acceleration
		device,
		renderManager.getPartVert(),
		renderManager.getPartPix(),
		renderManager.getPartText(1));

	emitters.push_back(emitter);
	emitters.push_back(emitter2);


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
	inputManager.update(deltaTime);
	
	for (Emitter * e : emitters)
	{
		e->setPosition(gameObjects[0]->getPosition());
		e->Update(deltaTime);
	}

	/*for (Obstacle * o : objects) {
		o->Move(-.005, XMFLOAT3(1, 0, 0));
	}*/

	/*if (gameObjects[0]->getPosition().x < -10)
	{
		gameObjects[0]->setPosition(XMFLOAT3(10, 2, 0));
	}
	if (gameObjects[1]->getPosition().x < -10)
	{
		gameObjects[1]->setPosition(XMFLOAT3(10, -2, 0));
	}*/

	gameObjects[1]->Move(-5 * deltaTime, XMFLOAT3(1,0,0));
	if (gameObjects[1]->getPosition().x < -10)
	{
		gameObjects[1]->setPosition(XMFLOAT3(10, 2, 0));
	}

	gameObjects[2]->Move(-5 * deltaTime, XMFLOAT3(1, 0, 0));
	if (gameObjects[2]->getPosition().x < -10)
	{
		gameObjects[2]->setPosition(XMFLOAT3(10, -2, 0));
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
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Set Data that is the same for the entire scene

	//renderManager.RenderShadowMap(context, &gameObjects, backBufferRTV, depthStencilView, &width, &height);

	renderManager.setSceneData(cam, dirLight, pointLight, spotLight);

	renderManager.DrawAll(context, gameObjects, cam, emitters, backBufferRTV, depthStencilView, width, height);

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