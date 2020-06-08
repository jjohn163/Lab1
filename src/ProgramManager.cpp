#include "ProgramManager.h"
#include <stdlib.h>
#include <memory>
#include "Shape.h"
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Program.h"


using namespace std;
using namespace glm;

ProgramManager* ProgramManager::sInstance = NULL;

//Texture * ProgramManager::tex_sample;
//Texture * ProgramManager::tex_chick;
//Texture * ProgramManager::tex_rock;
//Texture * ProgramManager::tex_wall;
//Texture * ProgramManager::tex_yellow;
//Texture * ProgramManager::tex_orange;

//Shape* ProgramManager::mesh_cube;
//Shape* ProgramManager::mesh_sphere;
//Shape* ProgramManager::mesh_rock;
//Shape* ProgramManager::mesh_wall;




ProgramManager::~ProgramManager()
{
}

const string ProgramManager::resourceDirectory = "../resources";

// Our shader program
//const std::shared_ptr<Program> ProgramManager::progMat = make_shared<Program>();
 
void initMesh(const string file, Shape* &mesh) {
	cout << file << endl;
	string objDirectory = ProgramManager::resourceDirectory + file;
	string errStr;
	vector<tinyobj::shape_t> TOshapesObject;
	vector<tinyobj::material_t> objMaterialsObject;
	//Shape* mesh;
	bool rc = tinyobj::LoadObj(TOshapesObject, objMaterialsObject, errStr, objDirectory.c_str());
	if (!rc) {
		cerr << errStr << endl;
	}
	else {
		//shared_ptr<Shape> temp = make_shared<Shape>();
		mesh = new Shape;
		mesh->createShape(TOshapesObject[0]);
		mesh->measure();
		mesh->init();
	}
}

ProgramManager* ProgramManager::Instance()
{
	if (!sInstance) {
		sInstance = new ProgramManager;
		sInstance->init();
	}
	return sInstance;
}

void ProgramManager::init() {
	progMat = make_shared<Program>();
	progMat->setVerbose(true);
	progMat->setShaderNames(resourceDirectory + "/simple_vert.glsl", resourceDirectory + "/gbuf_frag.glsl");
	progMat->init();
	progMat->addUniform("P");
	progMat->addUniform("V");
	progMat->addUniform("M");
	progMat->addUniform("LightPos");
	progMat->addUniform("Texture0");
	progMat->addUniform("shadowDepth");
	progMat->addUniform("LS");
	progMat->addUniform("Damage");
	progMat->addAttribute("vertPos");
	progMat->addAttribute("vertNor");
	progMat->addAttribute("vertTex");
	cout << "progMat " << progMat->getPID() << endl;
	tex_sample = new Texture();
	tex_sample->setFilename(resourceDirectory + "/crate.jpg");
	tex_sample->init();
	tex_sample->setUnit(0);
	tex_sample->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	tex_chick = new Texture();
	tex_chick->setFilename(resourceDirectory + "/chick.jpg");
	tex_chick->init();
	tex_chick->setUnit(0);
	tex_chick->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	tex_rock = new Texture();
	tex_rock->setFilename(resourceDirectory + "/squareRock.jpg");
	tex_rock->init();
	tex_rock->setUnit(0);
	tex_rock->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	tex_wall = new Texture();
	tex_wall->setFilename(resourceDirectory + "/sb_iceflow/iceflow_dn.jpg");
	tex_wall->init();
	tex_wall->setUnit(0);
	tex_wall->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	tex_yellow = new Texture();
	tex_yellow->setFilename(resourceDirectory + "/feathers_texture.jpg");
	tex_yellow->init();
	tex_yellow->setUnit(0);
	tex_yellow->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	tex_orange = new Texture();
	tex_orange->setFilename(resourceDirectory + "/orange.jpg");
	tex_orange->init();
	tex_orange->setUnit(0);
	tex_orange->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	tex_eagle = new Texture();
	tex_eagle->setFilename(resourceDirectory + "/TalonedEagle.bmp");
	tex_eagle->init();
	tex_eagle->setUnit(0);
	tex_eagle->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	tex_branch = new Texture();
	tex_branch->setFilename(resourceDirectory + "/branch_texture.jpg");
	tex_branch->init();
	tex_branch->setUnit(0);
	tex_branch->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	initMesh("/cube.obj", mesh_cube);
	initMesh("/spheresmooth.obj", mesh_sphere);
	initMesh("/squareRock.obj", mesh_rock);
	initMesh("/rockyCliff_uv_smooth.obj", mesh_wall);
	initMesh("/TalonedEagle.obj", mesh_eagle);
	initMesh("/tree.obj", mesh_branch);
}

