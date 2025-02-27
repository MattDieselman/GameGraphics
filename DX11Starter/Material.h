#pragma once

#include <map>

#include "SimpleShader.h"

namespace
{
	std::map<void*, int> matRefCount;
	std::map<void*, int> shaderRefCount;
	std::map<void*, int> textureRefCount;
	std::map<void*, int> samplerRefCount;
}

class Material
{
public:
	Material(SimpleVertexShader* vs, SimplePixelShader* ps);

	void AttatchTexture(ID3D11ShaderResourceView* tex, ID3D11SamplerState* sam);
	void AttatchNormalMap(ID3D11ShaderResourceView* nMap);

	SimpleVertexShader* getVertexShader();
	SimplePixelShader* getPixelShader();
	ID3D11ShaderResourceView* getTexture();
	ID3D11ShaderResourceView* getNormalMap();
	ID3D11SamplerState* getSampler();

	Material* copy();
	void release();

private:
	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	ID3D11ShaderResourceView* texture;
	ID3D11ShaderResourceView* normalMap;
	ID3D11SamplerState* sampler;

	~Material();
};

