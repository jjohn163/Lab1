/*
 * Program 3 base code - includes modifications to shape and initGeom in preparation to load
 * multi shape objects 
 * CPE 471 Cal Poly Z. Wood + S. Sueda + I. Dunn
 */

#include <iostream>
#include <glad/glad.h>
#include <cmath>

#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include "TimeManager.h"


#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader/tiny_obj_loader.h>

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;
	GLFWwindow *window = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog;
	std::shared_ptr<Program> progMat;
	std::shared_ptr<Program> progDogs;
	std::shared_ptr<Program> cubeProg;
	// Shape to be used (from  file) - modify to support multiple
	shared_ptr<Shape> mesh;
	shared_ptr<Shape> meshfloor;
	shared_ptr<Shape> meshsphere;
	shared_ptr<Shape> meshdog2;
	shared_ptr<Shape> meshtree;
	shared_ptr<Shape> meshpeng;
	shared_ptr<Texture> texture0;
	shared_ptr<Texture> texture1;
	shared_ptr<Texture> texture2;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	//example data that might be useful when trying to compute bounds on multi-shape
	vec3 gMin;

	//animation data
	float sTheta = 0;
	float gTrans = 0;
	double cursorX = -1;
	double cursorY = -1;
	bool movingForward = false;
	bool movingBackward = false;
	bool movingLeft = false;
	bool movingRight = false;
	string specifiedOBJ = "/bunny.obj";
	vector<vec3> head;
	vector<vec3> body;
	vector<vec3> bottom;
	vector<vec3> positions;
	vector<vec3> treePositions;
	vector<vec3> pengPositions;
	vector<shared_ptr<Shape>> multShape;
	float phi = 0;
	float pheta = 1.5708;
	vec3 lookAtPoint = vec3(0, 0, 1);
	vec3 eye = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);
	int matSelect = 0;
	float shape_scale;
	float speed = .5;
	float SPHERE_SIZE = .025;
	float PI = 3.14159;
	float rotate = 0;
	vec3 light = vec3(-2, 2, 2);
	unsigned int skyTextureId;

	vector<std::string> faces{
		"iceflow_lf.tga",
		"iceflow_rt.tga",
		"iceflow_up.jpg",
		"iceflow_dn.jpg",
		"iceflow_ft.tga",
		"iceflow_bk.tga"
	};

	vec3 crossProduct(vec3 u, vec3 v) {
		return vec3((u.y*v.z - u.z*v.y), (u.z*v.x - u.x*v.z), (u.x*v.y - u.y*v.x));
	}

	void updateLookAtPoint() {
		if (phi > 1.4) {
			phi = 1.5;
		}
		else if (phi < -1.4) {
			phi = -1.5;
		}
		lookAtPoint.x = cos(phi) * cos(pheta) + eye.x;
		lookAtPoint.y = sin(phi) + eye.y;
		lookAtPoint.z = cos(phi) * cos(1.5708 - pheta) + eye.z;
	}

	void updateCamera() {
		double newX, newY;
		glfwGetCursorPos(window, &newX, &newY);
		if (cursorX >= 0 && cursorY >= 0) {
			phi += (cursorY - newY) / 100.0;
			pheta -= (cursorX - newX) / 100.0;
			updateLookAtPoint();
		}
		cursorX = newX;
		cursorY = newY;

		if (movingForward) {
			vec3 direction = lookAtPoint - eye;
			vec3 w = normalize(direction);
			vec3 delta = speed * w;
			delta.y = 0;
			eye += delta;
			lookAtPoint += delta;
		}
		if (movingBackward) {
			vec3 direction = lookAtPoint - eye;
			vec3 w = normalize(direction);
			vec3 delta = speed * w;
			delta.y = 0;
			eye -= delta;
			lookAtPoint -= delta;
		}
		if (movingLeft) {
			vec3 direction = lookAtPoint - eye;
			vec3 w = normalize(direction);
			vec3 u = normalize(cross(up, w));
			eye += speed * u;
			lookAtPoint += speed * u;
		}
		if (movingRight) {
			vec3 direction = lookAtPoint - eye;
			vec3 w = normalize(direction);
			vec3 u = normalize(cross(up, w));
			eye -= speed * u;
			lookAtPoint -= speed * u;
		}
	}

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			movingForward = true;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
			movingForward = false;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			movingBackward = true;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
			movingBackward = false;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			movingLeft = true;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
			movingLeft = false;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			movingRight = true;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
			movingRight = false;
		}
		if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
			light.x -= 0.5;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS) {
			light.x += 0.5;
		}
		if (key == GLFW_KEY_M && action == GLFW_PRESS) {
			matSelect = (matSelect + 1) % 4;
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			 glfwGetCursorPos(window, &posX, &posY);
			 cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY) {
		return;
	}

	unsigned int createSky(string dir, vector<string> faces) {
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		int width, height, nrChannels;
		stbi_set_flip_vertically_on_load(false);
		for (GLuint i = 0; i < faces.size(); i++) {
			unsigned char *data =
				stbi_load((dir + faces[i]).c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else {
				cout << "failed to load: " << (dir + faces[i]).c_str() << endl;
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		cout << " creating cube map any errors : " << glGetError() << endl;
		return textureID;
	}

	// Code to load in the three textures
	void initTex(const std::string& resourceDirectory) {
		texture0 = make_shared<Texture>();
		texture0->setFilename(resourceDirectory + "/cheetah.jpg");
		texture0->init();
		texture0->setUnit(0);
		texture0->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		texture1 = make_shared<Texture>();
		texture1->setFilename(resourceDirectory + "/tiger.jpg");
		texture1->init();
		texture1->setUnit(1);
		texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		texture2 = make_shared<Texture>();
		texture2->setFilename(resourceDirectory + "/grass.jpg");
		texture2->init();
		texture2->setUnit(2);
		texture2->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		std::string cracksDirectory = resourceDirectory + "/sb_iceflow/";
		skyTextureId = createSky(cracksDirectory, faces);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();
		

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		initTex(resourceDirectory);
		// Initialize the GLSL program.
		prog = make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/tex_vert.glsl", resourceDirectory + "/tex_frag0.glsl");
		prog->init();
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		//prog->addUniform("MatDif");
		//prog->addUniform("MatAmb");
		//prog->addUniform("MatSpec");
		prog->addUniform("Texture0");
		//prog->addUniform("shine");
		prog->addUniform("LightPos");
		prog->addAttribute("vertPos");
		prog->addAttribute("vertNor");
		prog->addAttribute("vertTex");


		cubeProg = make_shared<Program>();
		cubeProg->setVerbose(true);
		cubeProg->setShaderNames(resourceDirectory + "/cube_vert.glsl", resourceDirectory + "/cube_frag.glsl");
		cubeProg->init();
		cubeProg->addUniform("P");
		cubeProg->addUniform("V");
		cubeProg->addUniform("M");
		cubeProg->addUniform("skybox");
		cubeProg->addAttribute("vertPos");
		cubeProg->addAttribute("vertNor");
		//prog->addAttribute("vertTex");
		
		progMat = make_shared<Program>();
		progMat->setVerbose(true);
		progMat->setShaderNames(resourceDirectory + "/my_vert.glsl", resourceDirectory + "/my_frag.glsl");
		progMat->init();
		progMat->addUniform("P");
		progMat->addUniform("V");
		progMat->addUniform("M");
		progMat->addUniform("MatDif");
		progMat->addUniform("MatAmb");
		progMat->addUniform("MatSpec");
		progMat->addUniform("shine");
		progMat->addUniform("LightPos");
		progMat->addAttribute("vertPos");
		progMat->addAttribute("vertNor");
		progMat->addAttribute("vertTex");
		
		window = windowManager->getHandle();

	}
	void pushToVector(vec3 newVect, int object) {
		if (object == 0) {
			head.push_back(newVect);
		}
		else if(object == 1){
			body.push_back(newVect);
		}
		else {
			bottom.push_back(newVect);
		}
	}

	void initGeom(const std::string& resourceDirectory)
	{

		//EXAMPLE set up to read one shape from one obj file - convert to read several
		// Initialize mesh
		// Load geometry
 		// Some obj files contain material information.We'll ignore them for this assignment.
 		vector<tinyobj::shape_t> TOshapes;
 		vector<tinyobj::material_t> objMaterials;
 		string errStr;
		vector<tinyobj::shape_t> TOshapesFloor;
		vector<tinyobj::material_t> objMaterialsFloor;
		vector<tinyobj::shape_t> TOshapesSphere;
		vector<tinyobj::material_t> objMaterialsSphere;
		vector<tinyobj::shape_t> TOshapesMult;
		vector<tinyobj::material_t> objMaterialsMult;
		vector<tinyobj::shape_t> TOshapesDog2;
		vector<tinyobj::material_t> objMaterialsDog2;
		vector<tinyobj::shape_t> TOshapesTree;
		vector<tinyobj::material_t> objMaterialsTree;
		vector<tinyobj::shape_t> TOshapesPeng;
		vector<tinyobj::material_t> objMaterialsPeng;

		//load in the mesh and make the shape(s)
		//mesh for the simple spheres that make up the snowmen
 		bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr, (resourceDirectory + "/dog.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		} else {
			mesh = make_shared<Shape>();
			mesh->createShape(TOshapes[0]);
			mesh->measure();
			mesh->init();
		}
		//mesh for the second dog
		rc = tinyobj::LoadObj(TOshapesDog2, objMaterialsDog2, errStr, (resourceDirectory + "/dog2.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			meshdog2 = make_shared<Shape>();
			meshdog2->createShape(TOshapesDog2[0]);
			meshdog2->measure();
			meshdog2->init();
		}

		rc = tinyobj::LoadObj(TOshapesTree, objMaterialsTree, errStr, (resourceDirectory + "/tree.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			meshtree = make_shared<Shape>();
			meshtree->createShape(TOshapesTree[0]);
			meshtree->measure();
			meshtree->init();
		}

		rc = tinyobj::LoadObj(TOshapesPeng, objMaterialsPeng, errStr, (resourceDirectory + "/penguin.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			meshpeng = make_shared<Shape>();
			meshpeng->createShape(TOshapesPeng[0]);
			meshpeng->measure();
			meshpeng->init();
		}

		rc = tinyobj::LoadObj(TOshapesFloor, objMaterialsFloor, errStr, (resourceDirectory + "/cube.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			meshfloor = make_shared<Shape>();
			meshfloor->createShape(TOshapesFloor[0]);
			meshfloor->measure();
			meshfloor->init();
		}

		rc = tinyobj::LoadObj(TOshapesSphere, objMaterialsSphere, errStr, (resourceDirectory + "/sphere.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			meshsphere = make_shared<Shape>();
			meshsphere->createShape(TOshapesSphere[0]);
			meshsphere->measure();
			meshsphere->init();
		}

		rc = tinyobj::LoadObj(TOshapesMult, objMaterialsMult, errStr, (resourceDirectory + specifiedOBJ).c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			float miny = 100000;
			float maxy = -100000;
			for (int i = 0; i < TOshapesMult.size(); i++) {
				shared_ptr<Shape> part = make_shared<Shape>();
				part->createShape(TOshapesMult[i]);
				part->measure();
				part->init();
				multShape.push_back(part);
				if (miny > part->min.y) {
					miny = part->min.y;
				}
				if (maxy < part->max.y) {
					maxy = part->max.y;
				}
			}
			
			shape_scale = 3 / (maxy - miny);
			//cout << "scale = " << shape_scale << endl;
		}
		
		//read out information stored in the shape about its size - something like this...
		//then do something with that information.....
		gMin.x = mesh->min.x;
		gMin.y = mesh->min.y;
		



		treePositions.push_back(vec3(8, 0, 0));
		treePositions.push_back(vec3(-8, 0, 0));
		treePositions.push_back(vec3(0, 0, 8));
		treePositions.push_back(vec3(0, 0, -8));
		treePositions.push_back(vec3(-7, 0, -4));
		treePositions.push_back(vec3(-4, 0, -7));
		treePositions.push_back(vec3(7, 0, -4));
		treePositions.push_back(vec3(4, 0, -7));
		treePositions.push_back(vec3(-7, 0, 4));
		treePositions.push_back(vec3(-4, 0, 7));
		treePositions.push_back(vec3(7, 0, 4));
		treePositions.push_back(vec3(4, 0, 7));


		pengPositions.push_back(vec3(-3, -1, 3));
		pengPositions.push_back(vec3(-2.2, -1, 3.7));
	}

	void setModel(std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack>M) {
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
   }

	void SetMaterial(int i) {
		switch (i) {
		case 0: //shiny blue plastic
			glUniform3f(progMat->getUniform("MatAmb"), 0.02, 0.04, 0.2);
			glUniform3f(progMat->getUniform("MatDif"), 0.0, 0.16, 0.9);
			glUniform3f(progMat->getUniform("MatSpec"), 0.14, 0.2, 0.8);
			glUniform1f(progMat->getUniform("shine"), 120.0);
			break;
		case 1: // flat grey
			glUniform3f(progMat->getUniform("MatAmb"), 0.13, 0.13, 0.14);
			glUniform3f(progMat->getUniform("MatDif"), 0.3, 0.3, 0.4);
			glUniform3f(progMat->getUniform("MatSpec"), 0.3, 0.3, 0.4);
			glUniform1f(progMat->getUniform("shine"), 4.0);
			break;
		case 2: //brass
			glUniform3f(progMat->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
			glUniform3f(progMat->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
			glUniform3f(progMat->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
			glUniform1f(progMat->getUniform("shine"), 27.9);
			break;
		case 3: //green plastic
			glUniform3f(progMat->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
			glUniform3f(progMat->getUniform("MatDif"), 0.2, 0.8, 0.2);
			glUniform3f(progMat->getUniform("MatSpec"), 0.3, 0.3, 0.4);
			glUniform1f(progMat->getUniform("shine"), 80.0);
			break;
		case 4: //light blue
			glUniform3f(progMat->getUniform("MatAmb"), 0.8, 0.8, 0.99);
			glUniform3f(progMat->getUniform("MatDif"), 0.0, 0.1, 0.9);
			glUniform3f(progMat->getUniform("MatSpec"), 0.3, 0.3, 0.4);
			glUniform1f(progMat->getUniform("shine"), 20.);
			break;
		case 5: //purple
			glUniform3f(progMat->getUniform("MatAmb"), 0.2, 0, 0.2);
			glUniform3f(progMat->getUniform("MatDif"), 0.5, 0.0, 0.5);
			glUniform3f(progMat->getUniform("MatSpec"), 0.3, 0.3, 0.4);
			glUniform1f(progMat->getUniform("shine"), 20.);
			break;
		case 6: //red
			glUniform3f(progMat->getUniform("MatAmb"), .2, 0, 0);
			glUniform3f(progMat->getUniform("MatDif"), 1, 0, 0);
			glUniform3f(progMat->getUniform("MatSpec"), 0.3, 0.3, 0.4);
			glUniform1f(progMat->getUniform("shine"), 20.);
			break;
		}
	}



	void drawDogs(shared_ptr<MatrixStack> Model) {
		Model->pushMatrix();
		Model->rotate(1, vec3(0, 1, 0));
		Model->translate(vec3(2.3, -2, 4));
		Model->scale(vec3(.06, .06, .06));
		Model->rotate(-2.9, vec3(0, 1, 0));
		Model->rotate(-1.5, vec3(1, 0, 0));
		Model->translate(vec3(5.777, 16.61, 0));
		setModel(prog, Model);
		texture0->bind(prog->getUniform("Texture0"));
		mesh->draw(prog);
		Model->popMatrix();

		Model->pushMatrix();
		Model->rotate(1, vec3(0, 1, 0));
		Model->translate(vec3(0, -2.2, -4));
		Model->rotate(-1.5, vec3(1, 0, 0));
		Model->scale(vec3(.04, .04, .04));
		Model->translate(vec3(5.777, 16.01, 0));
		setModel(prog, Model);
		texture1->bind(prog->getUniform("Texture0"));
		meshdog2->draw(prog);
		Model->popMatrix();
	}

	void drawPenguins(shared_ptr<MatrixStack> Model) {
		int mat = 5;
		for (vec3 pos : pengPositions) {
			SetMaterial(mat);
			Model->pushMatrix();
				Model->translate(pos);
				Model->scale(vec3(.15, .15, .15));
				Model->rotate(0.8, vec3(0, 1, 0));
				Model->rotate(-1.57, vec3(1, 0, 0));
				Model->translate(vec3(-3.05, 0.38, -0.28));
				setModel(progMat, Model);
				meshpeng->draw(progMat);
			Model->popMatrix();
			mat++;
		}
		
	}
	void drawTrees(shared_ptr<MatrixStack> Model) {
		SetMaterial(3);
		for (vec3 pos : treePositions) {
			Model->pushMatrix();
			Model->translate(pos);
			setModel(progMat, Model);
			meshtree->draw(progMat);
			Model->popMatrix();
		}

	}

	void drawFloor(shared_ptr<MatrixStack> Model) {
		Model->pushMatrix();
			Model->translate(vec3(0, -2, 0));
			Model->scale(vec3(60, .05, 60));
			setModel(progMat, Model);
			SetMaterial(4);
			meshfloor->draw(progMat);
		Model->popMatrix();
	}
	void render() {
		TimeManager::Instance()->Update();
		cout << TimeManager::Instance()->FrameRate() << endl;
		cout << TimeManager::Instance()->DeltaTime() << endl;
		// Get current frame buffer size.
		int width, height;
		
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		
		updateCamera();
		
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		//auto View = make_shared<MatrixStack>();
		mat4 View = glm::lookAt(eye, lookAtPoint, up);
		auto Model = make_shared<MatrixStack>();

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 100.0f);

		// Draw a stack of cubes with indiviudal transforms
		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View));
		glUniform3f(prog->getUniform("LightPos"), light.x, light.y, light.z);
		
			Model->pushMatrix();
				Model->loadIdentity();
				Model->rotate(rotate, vec3(0, 1, 0));
				drawDogs(Model);
			Model->popMatrix();
		prog->unbind();
		
		progMat->bind();
			glUniformMatrix4fv(progMat->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
			glUniformMatrix4fv(progMat->getUniform("V"), 1, GL_FALSE, value_ptr(View));
			glUniform3f(progMat->getUniform("LightPos"), light.x, light.y, light.z);
			Model->pushMatrix();
			Model->loadIdentity();
				Model->rotate(rotate, vec3(0, 1, 0));
				drawFloor(Model);
				drawTrees(Model);
				drawPenguins(Model);
			Model->popMatrix();
		progMat->unbind();

		//to draw the sky box bind the right shader
		cubeProg->bind();
		Model->loadIdentity();
		//Model->translate(vec3(0, 20, 0));
		Model->scale(vec3(80, 80, 80));
		
		
		//set the projection matrix - can use the same one
		glUniformMatrix4fv(cubeProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		//set the depth function to always draw the box!
		glDepthFunc(GL_LEQUAL);
		//set up view matrix to include your view transforms 
		//(your code likely will be different depending
		glUniformMatrix4fv(cubeProg->getUniform("V"), 1, GL_FALSE, value_ptr(View));
		//set and send model transforms - likely want a bigger cube
		glUniformMatrix4fv(cubeProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		//bind the cube map texture
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyTextureId);
		//draw the actual cube
		meshfloor->draw(cubeProg);
		//set the depth test back to normal!
		glDepthFunc(GL_LESS);
		//unbind the shader for the skybox
		cubeProg->unbind();
		
		//animation update example
		sTheta = sin(glfwGetTime());

		// Pop matrix stacks.
		Projection->popMatrix();
		//View->popMatrix();

	}
};

int main(int argc, char *argv[])
{
	// Where the resources are loaded from
	std::string resourceDir = "../resources";
	

	Application *application = new Application();

	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;
	if (argc >= 2)
	{
		std::string name (argv[1]);
		application->specifiedOBJ = "/" + name;
	}

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
