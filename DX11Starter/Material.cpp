#include "Material.h"

Material::Material(SimpleVertexShader * vs, SimplePixelShader * ps) :
	vertexShader(vs), pixelShader(ps)
{
	// Increment reference count
	matRefCount[this]++;
}

void Material::AttatchTexture(ID3D11ShaderResourceView * tex, ID3D11SamplerState * sam)
{
	texture = tex;
	sampler = sam;
}

Material::~Material()
{
	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	if (vertexShader != nullptr) { delete vertexShader; }
	if (pixelShader != nullptr) { delete pixelShader; }
	if (pixelShader != nullptr) { texture->Release(); }
	if (sampler != nullptr) { sampler->Release(); };
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