#include "Entity.h"

using namespace DirectX;

// Constructors / Destructors

Entity::Entity()
{
}

Entity::Entity(Mesh * mesh_,Material * material_)
{
	mesh = mesh_;
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&world, XMMatrixTranspose(W));
	position = XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);
	mat = material_;
}

Entity::~Entity()
{
	mesh->release();
	mat->release();
}

// Gets / Sets

XMFLOAT4X4 Entity::getWorld()
{
	XMVECTOR pos = XMLoadFloat3(&(position));//XMVectorAdd(XMLoadFloat3(&position),XMLoadFloat(&speed));//XMVectorAdd(XMLoadFloat3(&position) , XMVectorMultiply(XMLoadFloat3(&rot),XMLoadFloat( &speed)));
	XMMATRIX w = XMLoadFloat4x4(&world);
	XMMATRIX trans = XMMatrixTranspose(XMMatrixTranslationFromVector(pos));
	XMMATRIX rotMat = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));
	XMMATRIX scal = XMMatrixScalingFromVector(XMLoadFloat3(&scale));

	w = trans*
		rotMat *
		scal;
	XMStoreFloat3(&position, pos);
	XMStoreFloat4x4(&world, (w));
	return world;
}

XMFLOAT3 Entity::getRotation()
{
	return rotation;
}

XMFLOAT3 Entity::getScale() const
{
	return scale;
}

XMFLOAT3 Entity::getPosition() const
{
	return position;
}

void Entity::setPosition(XMFLOAT3 pos)
{
	position = pos;
}

Mesh * Entity::getMesh()
{
	return mesh;
}

Material * Entity::getMat()
{
	return mat;
}

void Entity::setScale(XMFLOAT3 scale_)
{
	scale = scale_;
}

// Methods

void Entity::Move(float speed, XMFLOAT3 rot)
{


	XMVECTOR pos = XMLoadFloat3(&(position));

	pos = pos + (XMLoadFloat3(&rot) * speed);
	XMMATRIX w = XMLoadFloat4x4(&world);

	XMMATRIX trans = XMMatrixTranspose( XMMatrixTranslationFromVector(pos));

	XMMATRIX rotMat = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));
	XMMATRIX scal = XMMatrixScalingFromVector(XMLoadFloat3(&scale));

	w = trans*
		rotMat *
		scal;


	XMStoreFloat3(&position, pos);
	XMStoreFloat4x4(&world, (w));
}

bool Entity::checkCollision(const Entity& other)
{
	XMFLOAT3 otherPos = other.getPosition();

	float resultantX = otherPos.x - position.x;

	float extentX = fabs(resultantX) - (scale.x + other.getScale().x) / 2;

	if (extentX <= 0)
	{
		float resultantY = otherPos.y - position.y;

		float extentY = fabs(resultantY) - (scale.y + other.getScale().y) / 2;

		if (extentY <= 0)
		{
 			return true;
		}
	}

	return false;
}

