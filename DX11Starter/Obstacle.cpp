#include "Obstacle.h"

bool Obstacle::checkCollision(Entity *a)
{
	if (a->getPosition().x > getPosition().x - getScale().x && a->getPosition().x < getPosition().x + getScale().x) {
		if (a->getPosition().y > getPosition().y - getScale().y&&a->getPosition().y < getPosition().y + getScale().y) {
			if (a->getPosition().z > getPosition().z - getScale().z&&a->getPosition().z < getPosition().z + getScale().z) {
				return true;
			}
		}
	}
	return false;
}

Obstacle::Obstacle():
	Entity(){
}

Obstacle::Obstacle(Mesh * mesh, Material * material, ColliderType colliderType):
	Entity(mesh, material, colliderType)
{
}

Obstacle::~Obstacle()
{
}
