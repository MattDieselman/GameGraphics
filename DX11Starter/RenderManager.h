#pragma once
#include <vector>
#include "DXCore.h"
#include "Entity.h"
#include "Camera.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include <WICTextureLoader.h>
#include "Emitter.h"

class RenderManager{

private:
	std::vector<ID3D11ShaderResourceView*> textures;
	std::vector<ID3D11ShaderResourceView*> normalMaps;
	std::vector<Material*> materials;
	std::vector<ID3D11ShaderResourceView*>particleTextures;


	SimpleVertexShader* partVertexShader;
	SimplePixelShader* partPixelShader;
	ID3D11DepthStencilState* particleDepthState;
	ID3D11BlendState* particleBlendState;

	//std::vector<DirectionalLight> lights;

public:
	RenderManager();
	~RenderManager();

	std::vector<Material*> getMaterials();
	void setSceneData(Camera* cam, DirectionalLight dirLight, PointLight pointLight, SpotLight spotLight);
	void setObjData(Entity* object);

	SimpleVertexShader * getPartVert();
	SimplePixelShader * getPartPix();
	ID3D11ShaderResourceView* getPartText(int index);


	void LoadShaders(ID3D11Device* device, ID3D11DeviceContext* context);
	void DrawAll(ID3D11DeviceContext* context, float deltaTime, float totalTime, std::vector<Entity*> gameObjects,Camera * cam,std::vector<Emitter*> emitters, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView);
	

};

