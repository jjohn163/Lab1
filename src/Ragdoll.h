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
	Ragdoll(physx::PxPhysics* &physics, physx::PxScene* &scene, physx::PxMaterial* &material);
	physx::PxPhysics* mPhysics;
	physx::PxScene* mScene;
	physx::PxMaterial* mMaterial;
	physx::PxRigidDynamic* body;
	physx::PxRigidDynamic* createDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, physx::PxReal mass);
	vector<physx::PxRigidDynamic*> limbs;
	void addBody(physx::PxRigidDynamic* physicsBody);
	void addLimb(physx::PxRigidDynamic* limb, vec3 localLimb, vec3 localBody);
	void setPosition(vec3 pos);
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
	static shared_ptr<Entity> createBirdRagdoll(vec3 pos, vector<shared_ptr<Entity>>& entities, shared_ptr<Ragdoll> ragdoll, const std::string& resourceDirectory) {

		//create physx bodies
		physx::PxRigidDynamic* body = ragdoll->createDynamic(physx::PxTransform(physx::PxVec3(pos.x, pos.y, pos.z)), physx::PxSphereGeometry(3.5), 10);
		physx::PxRigidDynamic* head = ragdoll->createDynamic(physx::PxTransform(physx::PxVec3(pos.x, pos.y, pos.z + 3)), physx::PxSphereGeometry(4), 10);
		physx::PxRigidDynamic* armLeft = ragdoll->createDynamic(physx::PxTransform(physx::PxVec3(pos.x + 3, pos.y, pos.z)), physx::PxBoxGeometry(1.5, 1, .5), 10);
		physx::PxRigidDynamic* legLeft = ragdoll->createDynamic(physx::PxTransform(physx::PxVec3(pos.x + 1, pos.y, pos.z - 3)), physx::PxBoxGeometry(0.5, 0.5, 1), 10);
		physx::PxRigidDynamic* armRight = ragdoll->createDynamic(physx::PxTransform(physx::PxVec3(pos.x - 3, pos.y, pos.z)), physx::PxBoxGeometry(1.5, 1, .5), 10);
		physx::PxRigidDynamic* legRight = ragdoll->createDynamic(physx::PxTransform(physx::PxVec3(pos.x - 1, pos.y, pos.z + 3)), physx::PxBoxGeometry(0.5, 0.5, 1), 10);
		
		//add bodies to ragdoll
		ragdoll->addBody(body);
		ragdoll->addLimb(head, vec3(0, 0, -4), vec3(0, 0, 3));
		ragdoll->addLimb(armLeft, vec3(-1.5, 0, 0), vec3(3, 0, 0));
		ragdoll->addLimb(legLeft, vec3(0, 0, 1), vec3(1, 0, -3));
		ragdoll->addLimb(armRight, vec3(1.5, 0, 0), vec3(-3, 0, 0));
		ragdoll->addLimb(legRight, vec3(0, 0, 1), vec3(-1, 0, -3));

		//add bodies to entity components
		shared_ptr<Entity> bird = make_shared<Entity>(
			resourceDirectory + "/sphere.obj",
			pos,
			vec3(3, 3, 3),
			vec3(1, 0, 0),
			true,
			ProgramManager::GREEN_PLASTIC,
			0,
			ProgramManager::YELLOW,
			body);
		entities.push_back(bird);

		shared_ptr<Entity> birdHead = make_shared<Entity>(
			resourceDirectory + "/sphere.obj",
			pos,
			vec3(4, 4, 4),
			vec3(1, 0, 0),
			true,
			ProgramManager::GREEN_PLASTIC,
			0,
			ProgramManager::YELLOW,
			head);
		entities.push_back(birdHead);

		shared_ptr<Entity> wingLeft = make_shared<Entity>(
			resourceDirectory + "/cube.obj",
			vec3(pos.x + 5, pos.y, pos.z),
			vec3(3, 2, 1),
			vec3(1, 0, 0),
			true,
			ProgramManager::GREEN_PLASTIC,
			0,
			ProgramManager::YELLOW,
			armLeft);
		entities.push_back(wingLeft);

		shared_ptr<Entity> wingRight = make_shared<Entity>(
			resourceDirectory + "/cube.obj",
			vec3(pos.x - 5, pos.y, pos.z),
			vec3(3, 2, 1),
			vec3(1, 0, 0),
			true,
			ProgramManager::GREEN_PLASTIC,
			0,
			ProgramManager::YELLOW,
			armRight);
		entities.push_back(wingRight);

		shared_ptr<Entity> footLeft = make_shared<Entity>(
			resourceDirectory + "/cube.obj",
			vec3(pos.x + 5, pos.y, pos.z),
			vec3(1, 1, 2),
			vec3(1, 0, 0),
			true,
			ProgramManager::GREEN_PLASTIC,
			0,
			ProgramManager::ORANGE,
			legLeft);
		entities.push_back(footLeft);

		shared_ptr<Entity> footRight = make_shared<Entity>(
			resourceDirectory + "/cube.obj",
			vec3(pos.x + 5, pos.y, pos.z),
			vec3(1, 1, 2),
			vec3(1, 0, 0),
			true,
			ProgramManager::GREEN_PLASTIC,
			0,
			ProgramManager::ORANGE,
			legRight);
		entities.push_back(footRight);

		return bird;
	}
};

