#pragma once
#include "Entity.h"
class Obstacle :
	public Entity
{
public:
	bool checkCollision(Entity *a);
	Obstacle();
	Obstacle(Mesh* mesh_, Material * material_);

	~Obstacle();
};

