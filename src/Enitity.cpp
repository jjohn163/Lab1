#include "Enitity.h"
#include "MatrixStack.h"
#include "Shape.h"
#include <stdlib.h>
#include <iostream>
#include "ProgramManager.h"
#include "Program.h"

Entity::Entity(
	string objDir, 
	glm::vec3 pos, 
	glm::vec3 scl, 
	glm::vec3 rot, 
	bool mov, 
	ProgramManager::Material mat, 
	float rotDeg,
	ProgramManager::CustomTextures texture,
	physx::PxRigidDynamic* collider)
{
	objDirectory = objDir;
	position = pos;
	scale = scl;
	rotation = rot;
	velocity = vec3(0.0, 0.0, 0.0);
	moving = mov;
	rotationDegrees = rotDeg;
	material = mat;
	this->texture = texture;
	body = collider;
	initMesh();
}

void Entity::initMesh() {
	string errStr;
	vector<tinyobj::shape_t> TOshapesObject;
	vector<tinyobj::material_t> objMaterialsObject;
	bool rc = tinyobj::LoadObj(TOshapesObject, objMaterialsObject, errStr, this->objDirectory.c_str());
	if (!rc) {
		cerr << errStr << endl;
	}
	else {
		this->mesh = make_shared<Shape>();
		this->mesh->createShape(TOshapesObject[0]);
		this->mesh->measure();
		this->mesh->init();
	}
}

void Entity::draw(shared_ptr<MatrixStack> Model) {
	ProgramManager::setMaterial(this->material);
	ProgramManager::setTexture(this->texture);
	Model->pushMatrix();
	Model->translate(this->position);
	Model->rotate(this->rotationDegrees, this->rotation);
	Model->scale(this->scale);
	ProgramManager::setModel(Model);
	ProgramManager::setMaterial(this->material);
	this->mesh->draw(ProgramManager::progMat);
	Model->popMatrix();
}

void Entity::updatePosition(float deltaTime) {
	vec3 change = deltaTime * velocity;
	position += change;
	for (shared_ptr<Collider> collider : colliders) {
		collider->updatePosition(change);
	}
}


Entity::~Entity()
{
}

