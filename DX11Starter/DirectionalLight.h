#pragma once

#include <DirectXMath.h>

struct DirectionalLight {
	DirectX::XMFLOAT4 ambientColor;
	DirectX::XMFLOAT4 diffuseColor;
	DirectX::XMFLOAT3 direction;
};