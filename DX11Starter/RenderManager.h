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
#include <time.h>
#include <bitset>

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
	float lastTime;
	int shadowMapSize;
	SimpleVertexShader* shadowVertexShader;
	ID3D11SamplerState* shadowSampler;
	ID3D11RasterizerState* shadowRasterizer;
	ID3D11DepthStencilView* spot1ShadowDSV;
	ID3D11ShaderResourceView* spot1ShadowSRV;
	DirectX::XMFLOAT4X4 spot1ShadowViewMatrix;
	DirectX::XMFLOAT4X4 spot1ShadowProjectionMatrix;
	ID3D11DepthStencilView* spot2ShadowDSV;
	ID3D11ShaderResourceView* spot2ShadowSRV;
	DirectX::XMFLOAT4X4 spot2ShadowViewMatrix;
	DirectX::XMFLOAT4X4 spot2ShadowProjectionMatrix;
	ID3D11DepthStencilView* dirShadowDSV;
	ID3D11ShaderResourceView* dirShadowSRV;
	DirectX::XMFLOAT4X4 dirShadowViewMatrix;
	DirectX::XMFLOAT4X4 dirShadowProjectionMatrix;

public:
	RenderManager();
	~RenderManager();

	//void init(unsigned int width, unsigned int height);

	Mesh* screen;

	std::vector<Material*> getMaterials();
	void setSceneData(Camera* cam, DirectionalLight dirLight, DirectionalLight dirLight2, PointLight pointLight, SpotLight spotLight, SpotLight spotLight2);
	void setObjData(Entity* object);

	void LoadShaders(ID3D11Device* device, ID3D11DeviceContext* context, unsigned int width, unsigned int height);
	void DrawAll(ID3D11DeviceContext* context, std::vector<Entity*> gameObjects,Camera * cam, std::vector<Emitter*> emitters,  ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView, unsigned int  width, unsigned int height, std::vector<std::bitset<1>> coinCollected);

	SimpleVertexShader * getPartVert();
	SimplePixelShader * getPartPix();
	ID3D11ShaderResourceView* getPartText(int index);

	void InitShadows(ID3D11Device* device, ID3D11DeviceContext* context, SpotLight* spotLight, SpotLight* spotLight2, DirectionalLight* dirLight);
	void RenderSpot1ShadowMap(ID3D11DeviceContext* context, std::vector<Entity*>* gameObjects, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView, unsigned int* width, unsigned int* height);
	void RenderSpot2ShadowMap(ID3D11DeviceContext* context, std::vector<Entity*>* gameObjects, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView, unsigned int* width, unsigned int* height);
	void RenderDirShadowMap(ID3D11DeviceContext* context, std::vector<Entity*>* gameObjects, ID3D11RenderTargetView* backBufferRTV, ID3D11DepthStencilView* depthStencilView, unsigned int* width, unsigned int* height);
	void DefaultLastTime();
	void moveSpotLights(float deltaTime, SpotLight* spotLight, SpotLight* spotLight2);
	void loopSpotLights(SpotLight* spotLight, SpotLight* spotLight2, float x);
	void rotateSpotLights(int x, int y, int z, float radian, SpotLight* spotLight, SpotLight* spotLight2);
	
	void coinSpinShrink(float totalTime, float radian, Entity* coinObj);
};

