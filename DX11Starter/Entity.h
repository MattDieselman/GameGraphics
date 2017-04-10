#pragma once

#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "Collider.h"

class Entity
{
private:

	DirectX::XMFLOAT4X4 world;
	Transform transform;
	Mesh* mesh;
	Material * mat;
	Collider collider;

public:

	Entity();
	Entity(Mesh* mesh,Material * material, ColliderType colliderType);
	~Entity();

	DirectX::XMFLOAT4X4 getWorld();
	DirectX::XMFLOAT3 getPosition() const;
	DirectX::XMFLOAT3 getRotation();
	DirectX::XMFLOAT3 getScale() const;

	void setScale(DirectX::XMFLOAT3 scale);
	void setPosition(DirectX::XMFLOAT3 pos);

	Mesh* getMesh();
	Material * getMat();

	Collider getCollider() const;
	void setCollider(DirectX::XMFLOAT3 min, DirectX::XMFLOAT3 max);
	void calculateCollider();

	void Move(float speed, DirectX::XMFLOAT3 rot);
	bool checkCollision(const Entity& other);
};

