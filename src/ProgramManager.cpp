#include "ProgramManager.h"
#include <stdlib.h>
#include <memory>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Program.h"


using namespace std;
using namespace glm;

Texture * ProgramManager::tex_sample;
Texture * ProgramManager::tex_chick;
Texture * ProgramManager::tex_rock;
Texture * ProgramManager::tex_wall;
Texture * ProgramManager::tex_yellow;
Texture * ProgramManager::tex_orange;

ProgramManager::ProgramManager()
{
}


ProgramManager::~ProgramManager()
{
}

const string ProgramManager::resourceDirectory = "../resources";

// Our shader program
const std::shared_ptr<Program> ProgramManager::progMat = make_shared<Program>();
 
void ProgramManager::init() {
	ProgramManager::progMat->setVerbose(true);
	ProgramManager::progMat->setShaderNames(resourceDirectory + "/my_vert.glsl", resourceDirectory + "/my_frag.glsl");
	ProgramManager::progMat->init();
	ProgramManager::progMat->addUniform("P");
	ProgramManager::progMat->addUniform("V");
	ProgramManager::progMat->addUniform("M");
	ProgramManager::progMat->addUniform("MatDif");
	ProgramManager::progMat->addUniform("MatAmb");
	ProgramManager::progMat->addUniform("MatSpec");
	ProgramManager::progMat->addUniform("shine");
	ProgramManager::progMat->addUniform("LightPos");
	ProgramManager::progMat->addUniform("Texture0");
	ProgramManager::progMat->addAttribute("vertPos");
	ProgramManager::progMat->addAttribute("vertNor");
	ProgramManager::progMat->addAttribute("vertTex");

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
	tex_wall->setFilename(resourceDirectory + "/cliff.jpg");
	tex_wall->init();
	tex_wall->setUnit(0);
	tex_wall->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	tex_yellow = new Texture();
	tex_yellow->setFilename(resourceDirectory + "/yellow.jpg");
	tex_yellow->init();
	tex_yellow->setUnit(0);
	tex_yellow->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	tex_orange = new Texture();
	tex_orange->setFilename(resourceDirectory + "/orange.jpg");
	tex_orange->init();
	tex_orange->setUnit(0);
	tex_orange->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void ProgramManager::setModel(std::shared_ptr<MatrixStack>M) {
	glUniformMatrix4fv(progMat->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
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
		tex_chick->bind(ProgramManager::progMat->getUniform("Texture0"));
		break;
	case ROCK:
		tex_rock->bind(ProgramManager::progMat->getUniform("Texture0"));
		break;
	case WALL:
		tex_wall->bind(ProgramManager::progMat->getUniform("Texture0"));
		break;
	case YELLOW:
		tex_yellow->bind(ProgramManager::progMat->getUniform("Texture0"));
		break;
	case ORANGE:
		tex_orange->bind(ProgramManager::progMat->getUniform("Texture0"));
		break;
	case DEFAULT:
		tex_sample->bind(ProgramManager::progMat->getUniform("Texture0"));
		break;
	}
}
