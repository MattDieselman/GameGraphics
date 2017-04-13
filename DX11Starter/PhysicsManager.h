#pragma once

#include <vector>

#include "Entity.h"

typedef std::pair<const Entity&, const Entity&> manifold;
typedef bool (*CollisionCallback)(const Entity& a, const Entity& b);
extern CollisionCallback CollisionCheck[ColliderType::COUNT][ColliderType::COUNT];

//namespace
//{
	// Collision check functions
	bool NoCollision(const Entity& a, const Entity& b);
	bool AABBvsAABB(const Entity& a, const Entity& b);
	bool AABBvsOOBB(const Entity& a, const Entity& b);
	bool AABBvsSphere(const Entity& a, const Entity& b);
	bool OOBBvsOOBB(const Entity& a, const Entity& b);
	bool OOBBvsAABB(const Entity& a, const Entity& b);
	bool OOBBvsSphere(const Entity& a, const Entity& b);
	bool SpherevsSphere(const Entity& a, const Entity& b);
	bool SpherevsAABB(const Entity& a, const Entity& b);
	bool SpherevsOOBB(const Entity& a, const Entity& b);
//}

class PhysicsManager
{
public:
	PhysicsManager();
	~PhysicsManager();

	std::vector<manifold> collisionPairs;

	//void init();

	void addPair(const Entity& a, const Entity& b);

	void update(std::vector<Entity*> gameObjects, float dt);
};

