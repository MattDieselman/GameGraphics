#pragma once

#include <DirectXMath.h>

struct SpotLight {
	DirectX::XMFLOAT4 ambientColor;
	DirectX::XMFLOAT4 diffuseColor;
	DirectX::XMFLOAT3 location;
	float bufferBytes;
	DirectX::XMFLOAT3 direction;
	float angle;
};