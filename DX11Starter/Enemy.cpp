#include "Enemy.h"

using namespace DirectX;

Enemy::Enemy()
{
}

Enemy::Enemy(Mesh * mesh, Material * material) :
	Entity(mesh, material)
{
	
}


Enemy::~Enemy()
{
}

void Enemy::init(ColliderType colliderType, float mass, float maxSpeed)
{
	//this->maxSpeed = maxSpeed;
	Entity::init(colliderType, mass);
}

DirectX::XMFLOAT3 Enemy::seek(const Entity& target, float maxSpeed)
{
	XMVECTOR desired = XMLoadFloat3(&target.getPosition()) - XMLoadFloat3(&transform.position);
	desired = XMVector3Normalize(desired) * maxSpeed;
	//desired -= XMLoadFloat3(&rigidbody.velocity);
	XMStoreFloat3(&desiredVelocity, desired);
	return desiredVelocity;
}

//void Enemy::setTarget(Entity* target)
//{
//	this->target = target;
//}

void Enemy::update(const Entity& target, float maxSpeed, float dt)
{
	rigidbody.velocity = seek(target, maxSpeed);
	rigidbody.velocity.x = fabs(rigidbody.velocity.x) * -1;
	if (rigidbody.velocity.y > 0.005)
	{
		rigidbody.velocity.y = 0.005;
		rigidbody.velocity.x = -maxSpeed;
	}
	//rigidbody.ApplyForce(dt);
	//XMVECTOR vel = XMLoadFloat3(&rigidbody.velocity);
	//XMVector3ClampLength(vel, -maxSpeed, maxSpeed);
	//XMStoreFloat3(&rigidbody.velocity, vel);

	Entity::update(dt);
}
