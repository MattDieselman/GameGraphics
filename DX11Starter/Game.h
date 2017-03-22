#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "DirectionalLight.h"

class Game 
	: public DXCore
{

public:
	ID3D11ShaderResourceView * shaderView;
	ID3D11ShaderResourceView * shaderView2;
	ID3D11ShaderResourceView * shaderView3;

	ID3D11SamplerState * sampState;
	D3D11_SAMPLER_DESC sampDesc;

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
	Mesh * mesh1;
	Mesh * mesh2;
	Mesh * mesh3;
	Mesh * mesh4;
	Mesh * mesh5;
	Mesh * mesh6;
	Mesh * mesh7;
	Mesh * mesh8;
	Mesh * mesh9;

	DirectionalLight light;
	DirectionalLight light2;

	Camera * cam;
	Entity * entity1;
	Entity * entity2;
	Entity * entity3;
	Material * Mat1;
	Material * Mat2;
	Material * Mat3;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void CreateBasicGeometry();

	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	
	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT3 worldUp;
	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};

