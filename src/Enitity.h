#pragma once
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include "Collider.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "Program.h"
#include "ProgramManager.h"
#include "physx/PxPhysicsAPI.h"


using namespace std;
using namespace glm;

class Entity
{
public:
	Entity(
		//string objDir, 
		ProgramManager::Mesh mesh,
		glm::vec3 pos,
		glm::vec3 scale, 
		glm::vec3 rot, 
		bool mov, 
		ProgramManager::Material mat, 
		float rotDeg = 0,
		ProgramManager::CustomTextures texture = ProgramManager::DEFAULT,
		physx::PxRigidDynamic* collider = NULL,
		float cullRad = 50);
	~Entity();
	void draw(shared_ptr<MatrixStack> Model, shared_ptr<Program> shader=NULL);
	void updatePosition(float deltaTime);
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::vec3 velocity;
	float rotationDegrees;
	ProgramManager::Material material;
	vector<shared_ptr<Collider>> colliders;
	bool moving;
	ProgramManager::Mesh mesh;
	string objDirectory;
	physx::PxRigidDynamic* body;
	float cullRadius;

	ProgramManager::CustomTextures texture;
};

