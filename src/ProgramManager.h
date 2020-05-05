#pragma once
#include <stdlib.h>
#include <memory>
#include <vector>
#include "./MatrixStack.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Program.h"


class ProgramManager
{
public:
	static const std::shared_ptr<Program> progMat;
	static const std::string resourceDirectory;
	static enum Material { BLUE_PLASTIC, FLAT_GREY, BRASS, GREEN_PLASTIC, LIGHT_BLUE, PURPLE, RED, DIRT };
	static void setMaterial(Material i);
	static void setModel(std::shared_ptr<MatrixStack> Model);

	ProgramManager();
	~ProgramManager();
private: 
	static std::shared_ptr<Program> initProgram();
};