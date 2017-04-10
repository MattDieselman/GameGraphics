#pragma once
#include "Entity.h"
class Obstacle :
	public Entity
{
public:
	bool checkCollision(Entity *a);
	Obstacle();
	Obstacle(Mesh* mesh, Material * material, ColliderType colliderType);

	~Obstacle();
};

