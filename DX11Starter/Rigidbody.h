#pragma once

#include <DirectXMath.h>

using namespace DirectX;

struct Rigidbody
{
	float mass, invMass;
	DirectX::XMFLOAT3 velocity, force;

	void ApplyForce(float dt)
	{
		DirectX::XMVECTOR acceleration = XMLoadFloat3(&force) * invMass;
		XMStoreFloat3(&velocity, acceleration * dt);
	}
};