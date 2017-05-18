#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "Collider.h"
#include "Rigidbody.h"

class Entity
{
protected:

	DirectX::XMFLOAT4X4 world;
	Transform transform;
	Mesh* mesh;
	Material * mat;
	Collider collider;
	Rigidbody rigidbody;

public:

	Entity();
	Entity(Mesh* mesh,Material * material);
	~Entity();

	bool active;
	bool xMovement;
	bool yMovement;

	virtual void init(ColliderType colliderType, float mass);
	virtual void update(float dt);

	DirectX::XMFLOAT4X4 getWorld();
	DirectX::XMFLOAT3 getPosition() const;
	DirectX::XMFLOAT3 getRotation() const;
	DirectX::XMFLOAT3 getScale() const;
	DirectX::XMFLOAT3 getVelocity() const;

	void setPosition(DirectX::XMFLOAT3 pos);
	void setScale(DirectX::XMFLOAT3 scale);
	void setRotation(DirectX::XMFLOAT3 rot);

	Mesh* getMesh();
	Material * getMat();

	Collider getCollider() const;
	void setCollider(DirectX::XMFLOAT3 min, DirectX::XMFLOAT3 max);
	void calculateCollider();

	void Move(float speed, DirectX::XMFLOAT3 rot);
	bool checkCollision(const Entity& other);
};

