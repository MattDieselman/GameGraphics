#include "PhysicsManager.h"

using namespace DirectX;

// Jump Table of all possible collision checks
CollisionCallback CollisionCheck[ColliderType::COUNT][ColliderType::COUNT] =
{		// None			// AABB			// OOBB			// Sphere
	{	&NoCollision,	&NoCollision,	&NoCollision,	&NoCollision	},	// None
	{	&NoCollision,	&AABBvsAABB,	&AABBvsOOBB,	&AABBvsSphere	},	// AABB
	{	&NoCollision,	&OOBBvsAABB,	&OOBBvsOOBB,	&OOBBvsSphere	},	// OOBB
	{	&NoCollision,	&SpherevsAABB,	&SpherevsOOBB,	&SpherevsSphere	}	// Sphere
};

namespace
{
	bool NoCollision(const Entity& a, const Entity& b)
	{
		return false;
	}
	bool AABBvsAABB(const Entity& a, const Entity& b)
	{
		/*Entity a = m.first;
		Entity b = m.second;*/

		float resultantX = a.getPosition().x - b.getPosition().x;

		float aExtentX = a.getCollider().max.x;
		float bExtentX = b.getCollider().max.x;

		if (fabs(resultantX) <= aExtentX + bExtentX)
		{
			float resultantY = a.getPosition().y - b.getPosition().y;

			float aExtentY = a.getCollider().max.y;
			float bExtentY = b.getCollider().max.y;

			if (fabs(resultantY) <= aExtentY + bExtentY)
			{
				return true;
			}
		}

		return false;
	}
	bool AABBvsOOBB(const Entity& a, const Entity& b)
	{
		return false;
	}
	bool AABBvsSphere(const Entity& a, const Entity& b)
	{
		XMFLOAT3 aPos = a.getPosition();
		XMFLOAT3 bPos = b.getPosition();
		XMVECTOR resultantVector = XMLoadFloat3(&aPos) - XMLoadFloat3(&bPos);

		XMVECTOR closest = resultantVector;
		//XMStoreFloat3(&closest, resultantVector);

		XMFLOAT3 aMin = a.getCollider().min;
		XMFLOAT3 aMax = a.getCollider().max;

		closest = XMVectorClamp(closest, XMLoadFloat3(&aMin), XMLoadFloat3(&aMax));

		XMFLOAT3 distanceSq;
		XMStoreFloat3(&distanceSq, XMVector3LengthSq(resultantVector - closest));

		float radius = b.getCollider().max.x;
		
		if (distanceSq.x < radius * radius)
		{
			return true;
		}

		return false;
	}
	bool OOBBvsOOBB(const Entity& a, const Entity& b)
	{
		return false;
	}
	bool OOBBvsAABB(const Entity& a, const Entity& b)
	{
		return AABBvsOOBB(b, a);
	}
	bool OOBBvsSphere(const Entity& a, const Entity& b)
	{
		return false;
	}
	bool SpherevsSphere(const Entity& a, const Entity& b)
	{
		/*Entity a = m.first;
		Entity b = m.second;*/

		XMFLOAT3 aPos = a.getPosition();
		XMFLOAT3 bPos = b.getPosition();
		XMVECTOR resultantVector = XMLoadFloat3(&aPos) - XMLoadFloat3(&bPos);

		XMFLOAT3 distanceSq;
		XMStoreFloat3(&distanceSq, XMVector3LengthSq(resultantVector));
		
		float radius = a.getCollider().max.x + b.getCollider().max.x;
		/*XMVECTOR aRadius = XMVector3Length(XMLoadFloat3(&aMax));
		XMVECTOR bRadius = XMVector3Length(XMLoadFloat3(&bMax));*/

		//resultantVector = XMVector3LengthSq(resultantVector);
		if (distanceSq.x < radius * radius)
		{
      			return true;
		}

		return false;
	}
	bool SpherevsAABB(const Entity& a, const Entity& b)
	{
		return AABBvsSphere(b, a);
	}
	bool SpherevsOOBB(const Entity& a, const Entity& b)
	{
		return OOBBvsSphere(b, a);
	}
}

PhysicsManager::PhysicsManager()
{
}

PhysicsManager::~PhysicsManager()
{
}

void PhysicsManager::addPair(const Entity& a, const Entity& b)
{
	collisionPairs.push_back(std::make_pair(a, b));
}

void PhysicsManager::update(std::vector<Entity*> gameObjects, float dt)
{
	for (Entity* objA : gameObjects)
	{
		for (Entity* objB : gameObjects)
		{
			if (objA == objB) continue; // Early out if objA and objB are the same
			if (CollisionCheck[objA->getCollider().colliderType][objB->getCollider().colliderType](*objA, *objB))
			{

			}
		}
	}
}
