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
	static void updateOrientation(shared_ptr<Entity> entity) {
		//update position information
		physx::PxVec3 pos = entity->body->getGlobalPose().p;
		entity->position = vec3(pos.x, pos.y, pos.z);

		//update rotation information
		physx::PxQuat rot = entity->body->getGlobalPose().q;
		physx::PxVec3 rotN = physx::PxVec3(0);
		rot.toRadiansAndUnitAxis(entity->rotationDegrees, rotN);
		entity->rotation = vec3(rotN.x, rotN.y, rotN.z);


	}
};

