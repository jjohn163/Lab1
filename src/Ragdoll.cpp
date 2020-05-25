#include "Ragdoll.h"

Ragdoll::Ragdoll(physx::PxPhysics* &physics, physx::PxScene* &scene, physx::PxMaterial* &material)
{
	//initialize physx
	static physx::PxDefaultErrorCallback gDefaultErrorCallback;
	static physx::PxDefaultAllocator gDefaultAllocatorCallback;

	physx::PxFoundation* mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!mFoundation)
		throw "PxCreateFoundation failed!";

	bool recordMemoryAllocations = true;

	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, physx::PxTolerancesScale(), recordMemoryAllocations);
	if (!physics)
		throw "PxCreatePhysics failed!";

	physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

	physx::PxDefaultCpuDispatcher* mCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	if (!mCpuDispatcher)
		throw "PxDefaultCpuDispatcherCreate failed!";
	sceneDesc.cpuDispatcher = mCpuDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	scene = physics->createScene(sceneDesc);
	if (!scene)
		throw "Scene creation failed!";
	material = physics->createMaterial(5.0f, 5.0f, 0.1f);    //static friction, dynamic friction, restitution
	if (!material)
		throw "createMaterial failed!";
	mPhysics = physics;
	mScene = scene;
	mMaterial = material;
}

physx::PxRigidDynamic* Ragdoll::createDynamic(const physx::PxTransform& t, const physx::PxGeometry& geometry, physx::PxReal mass)
{
	physx::PxRigidDynamic* dynamic = PxCreateDynamic(*mPhysics, t, geometry, *mMaterial, 5.0f);
	mScene->addActor(*dynamic);
	dynamic->setMass(mass);
	return dynamic;
}

void Ragdoll::addBody(physx::PxRigidDynamic* physicsBody)
{
	body = physicsBody;
}

void Ragdoll::addLimb(physx::PxRigidDynamic* limb, vec3 localLimb, vec3 localBody)
{
	limbs.push_back(limb);
	physx::PxSphericalJoint* joint = physx::PxSphericalJointCreate(*mPhysics,
		body, physx::PxTransform(physx::PxVec3(localBody.x, localBody.y, localBody.z)),
		limb, physx::PxTransform(physx::PxVec3(localLimb.x, localLimb.y, localLimb.z)));
	joint->setLimitCone(physx::PxJointLimitCone(physx::PxPi / 6, physx::PxPi / 8, 0.5));
	joint->setSphericalJointFlag(physx::PxSphericalJointFlag::eLIMIT_ENABLED, false);
}

void Ragdoll::setPosition(vec3 pos)
{
	body->setGlobalPose(physx::PxTransform(physx::PxVec3(pos.x, pos.y, pos.z)));
	for (physx::PxRigidDynamic* limb : limbs) {
		limb->setGlobalPose(physx::PxTransform(physx::PxVec3(pos.x, pos.y, pos.z)));
	}
}


