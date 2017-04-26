#include "Material.h"

Material::Material(SimpleVertexShader * vs, SimplePixelShader * ps)
{
	// Increment reference count
	matRefCount[this]++;
	shaderRefCount[vs]++;
	shaderRefCount[ps]++;

	// Cast the shader pointers to the same address as the parameters
	vertexShader = vs;
	pixelShader = ps;
}

void Material::AttatchTexture(ID3D11ShaderResourceView * tex, ID3D11SamplerState * sam)
{
	textureRefCount[tex]++;
	samplerRefCount[sam]++;

	texture = tex;
	sampler = sam;
}

void Material::AttatchNormalMap(ID3D11ShaderResourceView * nMap)
{
	textureRefCount[nMap]++;
	normalMap = nMap;
}

Material::~Material()
{
	--shaderRefCount[vertexShader];
	--shaderRefCount[pixelShader];
	--textureRefCount[texture];
	--textureRefCount[normalMap];
	--samplerRefCount[sampler];

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	if (shaderRefCount[vertexShader] == 0) { delete vertexShader; }
	if (shaderRefCount[pixelShader] == 0) { delete pixelShader;	}
	if (textureRefCount[texture] == 0) { texture->Release(); }
	if (textureRefCount[normalMap] == 0) { normalMap->Release(); }
	if (samplerRefCount[sampler] == 0) { sampler->Release(); };
}

SimpleVertexShader * Material::getVertexShader()
{
	return vertexShader;
}

SimplePixelShader * Material::getPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView * Material::getTexture()
{
	return texture;
}

ID3D11ShaderResourceView * Material::getNormalMap()
{
	return normalMap;
}

ID3D11SamplerState * Material::getSampler()
{
	return sampler;
}

Material * Material::copy()
{
	matRefCount[this]++;
	return this;
}

void Material::release()
{
	--matRefCount[this];
	if (matRefCount[this] == 0)
	{
		delete this;
	}
}