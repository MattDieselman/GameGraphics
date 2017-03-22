#include "Material.h"



SimpleVertexShader * Material::getShader()
{
	return shader;
}

SimplePixelShader * Material::getPixel()
{
	return pixel;
}

ID3D11ShaderResourceView * Material::getShaderView()
{
	return shaderView;
}

ID3D11SamplerState * Material::getSampState()
{
	return sampState;
}

Material::Material(SimpleVertexShader * shade_, SimplePixelShader * pix_, ID3D11ShaderResourceView * shaderView_, ID3D11SamplerState* sampState_)
{
	shader = shade_;
	pixel = pix_;
	shaderView = shaderView_;
	sampState = sampState_;
}

Material::Material()
{
}


Material::~Material()
{
}
