#pragma once

#include <DirectXMath.h>

struct Rigidbody
{
	float mass, invMass;
	DirectX::XMFLOAT3 velocity, force;
};