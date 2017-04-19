#pragma once

#include <DirectXMath.h>

struct PointLight {
	DirectX::XMFLOAT4 ambientColor;
	DirectX::XMFLOAT4 diffuseColor;
	DirectX::XMFLOAT3 location;
};