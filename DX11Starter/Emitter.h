#pragma once
#include <DirectXMath.h>
#include "DXCore.h"
#include "Camera.h"
#include "SimpleShader.h"
using namespace DirectX;

//Structs

struct Particle {
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT3 StartVelocity;
	float Size;
	float Age;
};

struct ParticleVertex {
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT4 Color;
	float Size;
};



class Emitter
{
private:
	//Properties
	int particlesPerSec;
	float secsPerParticle;
	float timeSinceEmit;

	int liveParticles;
	float lifetime;

	DirectX::XMFLOAT3 emitterAccel;
	DirectX::XMFLOAT3 emitterPos;
	DirectX::XMFLOAT3 startVel;
	DirectX::XMFLOAT4 startColor;
	DirectX::XMFLOAT4 endColor;
	float startSize;
	float endSize;

	Particle* particles;
	int maxParticles;
	int firstDeadIndex;
	int firstAliveIndex;

	ParticleVertex* localPartVerts;
	ID3D11Buffer* vertBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11ShaderResourceView* texture;
	SimpleVertexShader* vertShader;
	SimplePixelShader* pixShader;

public:

	bool shouldDraw;
	bool isWorld;
	Emitter();
	~Emitter();

	Emitter(int maxParticles_,
		int particlesPerSecond_,
		float lifetime_,
		float startSize_,
		float endSize_,
		DirectX::XMFLOAT4 startColor_,
		DirectX::XMFLOAT4 endColor_,
		DirectX::XMFLOAT3 startVelocity_,
		DirectX::XMFLOAT3 emitterPosition_,
		DirectX::XMFLOAT3 emitterAcceleration_,
		ID3D11Device* device_,
		SimpleVertexShader* vertShader_,
		SimplePixelShader* pixShader_,
		ID3D11ShaderResourceView* texture_);

	void setPosition(DirectX::XMFLOAT3 position);
	void randomizeVelocity();
	void Update(float dt);
	void UpdateSinglePart(float dt, int index);
	void SpawnPart();

	void CopyPartsToGPU(ID3D11DeviceContext* context);
	void CopyPart(int index);
	void DrawAll(ID3D11DeviceContext* context, Camera* camera);


};

