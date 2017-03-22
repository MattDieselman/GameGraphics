#include "Entity.h"

using namespace DirectX;

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
}

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

XMFLOAT3 Entity::getPosition()
{
	return position;
}

void Entity::setPosition(XMFLOAT3 pos)
{
	position = pos;
}

void Entity::setScale(XMFLOAT3 scale_)
{
	scale = scale_;
}

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

