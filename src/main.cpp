
#include <iostream>
#include <glad/glad.h>
#include <cmath>
#include <stdlib.h>
#include <chrono>


#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Texture.h"
#include "TimeManager.h"
#include "CollectionSphere.h"
#include "Enitity.h"
#include "Collider.h"
#include "PlaneCollider.h"
#include "SphereCollider.h"


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
	std::shared_ptr<Program> progMat;
	std::shared_ptr<Program> cubeProg;
	// Shape to be used (from  file) - modify to support multiple
	shared_ptr<Shape> meshfloor;
	shared_ptr<Shape> meshsphere;
	shared_ptr<Shape> goose;

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
	long long lastSpawn = 0;
	
	
	float phi = 0;
	float pheta = 1.5708;
	vec3 lookAtPoint = vec3(0, 0, 1);
	vec3 eye = vec3(0, 10, -5);
	vec3 up = vec3(0, 1, 0);
	vec3 lastPosition = vec3(0, 0, 0);
	const int SPAWN_DELAY = 3000;
	float PLAYER_SPEED = 10.0;
	float SPHERE_SPEED = 5.0;
	const int WIDTH = 30.0;
	const int HEIGHT = 30.0;
	int collected = 0;

	float deltaTime = 0;
	float PI = 3.14159;
	float rotate = 0;
	vec3 light = vec3(-2, 2, 2);
	unsigned int skyTextureId;

	float SPHERE_RADIUS = 1.0;
	float EYE_RADIUS = 2.0;
	vector<shared_ptr<CollectionSphere>> collectionSpheres;
	shared_ptr<Entity> bird;
	vector<shared_ptr<Entity>> entities;

	vector<std::string> faces{
		"iceflow_lf.tga",
		"iceflow_rt.tga",
		"iceflow_up.jpg",
		"iceflow_dn.jpg",
		"iceflow_ft.tga",
		"iceflow_bk.tga"
	};


	void updateLookAtPoint(shared_ptr<Entity> entity) {
		if (phi > 1.5) {
			phi = 1.6;
		}
		else if (phi < -1.5) {
			phi = -1.6;
		}
		lookAtPoint.x = cos(phi) * cos(pheta) + entity->position.x;
		lookAtPoint.y = sin(phi) + entity->position.y;
		lookAtPoint.z = cos(phi) * cos(1.5708 - pheta) + entity->position.z;
		//lookAtPoint.x = cos(phi) * cos(pheta) + eye.x;
		//lookAtPoint.y = sin(phi) + eye.y;
		//lookAtPoint.z = cos(phi) * cos(1.5708 - pheta) + eye.z;
	}

	void updateCamera(shared_ptr<Entity> entity) {
		vec3 difference = entity->position - lastPosition;
		eye += difference;
		lastPosition = entity->position;
		double newX, newY;
		glfwGetCursorPos(window, &newX, &newY);
		if (cursorX >= 0 && cursorY >= 0) {
			//phi += (cursorY - newY) / 100.0;
			//pheta -= (cursorX - newX) / 100.0;
			//updateLookAtPoint(entity);
		}
		updateLookAtPoint(entity);
		cursorX = newX;
		cursorY = newY;

		if (movingForward) {
			//vec3 direction = lookAtPoint - targetPoint;
			vec3 direction = lookAtPoint - entity->position;
			vec3 w = normalize(direction);
			vec3 delta = PLAYER_SPEED * deltaTime * w;
			delta.y = 0;
			entity->velocity += delta;
			//eye += delta;
		}
		if (movingBackward) {
			//vec3 direction = lookAtPoint - targetPoint;
			vec3 direction = lookAtPoint - entity->position;
			vec3 w = normalize(direction);
			vec3 delta = PLAYER_SPEED * deltaTime * w;
			delta.y = 0;
			entity->velocity -= delta;
			//eye -= delta;
		}
		if (movingLeft) {
			//vec3 direction = lookAtPoint - targetPoint;
			vec3 direction = lookAtPoint - entity->position;
			vec3 w = normalize(direction);
			vec3 u = normalize(cross(up, w));
			entity->velocity += PLAYER_SPEED * deltaTime * u;
			//eye += PLAYER_SPEED * deltaTime * u;
		}
		if (movingRight) {
			//vec3 direction = lookAtPoint - targetPoint;w
			vec3 direction = lookAtPoint - entity->position;
			vec3 w = normalize(direction);
			vec3 u = normalize(cross(up, w));
			entity->velocity -= PLAYER_SPEED * deltaTime * u;
			//eye -= PLAYER_SPEED * deltaTime * u;
		}

		eye += ((lookAtPoint+vec3(0,25,-1)) - eye) * deltaTime;

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
			eye.y -= 0.5;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS) {
			eye.y += 0.5;
		}
		if (key == GLFW_KEY_F && action == GLFW_PRESS) {
			cout << "Framerate: " << TimeManager::Instance()->FrameRate() << endl;
		}
		if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		}
	}
	void manageCollisions() {
		for (shared_ptr<Entity> entity : entities) {
			for (shared_ptr<Collider> collider : entity->colliders) {
				std::pair<bool, vec3> result = bird->colliders[0]->isColliding(collider.get(), bird->velocity);
				if (result.first) {
					bird->velocity = result.second;
				}
			}
		}
	}

