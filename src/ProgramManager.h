#pragma once
#include <stdlib.h>
#include <memory>
#include <vector>
#include "./MatrixStack.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Program.h"
#include "Texture.h"


class ProgramManager
{
public:
	static const std::shared_ptr<Program> progMat;
	static const std::string resourceDirectory;
	static enum Material { BLUE_PLASTIC, FLAT_GREY, BRASS, GREEN_PLASTIC, LIGHT_BLUE, PURPLE, RED, DIRT };
	static enum CustomTextures { CHICK, DEFAULT };
	static void init();
	static void setMaterial(Material i);
	static void setTexture(CustomTextures i);
	static void setModel(std::shared_ptr<MatrixStack> Model);

	ProgramManager();
	~ProgramManager();

private: 

	static Texture * tex_sample;
	static Texture * tex_chick;
	static Texture * tex_rock;
	static Texture * tex_wall;
};