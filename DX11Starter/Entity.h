#pragma once
#include "DXCore.h"
#include <d3d11.h>
#include "Vertex.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
using namespace DirectX;
class Entity
{
private:
	XMFLOAT4X4 world;

public:
	//Fields
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
	XMFLOAT3 position;
	Mesh* mesh;
	Material * mat;
	Entity();
	Entity(Mesh* mesh_,Material * material_);
	~Entity();

	XMFLOAT4X4 getWorld();
	XMFLOAT3 getPosition();
	void Entity::setPosition(XMFLOAT3 pos);
	void setScale(XMFLOAT3 scale_);

	void Move(float speed, XMFLOAT3 rot);
	
};