void mouseCallback(GLFWwindow *window, int button, int action, int mods)
{
	double posX, posY;

	if (action == GLFW_PRESS)
	{
		glfwGetCursorPos(window, &posX, &posY);
		cout << "Pos X " << posX << " Pos Y " << posY << endl;
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
		std::string cracksDirectory = resourceDirectory + "/sb_iceflow/";
		skyTextureId = createSky(cracksDirectory, faces);
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();
		
		bird = make_shared<Entity>(vec3(0, 50, 15), vec3(1.0), vec3(0), true);
		bird->colliders.push_back(make_shared<SphereCollider>(vec3(0, 50, 15), SPHERE_RADIUS));
		bird->velocity.x += 1.f;
		shared_ptr<Entity> ground = make_shared<Entity>(vec3(0, 0, 0), vec3(1.0), vec3(0), false);
		ground->colliders.push_back(make_shared<PlaneCollider>(vec3(0, 0, 1), vec3(-1, 0, 0), vec3(1, 0, 0)));
		entities.push_back(ground);

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		initTex(resourceDirectory);
		// Initialize the GLSL program.

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

		srand(time(NULL));
	}


	void initGeom(const std::string& resourceDirectory)
	{
 		string errStr;
		vector<tinyobj::shape_t> TOshapesFloor;
		vector<tinyobj::material_t> objMaterialsFloor;
		vector<tinyobj::shape_t> TOshapesSphere;
		vector<tinyobj::material_t> objMaterialsSphere;
		vector<tinyobj::shape_t> TOshapesGoose;
		vector<tinyobj::material_t> objMaterialsGoose;


		bool rc = tinyobj::LoadObj(TOshapesFloor, objMaterialsFloor, errStr, (resourceDirectory + "/cube.obj").c_str());
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

		rc = tinyobj::LoadObj(TOshapesGoose, objMaterialsGoose, errStr, (resourceDirectory + "/goose.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			goose = make_shared<Shape>();
			goose->createShape(TOshapesGoose[0]);
			goose->measure();
			goose->init();
		}

		CollectionSphere::spawnEnemies(collectionSpheres, EYE_RADIUS, eye);

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

	void drawEnemies(shared_ptr<MatrixStack> Model) {
		for (shared_ptr<CollectionSphere> sphere : collectionSpheres) {
			if (sphere->isMoving()) {
				SetMaterial(3);
			}
			else {
				SetMaterial(6);
			}
			Model->pushMatrix();
			Model->translate(sphere->getPosition());
			setModel(progMat, Model);
			meshsphere->draw(progMat);
			Model->popMatrix();
		}
	}

	void drawGeese(shared_ptr<MatrixStack> Model) {
		
		SetMaterial(3);
		Model->pushMatrix();
			Model->translate(bird->position);
			//Model->rotate(atan2(sphere->getDirection().x, sphere->getDirection().z), vec3(0, 1, 0));
			Model->scale(vec3(4, 4, 4));
			setModel(progMat, Model);
			meshsphere->draw(progMat);
		Model->popMatrix();
	}

	//void drawGeeseBlue(shared_ptr<MatrixStack> Model) {
	//		SetMaterial(4);
	//		Model->pushMatrix();
	//		Model->translate(targetPoint);
	//		//Model->rotate(atan2(sphere->getDirection().x, sphere->getDirection().z), vec3(0, 1, 0));
	//		Model->scale(vec3(4, 4, 4));
	//		setModel(progMat, Model);
	//		goose->draw(progMat);
	//		Model->popMatrix();
	//}

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
		//deltaTime = TimeManager::Instance()->DeltaTime();
		deltaTime = .02;
		//cout << TimeManager::Instance()->FrameRate() << endl;

		//CollectionSphere::manageCollisions(collectionSpheres, EYE_RADIUS, eye);
		//CollectionSphere::updateEnemies(collectionSpheres);
		//CollectionSphere::spawnEnemies(collectionSpheres, EYE_RADIUS, eye);
		manageCollisions();
		bird->updatePosition(deltaTime);

		// Get current frame buffer size.
		int width, height;
		
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		
		updateCamera(bird);
		
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
		
		
		progMat->bind();
			glUniformMatrix4fv(progMat->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
			glUniformMatrix4fv(progMat->getUniform("V"), 1, GL_FALSE, value_ptr(View));
			glUniform3f(progMat->getUniform("LightPos"), light.x, light.y, light.z);
			Model->pushMatrix();
			Model->loadIdentity();
				Model->rotate(rotate, vec3(0, 1, 0));
				drawFloor(Model);
				drawGeese(Model);
				//drawGeeseBlue(Model);
			Model->popMatrix();
		progMat->unbind();

		//to draw the sky box bind the right shader
		//cubeProg->bind();
		//Model->loadIdentity();
		////Model->translate(vec3(0, 20, 0));
		//Model->scale(vec3(80, 80, 80));
		//
		//
		////set the projection matrix - can use the same one
		//glUniformMatrix4fv(cubeProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		////set the depth function to always draw the box!
		//glDepthFunc(GL_LEQUAL);
		////set up view matrix to include your view transforms 
		////(your code likely will be different depending
		//glUniformMatrix4fv(cubeProg->getUniform("V"), 1, GL_FALSE, value_ptr(View));
		////set and send model transforms - likely want a bigger cube
		//glUniformMatrix4fv(cubeProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		////bind the cube map texture
		//glBindTexture(GL_TEXTURE_CUBE_MAP, skyTextureId);
		////draw the actual cube
		//meshfloor->draw(cubeProg);
		////set the depth test back to normal!
		//glDepthFunc(GL_LESS);
		////unbind the shader for the skybox
		//cubeProg->unbind();
		
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
	windowManager->init(1280, 960);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;
	

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
