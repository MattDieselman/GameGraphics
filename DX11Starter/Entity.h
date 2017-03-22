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
	XMFLOAT3 rotation;
	XMFLOAT3 scale;
	XMFLOAT3 position;
	Mesh* mesh;
	Material * mat;

public:

	Entity();
	Entity(Mesh* mesh_,Material * material_);
	~Entity();

	XMFLOAT4X4 getWorld();
	XMFLOAT3 getRotation();
	XMFLOAT3 getScale();
	void setScale(XMFLOAT3 scale_);
	XMFLOAT3 getPosition();
	void setPosition(XMFLOAT3 pos);
	Mesh* getMesh();
	Material * getMat();
	
	void Move(float speed, XMFLOAT3 rot);
	
};

