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
	
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	lights.push_back(DirectionalLight());
	lights[0].ambientColor = XMFLOAT4(0.1, 0.1, 0.1, 1.0);
	lights[0].diffuseColor = XMFLOAT4(1, 0, 0, 1);
	lights[0].direction = XMFLOAT3(1, 0, 0);
	
	lights.push_back(DirectionalLight());
	lights[1].ambientColor = XMFLOAT4(0.1, 0.1, 0.1, 1.0);
	lights[1].diffuseColor = XMFLOAT4(0,1, 1, 1);
	lights[1].direction = XMFLOAT3(-1, -10, 1);

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	//LoadShaders();
	renderManager.LoadShaders(device,context,lights);
	materials = renderManager.getMaterials();

	CreateMatrices();
	CreateBasicGeometry();
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	//SimpleVertexShader* vertexShader = new SimpleVertexShader(device, context);
	//if (!vertexShader->LoadShaderFile(L"Debug/VertexShader.cso"))
	//	vertexShader->LoadShaderFile(L"VertexShader.hlsl");	

	//SimplePixelShader* pixelShader = new SimplePixelShader(device, context);
	//if(!pixelShader->LoadShaderFile(L"Debug/PixelShader.cso"))	
	//	pixelShader->LoadShaderFile(L"PixelShader.hlsl");
	//
	////Set Lighting

	//pixelShader->SetData(
	//	"light",
	//	&lights[0],
	//	sizeof(DirectionalLight));
	//pixelShader->SetData(
	//	"light2",
	//	&lights[1],
	//	sizeof(DirectionalLight));
	//pixelShader->CopyAllBufferData();

	//ID3D11ShaderResourceView* texture1;
	//CreateWICTextureFromFile(device, context, L"textures/sphere.png", 0, &texture1);
	//ID3D11ShaderResourceView* texture2;
	//CreateWICTextureFromFile(device, context, L"textures/box.png", 0, &texture2);
	//ID3D11ShaderResourceView* texture3;
	//CreateWICTextureFromFile(device, context, L"textures/disc.png", 0, &texture3);

	//textures.push_back(texture1);
	//textures.push_back(texture2);
	//textures.push_back(texture3);

	//ID3D11SamplerState* sampler;
	//D3D11_SAMPLER_DESC sampDesc = {};
	//sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//sampDesc.MaxAnisotropy = 16;
	//sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//device->CreateSamplerState(&sampDesc, &sampler);


	//materials.push_back(new Material(vertexShader, pixelShader));
	//materials[0]->AttatchTexture(textures[0], sampler);

	//materials.push_back(new Material(vertexShader, pixelShader));
	//materials[1]->AttatchTexture(textures[1], sampler);

	//materials.push_back(new Material(vertexShader, pixelShader));
	//materials[2]->AttatchTexture(textures[2], sampler);

	// You'll notice that the code above attempts to load each
	// compiled shader file (.cso) from two different relative paths.

	// This is because the "working directory" (where relative paths begin)
	// will be different during the following two scenarios:
	//  - Debugging in VS: The "Project Directory" (where your .cpp files are) 
	//  - Run .exe directly: The "Output Directory" (where the .exe & .cso files are)

	// Checking both paths is the easiest way to ensure both 
	// scenarios work correctly, although others exist
}



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
		{ XMFLOAT3(+2.0f, +1.0f, +0.0f) },
		{ XMFLOAT3(+2.0f, +0.0f, +0.0f) },
		{ XMFLOAT3(+1.0f, +0.0f, +0.0f) },
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f) },
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

	gameObjects.push_back(new Entity(meshes[7]->copy(), materials[1]->copy(), ColliderType::SPHERE));
	gameObjects.push_back(new Entity(meshes[4]->copy(), materials[0]->copy(), ColliderType::AABB));
	gameObjects[1]->setPosition(XMFLOAT3(5, 2, 0));
	gameObjects[1]->setScale(XMFLOAT3(1, 2, 1));
	gameObjects.push_back(new Entity(meshes[4]->copy(), materials[2]->copy(), ColliderType::AABB));
	gameObjects[2]->setPosition(XMFLOAT3(-5, -2, 0));
	gameObjects[2]->setScale(XMFLOAT3(1, 2, 1));

	/*objects.push_back(new Obstacle(meshes[8]->copy(),materials[2]->copy(), ColliderType::AABB));
	objects[0]->setPosition(XMFLOAT3(5, 2, 0));
	objects[0]->setScale(XMFLOAT3(1, 2, 1));
	objects.push_back(new Obstacle(meshes[8]->copy(), materials[2]->copy(), ColliderType::AABB));
	objects[1]->setPosition(XMFLOAT3(-5, -2, 0));
	objects[1]->setScale(XMFLOAT3(1, 2, 1));*/
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
	inputManager.update(gameObjects[0]);

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

	gameObjects[1]->Move(-.005, XMFLOAT3(1,0,0));
	if (gameObjects[1]->getPosition().x < -10)
	{
		gameObjects[1]->setPosition(XMFLOAT3(10, 2, 0));
	}

	gameObjects[2]->Move(-.005, XMFLOAT3(1, 0, 0));
	if (gameObjects[2]->getPosition().x < -10)
	{
		gameObjects[2]->setPosition(XMFLOAT3(10, -2, 0));
	}
	/*
	if (GetAsyncKeyState(' ') & 0x8000) {
		gameObjects[0]->Move(0.015, XMFLOAT3(0, 1, 0));
	}
	else
	{
		gameObjects[0]->Move(0.01, XMFLOAT3(0, -1, 0));
	}
	*/
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

	if (GetAsyncKeyState('X') & 0x8000) {
		cam->MoveYAxis(-.5*deltaTime);
	}
	if (GetAsyncKeyState('W') & 0x8000) {
		cam->Move(cam->getDir(), 1 * deltaTime);
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		XMVECTOR temp1;
		XMFLOAT3 temp2;
		temp1 = DirectX::XMVectorNegate(XMLoadFloat3(&cam->getDir()));
		XMStoreFloat3(&temp2, temp1);
		cam->Move(temp2, 1*deltaTime);
	}
	if (GetAsyncKeyState('A') & 0x8000) {

		XMVECTOR tempDir = XMLoadFloat3(&cam->getDir());
		XMVECTOR tempUp = XMLoadFloat3(&worldUp);
		XMFLOAT3 tempDest;
		XMStoreFloat3(&tempDest, DirectX::XMVector3Cross(tempDir, tempUp));
		cam->Move(tempDest, 1 * deltaTime);
	}
	if (GetAsyncKeyState('D') & 0x8000) {

		XMVECTOR tempDir = XMLoadFloat3(&cam->getDir());
		XMVECTOR tempUp = XMLoadFloat3(&worldUp);
		XMVECTOR tempNeg = DirectX::XMVectorNegate(DirectX::XMVector3Cross(tempDir, tempUp));
		XMFLOAT3 tempDest;
		XMStoreFloat3(&tempDest, tempNeg);
		cam->Move(tempDest, 1 * deltaTime);
	}
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = {0.4f, 0.6f, 0.75f, 0.0f};

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
	renderManager.DrawAll( context, deltaTime, totalTime, gameObjects, cam, backBufferRTV, depthStencilView);
	//for (Obstacle * object : objects) {
	//	object->getMat()->getVertexShader()->SetMatrix4x4("world", object->getWorld());//entity1->world);
	//	object->getMat()->getVertexShader()->SetMatrix4x4("view", cam->getView());
	//	object->getMat()->getVertexShader()->SetMatrix4x4("projection", cam->getProj());
	//	object->getMat()->getVertexShader()->CopyAllBufferData();
	//	object->getMat()->getVertexShader()->SetShader();

	//	object->getMat()->getPixelShader()->SetShaderResourceView("diffuseTexture", object->getMat()->getTexture());
	//	object->getMat()->getPixelShader()->SetSamplerState("sampState", object->getMat()->getSampler());
	//	object->getMat()->getPixelShader()->CopyAllBufferData();
	//	object->getMat()->getPixelShader()->SetShader();

	//	UINT stride = sizeof(Vertex);
	//	UINT offset = 0;
	//	ID3D11Buffer *verts = object->getMesh()->getVertexBuffer();
	//	context->IASetVertexBuffers(0, 1, &verts, &stride, &offset);
	//	context->IASetIndexBuffer(object->getMesh()->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	//	context->DrawIndexed(
	//		object->getMesh()->getIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
	//		0,     // Offset to the first index we want to use
	//		0);    // Offset to add to each index when looking up vertices
	//}

	//for each (Entity* object in gameObjects)
	//{
	//	// Send data to shader variables
	//	//  - Do this ONCE PER OBJECT you're drawing
	//	//  - This is actually a complex process of copying data to a local buffer
	//	//    and then copying that entire buffer to the GPU.  
	//	//  - The "SimpleShader" class handles all of that for you.

	//	object->getMat()->getVertexShader()->SetMatrix4x4("world", object->getWorld());//entity1->world);
	//	object->getMat()->getVertexShader()->SetMatrix4x4("view", cam->getView());
	//	object->getMat()->getVertexShader()->SetMatrix4x4("projection", cam->getProj());
	//	object->getMat()->getVertexShader()->CopyAllBufferData();
	//	object->getMat()->getVertexShader()->SetShader();

	//	// Once you've set all of the data you care to change for
	//	// the next draw call, you need to actually send it to the GPU
	//	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!

	//	// Set the vertex and pixel shaders to use for the next Draw() command
	//	//  - These don't technically need to be set every frame...YET
	//	//  - Once you start applying different shaders to different objects,
	//	//    you'll need to swap the current shaders before each draw
	//	object->getMat()->getPixelShader()->SetShaderResourceView("diffuseTexture", object->getMat()->getTexture());
	//	object->getMat()->getPixelShader()->SetSamplerState("sampState", object->getMat()->getSampler());
	//	object->getMat()->getPixelShader()->CopyAllBufferData();
	//	object->getMat()->getPixelShader()->SetShader();


	//	// Set buffers in the input assembler
	//	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//	//    have different geometry.
	//	UINT stride = sizeof(Vertex);
	//	UINT offset = 0;
	//	ID3D11Buffer *verts = object->getMesh()->getVertexBuffer();
	//	context->IASetVertexBuffers(0, 1, &verts, &stride, &offset);
	//	context->IASetIndexBuffer(object->getMesh()->getIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	//	context->DrawIndexed(
	//		object->getMesh()->getIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
	//		0,     // Offset to the first index we want to use
	//		0);    // Offset to add to each index when looking up vertices
	//}
	//
	//// Present the back buffer to the user
	////  - Puts the final frame we're drawing into the window so the user can see it
	////  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
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