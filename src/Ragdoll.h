#pragma once

#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include "physx/PxPhysicsAPI.h"
#include "Enitity.h"

using namespace std;
using namespace glm;

class Ragdoll
{
public:
	Ragdoll(physx::PxPhysics* physics, physx::PxScene* scene, physx::PxMaterial* material);
	physx::PxPhysics* mPhysics;
	physx::PxScene* mScene;
	physx::PxMaterial* mMaterial;
	physx::PxRigidDynamic* body;
	physx::PxRigidDynamic* createDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, physx::PxReal mass);
	void addBody(physx::PxRigidDynamic* physicsBody);
	void addLimb(physx::PxRigidDynamic* limb, vec3 localLimb, vec3 localBody);
	//static physx::PxPhysics* mPhysics;
	//static void updateRotation(shared_ptr<Entity> entity) {}
	//static void updatePosition(shared_ptr<Entity> entity) {}
};

