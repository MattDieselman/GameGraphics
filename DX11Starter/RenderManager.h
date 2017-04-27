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

	// Particle requirements
	SimpleVertexShader* partVertexShader;
	SimplePixelShader* partPixelShader;
	ID3D11DepthStencilState* particleDepthState;
	ID3D11BlendState* particleBlendState;

	//std::vector<DirectionalLight> lights;

	// Post-processing requirements
	ID3D11RenderTargetView* ppRTV;
	ID3D11ShaderResourceView* ppSRV;
	ID3D11SamplerState* ppSampler;
	SimpleVertexShader* ppVS;
	SimplePixelShader* ppPS;

	// Shadow requirements
	int shadowMapSize;
	ID3D11DepthStencilView* shadowDSV;
	ID3D11ShaderResourceView* shadowSRV;
	ID3D11SamplerState* shadowSampler;
	ID3D11RasterizerState* shadowRasterizer;
	SimpleVertexShader* shadowVertexShader;
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;

public:
	RenderManager();
	~RenderManager();

	//void init(unsigned int width, unsigned int height);

	Mesh* screen;

	std::vector<Material*> getMaterials();
	void setSceneData(Camera* cam, DirectionalLight dirLight, PointLight pointLight, SpotLight spotLight);
	void setObjData(Entity* object);

	void LoadShaders(ID3D11Device* device, ID3D11DeviceContext* context, unsigned int width, unsigned int height);
	void DrawAll(ID3D11DeviceContext* context, std::vector<Entity*> gameObjects,Camera * cam, std::vector<Emitter*> emitters,  ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView, unsigned int  width, unsigned int height);

	SimpleVertexShader * getPartVert();
	SimplePixelShader * getPartPix();
	ID3D11ShaderResourceView* getPartText(int index);

	void InitShadows(ID3D11Device* device, ID3D11DeviceContext* context);
	void RenderShadowMap(ID3D11DeviceContext* context, std::vector<Entity*>* gameObjects, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView, unsigned int* width, unsigned int* height);
};

