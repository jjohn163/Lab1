
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
#include "ParticleSystem.h"
#include "ProgramManager.h"
#include "OBBCollider.h"


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

	//std::shared_ptr<Program> cubeProg;
	// Shape to be used (from  file) - modify to support multiple
	shared_ptr<Shape> meshfloor;
	shared_ptr<Shape> meshsphere;
	shared_ptr<Shape> meshwall;
	shared_ptr<Shape> meshrock1;
	shared_ptr<Shape> meshChick;
	shared_ptr<Shape> meshPillar;

	ParticleSystem * particleSystem;

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
	bool movingUp = false;
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
	int NUM_ROCKS = 20;

	//the scale at which the rock generation grid is at
	float GRID_SCALE = 5;

	float deltaTime = 0;
	float PI = 3.14159;
	float rotate = 0;
	vec3 light = vec3(2000, 2000, 2000);
	unsigned int skyTextureId;

	float SPHERE_RADIUS = 1.0;
	float EYE_RADIUS = 2.0;
	vector<shared_ptr<CollectionSphere>> collectionSpheres;
	shared_ptr<Entity> bird;
	vector<shared_ptr<Entity>> entities;
	vector<vec3> rockPositions{};

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
	}

	void updateCamera(shared_ptr<Entity> entity) {
		//vec3 difference = entity->position - lastPosition;
		//eye += difference;
		//lastPosition = entity->position;
		//double newX, newY;
		//glfwGetCursorPos(window, &newX, &newY);
		//if (cursorX >= 0 && cursorY >= 0) {
		//	//phi += (cursorY - newY) / 100.0;
		//	//pheta -= (cursorX - newX) / 100.0;
		//	//updateLookAtPoint(entity);
		//}		
		//cursorX = newX;
		//cursorY = newY;

		updateLookAtPoint(entity);
		if (movingForward) {
			vec3 direction = lookAtPoint - entity->position;
			vec3 w = normalize(direction);
			vec3 delta = PLAYER_SPEED * deltaTime * w;
			delta.y = 0;
			entity->velocity += delta;
		}
		if (movingBackward) {
			vec3 direction = lookAtPoint - entity->position;
			vec3 w = normalize(direction);
			vec3 delta = PLAYER_SPEED * deltaTime * w;
			delta.y = 0;
			entity->velocity -= delta;
		}
		if (movingLeft) {
			vec3 direction = lookAtPoint - entity->position;
			vec3 w = normalize(direction);
			vec3 u = normalize(cross(up, w));
			entity->velocity += PLAYER_SPEED * deltaTime * u;
		}
		if (movingRight) {
			vec3 direction = lookAtPoint - entity->position;
			vec3 w = normalize(direction);
			vec3 u = normalize(cross(up, w));
			entity->velocity -= PLAYER_SPEED * deltaTime * u;
		}
		if (movingUp) {
			/*vec3 direction = lookAtPoint - entity->position;
			vec3 w = normalize(direction);
			vec3 u = normalize(cross(up, w));*/
			entity->velocity += deltaTime * (up * vec3(40));
		}

		vec3 pt = lookAtPoint;
		//cout <<"entity: " <<  entity->position.x << ", " << entity->position.y << ", " << entity->position.z << "   eye: " << eye.x << ", " << eye.y << ", " << eye.z << endl;
		eye.y += (pt.y + 25 - eye.y) * deltaTime * 10;
		eye.x += ((pt.x) - eye.x) * deltaTime * 10;
		eye.z += ((pt.z) - eye.z) * deltaTime * 10;
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
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
			movingUp = true;
		}
		if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
			movingUp = false;
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
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	const float GRAVITY = -17.0f;
	const float AIR_RESISTANCE_FACTOR = 0.985;

	void managePhysics(shared_ptr<Entity> entity) {
		entity->velocity.y += GRAVITY * deltaTime;
		entity->velocity.x *= AIR_RESISTANCE_FACTOR;
		entity->velocity.z *= AIR_RESISTANCE_FACTOR;
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

	//Rock generation constants
	double LINE_SLOPE = -8.0;
	float LINE_Y_OFFSET = 0;
	float ROCK_OFFSET_MAX = 4.0;
	float START_VALUE = -NUM_ROCKS*GRID_SCALE;
	float LINE_Z_OFFSET = START_VALUE;
	float COLLISION_PLANE_OFFSET = 12;
	float BIRD_RADIUS = 3.5;


	void addRock(shared_ptr<Entity> rock) {
		const vec3 ROCK_SCALE = vec3(.22, .2, .35);
		const vec3 ROT_AXIS = vec3(1, 0, 0);
		const float ROT_ANGLE = 0;

		vec3 u[3] = { vec3(1,0,0), vec3(0,1,0), vec3(0,0,1) };
		float e[3] = { 2, 2, 2 };
		//rock->colliders.push_back(make_shared<SphereCollider>(position, 4 * WORLD_SCALE));
		rock->colliders.push_back(make_shared<OBBCollider>(rock->position, u, e));
		entities.push_back(rock);
	}

	//Gets random number between -ROCK_OFFSET_MAX and ROCK_OFFSET_MAX
	float randOffset() {
		float numer =(rand() % 100); 
		return numer / (100.0/ROCK_OFFSET_MAX) - ROCK_OFFSET_MAX/2;
	}

	//in the Y-Z plane
	vec3 rockEquation(int z, int x = 0) {
		//double y = pow((0.16 * z), 4);
		double y = LINE_SLOPE * (z + LINE_Z_OFFSET) + LINE_Y_OFFSET;
		return vec3(x, y, z);
	}

	void initWallEntities(string resourceDirectory) {

		float WALL_HEIGHT = 4;
		float WALL_WIDTH = 39;
		int NUM_WALLS_WIDE = 5;

		const vec3 WALL_SCALE = vec3(.2);
		const vec3 ROT_AXIS = vec3(1, 0, 0);
		const float ROT_ANGLE = (PI * 1.5 - atan(LINE_SLOPE));
		const string OBJ_DIR = resourceDirectory + "/cliff_3.obj";

		//Initialize first wall with collider

		vec3 wallStart = rockEquation(START_VALUE) - vec3(0, GRID_SCALE, 0);
		shared_ptr<Entity> wall = make_shared<Entity>(OBJ_DIR, wallStart, WALL_SCALE, ROT_AXIS, false, ProgramManager::RED, ROT_ANGLE);

		//Creates a plane along the slope of the line, and offsets it vertically based on COLLISIONS_PLANE_OFFSET
		wall->colliders.push_back(make_shared<PlaneCollider>(
			vec3(-1, wallStart.y + COLLISION_PLANE_OFFSET, wallStart.z),
			vec3(1, wallStart.y + COLLISION_PLANE_OFFSET, wallStart.z),
			vec3(0, wallStart.y - LINE_SLOPE + COLLISION_PLANE_OFFSET, wallStart.z - 1)));
		entities.push_back(wall);
		
		vec3 wallPos = wallStart;

		for (int curWallsWide = 0; curWallsWide < NUM_WALLS_WIDE; curWallsWide++) {
			//Move wall right for next wall tile (this is still slightly buggy)
			wallPos = wallStart - vec3(WALL_WIDTH*((NUM_WALLS_WIDE / 2 - curWallsWide) / 2), 0, 0);

			for (int i = 0; i < NUM_ROCKS * 2; i++) {
				wall = make_shared<Entity>(OBJ_DIR, wallPos, WALL_SCALE, ROT_AXIS, false, ProgramManager::RED, ROT_ANGLE);
				entities.push_back(wall);

				wallPos += vec3(0, LINE_SLOPE*WALL_HEIGHT, WALL_HEIGHT);
			}
		}
	}

	void initRockEntities(string resourceDirectory) {
		const float SPACE_BETWEEN_ROCKS = GRID_SCALE * 3;

		const string OBJ_DIR = resourceDirectory + "/squareRock.obj";
		const vec3 ROCK_POS = rockEquation(START_VALUE);
		const vec3 ROCK_SCALE = vec3(.22, .2, .35);
		const vec3 ROT_AXIS = vec3(1, 0, 0);
		const float ROT_ANGLE = 0;
		const ProgramManager::Material ROCK_MAT = ProgramManager::BRASS;

		shared_ptr<Entity> rock = make_shared<Entity>(OBJ_DIR, ROCK_POS, ROCK_SCALE, ROT_AXIS, false, ROCK_MAT, ROT_ANGLE);

		//Starting rock
		addRock(rock);

		for (int i = START_VALUE + GRID_SCALE; i <= 0; i += GRID_SCALE) {
			//rock center
			vec3 position1 = rockEquation(i) + vec3(0, randOffset(), 0);
			if (rand() % 2 == 1) {
				addRock(make_shared<Entity>(OBJ_DIR, position1, ROCK_SCALE, ROT_AXIS, false, ROCK_MAT, ROT_ANGLE));
			}

			//rock right
			if (rand() % 2 == 1) {
				vec3 position2 = position1 + vec3(SPACE_BETWEEN_ROCKS , randOffset(), 0);
				addRock(make_shared<Entity>(OBJ_DIR, position2, ROCK_SCALE, ROT_AXIS, false, ROCK_MAT, ROT_ANGLE));
			}

			//rock left
			if (rand() % 2 == 1) {
				vec3 position3 = position1 + vec3(-SPACE_BETWEEN_ROCKS, randOffset(), 0);
				addRock(make_shared<Entity>(OBJ_DIR, position3, ROCK_SCALE, ROT_AXIS, false, ROCK_MAT, ROT_ANGLE));
			}

		}
	}

	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		vec3 rockStart = rockEquation(START_VALUE);

		bird = make_shared<Entity>(
			resourceDirectory + "/Chick.obj",
			vec3(rockStart.x, rockStart.y+GRID_SCALE, rockStart.z+GRID_SCALE),
			vec3(.4, .4, .4),
			vec3(1, 0, 0), 
			true,
			ProgramManager::GREEN_PLASTIC);
		bird->colliders.push_back(make_shared<SphereCollider>(bird->position, BIRD_RADIUS));
		entities.push_back(bird);


		initWallEntities(resourceDirectory);
		initRockEntities(resourceDirectory);

		shared_ptr<Entity> ground = make_shared<Entity>((resourceDirectory + "/cube.obj"), vec3(0, 0, 0), vec3(60, .05, 60), vec3(0), false, ProgramManager::LIGHT_BLUE);
		ground->colliders.push_back(make_shared<PlaneCollider>(vec3(0, 0, 1), vec3(1, 0, 0), vec3(-1, 0, 0)));
		entities.push_back(ground);

		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		initTex(resourceDirectory);
		// Initialize the GLSL program.
		ProgramManager::init();
		//cubeProg = make_shared<Program>();
		//cubeProg->setVerbose(true);
		//cubeProg->setShaderNames(resourceDirectory + "/cube_vert.glsl", resourceDirectory + "/cube_frag.glsl");
		//cubeProg->init();
		//cubeProg->addUniform("P");
		//cubeProg->addUniform("V");
		//cubeProg->addUniform("M");
		//cubeProg->addUniform("skybox");
		//cubeProg->addAttribute("vertPos");
		//cubeProg->addAttribute("vertNor");
		//prog->addAttribute("vertTex");


		window = windowManager->getHandle();

		srand(time(NULL));

		particleSystem = new ParticleSystem( resourceDirectory + "/particle.png", resourceDirectory + "/particle_vert.glsl", resourceDirectory + "/particle_frag.glsl");
		particleSystem->init();
		particleSystem->setParent(bird);
	}


	void render() {
		TimeManager::Instance()->Update();
		deltaTime = TimeManager::Instance()->DeltaTime();
		//deltaTime = .02;
		//cout << TimeManager::Instance()->FrameRate() << endl;


		managePhysics(bird);
		manageCollisions();
		bird->updatePosition(deltaTime);
		updateCamera(bird);

		// Get current frame buffer size.
		int width, height;
		
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		
		
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		//auto View = make_shared<MatrixStack>();
		mat4 View = glm::lookAt(eye, lookAtPoint, vec3(0,0,1));
		auto Model = make_shared<MatrixStack>();

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 1000.0f);

		// Draw a stack of cubes with indiviudal transforms
		
		
		ProgramManager::progMat->bind();
			glUniformMatrix4fv(ProgramManager::progMat->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
			glUniformMatrix4fv(ProgramManager::progMat->getUniform("V"), 1, GL_FALSE, value_ptr(View));
			glUniform3f(ProgramManager::progMat->getUniform("LightPos"), light.x, light.y, light.z);
			Model->pushMatrix();
			Model->loadIdentity();
				Model->rotate(rotate, vec3(0, 1, 0));
				for (shared_ptr<Entity> entity : entities) {
					entity->draw(Model);
				}
			Model->popMatrix();
		ProgramManager::progMat->unbind();

		particleSystem->setViewProjection(Projection->topMatrix(), View, eye);
		particleSystem->update(deltaTime);

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
	std::string resourceDir = ProgramManager::resourceDirectory;
	

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
