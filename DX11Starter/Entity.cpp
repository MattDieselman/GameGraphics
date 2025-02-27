#include "Entity.h"

using namespace DirectX;

// Constructors / Destructors

Entity::Entity()
{
	collider.colliderType = ColliderType::NONE;
}

Entity::Entity(Mesh * mesh, Material * material)
{
	this->mesh = mesh;

	mat = material;

	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&world, XMMatrixTranspose(W));

	transform.position = XMFLOAT3(0, 0, 0);
	transform.rotation = XMFLOAT3(0, 0, 0);
	transform.scale = XMFLOAT3(1, 1, 1);

	rigidbody.velocity = XMFLOAT3(0, 0, 0);
}

Entity::~Entity()
{
	mesh->release();
	mat->release();
}

// Gets / Sets

void Entity::init(ColliderType colliderType, float mass)
{
	active = true;
	collider.colliderType = colliderType;
	rigidbody.mass = mass;
	rigidbody.invMass = mass != 0 ? 1 / mass : 0;

	calculateCollider();
}

void Entity::update(float dt)
{
	XMVECTOR pos = XMLoadFloat3(&transform.position);
	pos += XMLoadFloat3(&rigidbody.velocity) * dt;
	XMStoreFloat3(&transform.position, pos);
	rigidbody.velocity = XMFLOAT3(0, 0, 0);
}

XMFLOAT4X4 Entity::getWorld()
{
	//XMVECTOR pos = XMLoadFloat3(&(transform.position));//XMVectorAdd(XMLoadFloat3(&position),XMLoadFloat(&speed));//XMVectorAdd(XMLoadFloat3(&position) , XMVectorMultiply(XMLoadFloat3(&rot),XMLoadFloat( &speed)));
	//XMMATRIX w = XMLoadFloat4x4(&world);
	XMMATRIX trans = XMMatrixTranspose(XMMatrixTranslationFromVector(XMLoadFloat3(&transform.position)));
	XMMATRIX rotMat = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&transform.rotation));
	XMMATRIX scale = XMMatrixScalingFromVector(XMLoadFloat3(&transform.scale));

	XMMATRIX w = trans*
		rotMat *
		scale;

	//XMStoreFloat3(&transform.position, pos);
	XMStoreFloat4x4(&world, (w));

	return world;
}

XMFLOAT3 Entity::getPosition() const
{
	return transform.position;
}

XMFLOAT3 Entity::getRotation() const
{
	return transform.rotation;
}

XMFLOAT3 Entity::getScale() const
{
	return transform.scale;
}

DirectX::XMFLOAT3 Entity::getVelocity() const
{
	return rigidbody.velocity;
}

void Entity::setPosition(XMFLOAT3 pos)
{
	transform.position = pos;
}

void Entity::setScale(XMFLOAT3 scale)
{
	transform.scale = scale;

	calculateCollider();
}

void Entity::setRotation(DirectX::XMFLOAT3 rot)
{
	transform.rotation = rot;
}

Mesh * Entity::getMesh()
{
	return mesh;
}

Material * Entity::getMat()
{
	return mat;
}

Collider Entity::getCollider() const
{
	return collider;
}

void Entity::setCollider(XMFLOAT3 min, XMFLOAT3 max)
{
	XMVECTOR vecToMin = XMVector3Transform(XMLoadFloat3(&min), XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z));
	XMVECTOR vecToMax = XMVector3Transform(XMLoadFloat3(&max), XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z));

	XMStoreFloat3(&collider.min, vecToMin);
	XMStoreFloat3(&collider.max, vecToMax);
}

void Entity::calculateCollider()
{
	if (collider.colliderType != ColliderType::NONE)
	{
		std::vector<Vertex> vertices = mesh->getVertices();
		DirectX::XMFLOAT3 min = DirectX::XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
		DirectX::XMFLOAT3 max = DirectX::XMFLOAT3(FLT_MIN, FLT_MIN, FLT_MIN);
		for (int i = 0; i < mesh->getVertexCount(); i++)
		{
			// Find mesh min xyz
			min.x = fmin(min.x, vertices[i].position.x);
			min.y = fmin(min.y, vertices[i].position.y);
			min.z = fmin(min.z, vertices[i].position.z);

			// Find mesh max xyz
			max.x = fmax(max.x, vertices[i].position.x);
			max.y = fmax(max.y, vertices[i].position.y);
			max.z = fmax(max.z, vertices[i].position.z);
		}

		setCollider(min, max);
	}
}

// Methods

void Entity::Move(float speed, XMFLOAT3 rot)
{
	XMVECTOR vel = XMLoadFloat3(&rigidbody.velocity);
	vel += XMLoadFloat3(&rot) * speed;

	XMStoreFloat3(&rigidbody.velocity, vel);
}

bool Entity::checkCollision(const Entity& other)
{
	XMFLOAT3 otherPos = other.getPosition();

	float resultantX = otherPos.x - transform.position.x;

	float aExtentX = collider.max.x;
	float bExtentX = other.getCollider().max.x;

	if (fabs(resultantX) <= aExtentX + bExtentX)
	{
		float resultantY = otherPos.y - transform.position.y;

		float aExtentY = collider.max.y;
		float bExtentY = other.getCollider().max.y;

		if (fabs(resultantY )<= aExtentY + bExtentY)
		{
 			return true;
		}
	}

	return false;
}