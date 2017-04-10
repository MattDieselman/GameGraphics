#pragma once

#include <DirectXMath.h>

enum ColliderType
{
	NONE,
	AABB,
	OOBB,
	SPHERE,
	COUNT
};

struct Collider
{
	ColliderType colliderType;

	DirectX::XMFLOAT3 min;
	DirectX::XMFLOAT3 max;
};