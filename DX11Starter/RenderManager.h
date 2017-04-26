#pragma once
#include <vector>
#include "DXCore.h"
#include "Entity.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include <WICTextureLoader.h>

class RenderManager{

private:
	std::vector<ID3D11ShaderResourceView*> textures;
	std::vector<ID3D11ShaderResourceView*> normalMaps;
	std::vector<Material*> materials;
	//std::vector<DirectionalLight> lights;

	// Post-processing requirements
	ID3D11RenderTargetView* ppRTV;
	ID3D11ShaderResourceView* ppSRV;
	ID3D11SamplerState* ppSampler;
	SimpleVertexShader* ppVS;
	SimplePixelShader* ppPS;

public:
	RenderManager();
	~RenderManager();

	//void init(unsigned int width, unsigned int height);

	std::vector<Material*> getMaterials();

	void setSceneData(Camera* cam, DirectionalLight dirLight, PointLight pointLight, SpotLight spotLight);
	void setObjData(Entity* object);

	void LoadShaders(ID3D11Device* device, ID3D11DeviceContext* context, unsigned int width, unsigned int height);
	void DrawAll(ID3D11DeviceContext* context, float deltaTime, float totalTime, std::vector<Entity*> gameObjects,Camera * cam, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView, unsigned int  width, unsigned int height);
	
};