void ProgramManager::setModel(std::shared_ptr<MatrixStack>M, shared_ptr<Program> shader) {
	glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
}

void ProgramManager::setMaterial(Material i) {
	switch (i) {
	case BLUE_PLASTIC:
		glUniform3f(progMat->getUniform("MatAmb"), 0.02, 0.04, 0.2);
		glUniform3f(progMat->getUniform("MatDif"), 0.0, 0.16, 0.9);
		glUniform3f(progMat->getUniform("MatSpec"), 0.14, 0.2, 0.8);
		glUniform1f(progMat->getUniform("shine"), 120.0);
		break;
	case FLAT_GREY:
		glUniform3f(progMat->getUniform("MatAmb"), 0.13, 0.13, 0.14);
		glUniform3f(progMat->getUniform("MatDif"), 0.3, 0.3, 0.4);
		glUniform3f(progMat->getUniform("MatSpec"), 0.3, 0.3, 0.4);
		glUniform1f(progMat->getUniform("shine"), 4.0);
		break;
	case BRASS:
		glUniform3f(progMat->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
		glUniform3f(progMat->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
		glUniform3f(progMat->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
		glUniform1f(progMat->getUniform("shine"), 27.9);
		break;
	case GREEN_PLASTIC:
		glUniform3f(progMat->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
		glUniform3f(progMat->getUniform("MatDif"), 0.2, 0.8, 0.2);
		glUniform3f(progMat->getUniform("MatSpec"), 0.3, 0.3, 0.4);
		glUniform1f(progMat->getUniform("shine"), 80.0);
		break;
	case LIGHT_BLUE:
		glUniform3f(progMat->getUniform("MatAmb"), 0.8, 0.8, 0.99);
		glUniform3f(progMat->getUniform("MatDif"), 0.0, 0.1, 0.9);
		glUniform3f(progMat->getUniform("MatSpec"), 0.3, 0.3, 0.4);
		glUniform1f(progMat->getUniform("shine"), 20.);
		break;
	case PURPLE:
		glUniform3f(progMat->getUniform("MatAmb"), 0.2, 0, 0.2);
		glUniform3f(progMat->getUniform("MatDif"), 0.5, 0.0, 0.5);
		glUniform3f(progMat->getUniform("MatSpec"), 0.3, 0.3, 0.4);
		glUniform1f(progMat->getUniform("shine"), 20.);
		break;
	case RED:
		glUniform3f(progMat->getUniform("MatAmb"), .2, 0, 0);
		glUniform3f(progMat->getUniform("MatDif"), 1, 0, 0);
		glUniform3f(progMat->getUniform("MatSpec"), 0.3, 0.3, 0.4);
		glUniform1f(progMat->getUniform("shine"), 20.);
		break;
	case DIRT:
		glUniform3f(progMat->getUniform("MatAmb"), .5, .16, .0);
		glUniform3f(progMat->getUniform("MatDif"), .8, .2, 0);
		glUniform3f(progMat->getUniform("MatSpec"), 0.3, 0.3, 0.4);
		glUniform1f(progMat->getUniform("shine"), 20.);
		break;
	}
}

void ProgramManager::setTexture(CustomTextures i) {
	switch (i) {
	case CHICK:
		tex_chick->bind(progMat->getUniform("Texture0"));
		break;
	case ROCK:
		tex_rock->bind(progMat->getUniform("Texture0"));
		break;
	case WALL:
		tex_wall->bind(progMat->getUniform("Texture0"));
		break;
	case YELLOW:
		tex_yellow->bind(progMat->getUniform("Texture0"));
		break;
	case ORANGE:
		tex_orange->bind(progMat->getUniform("Texture0"));
		break;
	case EAGLE:
		tex_eagle->bind(progMat->getUniform("Texture0"));
		break;
	case BRANCH:
		tex_branch->bind(progMat->getUniform("Texture0"));
		break;
	case DEFAULT:
		tex_sample->bind(progMat->getUniform("Texture0"));
		break;
	}	
}

void ProgramManager::drawMesh(Mesh i, shared_ptr<Program> shader) {
	switch (i) {
	case CUBE_MESH:
		mesh_cube->draw(shader);
		break;
	case SPHERE_MESH:
		mesh_sphere->draw(shader);
		break;
	case ROCK_MESH:
		mesh_rock->draw(shader);
		break;
	case WALL_MESH:
		mesh_wall->draw(shader);
		break;
	case EAGLE_MESH:
		mesh_eagle->draw(shader);
		break;
	case BRANCH_MESH:
		mesh_branch->draw(shader);
		break;
	}
}
