#include "Ragdoll.h"

Ragdoll::Ragdoll(physx::PxPhysics* physics, physx::PxScene* scene, physx::PxMaterial* material)
{
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
	physx::PxSphericalJoint* joint = physx::PxSphericalJointCreate(*mPhysics,
		body, physx::PxTransform(physx::PxVec3(localBody.x, localBody.y, localBody.z)),
		limb, physx::PxTransform(physx::PxVec3(localLimb.x, localLimb.y, localLimb.z)));
	joint->setLimitCone(physx::PxJointLimitCone(physx::PxPi / 2, physx::PxPi / 2, 0.01f));
	joint->setSphericalJointFlag(physx::PxSphericalJointFlag::eLIMIT_ENABLED, true);
}