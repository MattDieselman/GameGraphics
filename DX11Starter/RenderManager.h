#pragma once
#include <vector>
#include "DXCore.h"
#include "Material.h"
#include "Entity.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include <WICTextureLoader.h>

class RenderManager{

private:
	std::vector<ID3D11ShaderResourceView*> textures;
	std::vector<Material*> materials;
	//std::vector<DirectionalLight> lights;

public:
	RenderManager();
	~RenderManager();

	std::vector<Material*> getMaterials();

	void LoadShaders(ID3D11Device* device,ID3D11DeviceContext* context, std::vector<DirectionalLight> lights);
	void DrawAll(ID3D11DeviceContext* context, float deltaTime, float totalTime, std::vector<Entity*> gameObjects,Camera * cam, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView);
	
};

