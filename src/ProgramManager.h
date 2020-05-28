#pragma once
#include <stdlib.h>
#include <memory>
#include <vector>
#include "./MatrixStack.h"
#include "Shape.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Program.h"
#include "Texture.h"

using namespace std;

class ProgramManager
{
public:
	
	static const std::string resourceDirectory;
	static ProgramManager* Instance();
	static enum Material { BLUE_PLASTIC, FLAT_GREY, BRASS, GREEN_PLASTIC, LIGHT_BLUE, PURPLE, RED, DIRT };

	static enum Mesh { CUBE_MESH, SPHERE_MESH, ROCK_MESH, WALL_MESH , EAGLE_MESH, BRANCH_MESH };
	static enum CustomTextures { CHICK, ROCK, WALL, YELLOW, ORANGE, EAGLE, BRANCH, DEFAULT };
	void init();
	void setMaterial(Material i);
	void setTexture(CustomTextures i);
	void setModel(std::shared_ptr<MatrixStack> Model, shared_ptr<Program> shader);
	void drawMesh(Mesh i, shared_ptr<Program> shader);

	std::shared_ptr<Program> progMat;
	std::shared_ptr<Program> prog;
	

private: 
	ProgramManager() {};
	~ProgramManager();
	static ProgramManager* sInstance;
	Texture * tex_sample;
	Texture * tex_chick;
	Texture * tex_rock;
	Texture * tex_wall;
	Texture* tex_yellow;
	Texture* tex_orange;
	Texture* tex_eagle;
	Texture* tex_branch;
	
	Shape * mesh_cube;
	Shape * mesh_sphere;
	Shape * mesh_rock;
	Shape * mesh_wall;
	Shape * mesh_eagle;
	Shape * mesh_branch;

	//std::shared_ptr<Program> progMat;

};