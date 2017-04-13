#pragma once
#include "Entity.h"
class Enemy :
	public Entity
{
public:
	Enemy();
	Enemy(Mesh* mesh, Material * material);
	~Enemy();

	void init(ColliderType colliderType, float mass, float maxSpeed);
	void update(const Entity& target, float maxSpeed, float dt);

	DirectX::XMFLOAT3 seek(const Entity& target, float maxSpeed);
	//DirectX::XMFLOAT3 seek();

	//void setTarget(Entity* target);

private:
	//Entity* target;
	DirectX::XMFLOAT3 desiredVelocity;
	//float maxSpeed;
};

