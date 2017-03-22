#pragma once
#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>

class Material
{
private:
	SimpleVertexShader * shader;
	SimplePixelShader * pixel;
	ID3D11ShaderResourceView * shaderView;
	ID3D11SamplerState * sampState;
public:
	SimpleVertexShader * getShader();
	SimplePixelShader * getPixel();
	ID3D11ShaderResourceView * getShaderView();
	ID3D11SamplerState * getSampState();
	Material(SimpleVertexShader * shade_, SimplePixelShader* pix_, ID3D11ShaderResourceView * shaderView_, ID3D11SamplerState* sampState_);
	Material();

	~Material();
};

