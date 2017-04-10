#pragma once

#include <vector>

#include "DXCore.h"
#include "PhysicsManager.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "Obstacle.h"

class Game 
	: public DXCore
{

public:

	// ID3D11SamplerState * sampState;
	// D3D11_SAMPLER_DESC sampDesc;

	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);

private:
	std::vector<ID3D11ShaderResourceView*> textures;

	std::vector<Mesh*> meshes;

	std::vector<DirectionalLight> lights;

	Camera * cam;

	std::vector<Entity*> gameObjects;
	// std::vector<Obstacle*> objects;
	std::vector<Material*> materials;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void CreateBasicGeometry();
	
	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT3 worldUp;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	// Managers
	PhysicsManager physicsManager;
};

