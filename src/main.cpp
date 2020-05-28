
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
#include "physx/PxPhysicsAPI.h"
#include "Ragdoll.h"
#include "irrKlang.h"
#include "GLTextureWriter.h"
#include "Billboard.h"
//#include "physx/extensions/PxExtensionsAPI.h"
//#include "physx/common/PxTolerancesScale.h"



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

	//PHYSX
	physx::PxPhysics* mPhysics;
	physx::PxScene* mScene;
	physx::PxMaterial* mMaterial;
	float lastSpeed = 0.f;
	const float MIN_SPEED_CHANGE = 3.f;
	vec3 startPosition;
	shared_ptr<Ragdoll> ragdoll;

	//IRRKLANG
	irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();
	irrklang::ISoundSource* impactSound;
	const std::string IMPACT_SOUND_FILE = "/impact.wav";
	const std::string BACKGROUND_MUSIC_FILE = "/bensound-buddy.mp3";

	//SHADOWS
	GLuint depthMapFBO;
	//const GLuint S_WIDTH = 1024*10, S_HEIGHT = 1024*10;
	const GLuint S_WIDTH = 1024 * 4, S_HEIGHT = 1024 *8;
	GLuint depthMap;
	shared_ptr<Program> DepthProg;
	shared_ptr<Program> ShadowProg;
	shared_ptr<Program> DepthProgDebug;
	bool SHADOW = true;
	bool DEBUG = false;
	bool FIRST = true;
	mat4 LP, LV, LS;
	float EDGE = 500;
	float EDGE_BOT = -1.5f * EDGE;
	float EDGE_TOP = 0.5f * EDGE;
	float TOP_EDGE = -20.0f;

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
	shared_ptr<Shape> meshSkybox;
	shared_ptr<Program> psky;

	ParticleSystem * particleSystem;
	Billboard* billboardUI;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;
	GLuint Texture;

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
	float PLAYER_SPEED = 10.0;
	int NUM_ROCKS = 20;

	float deltaTime = 0;
	float PI = 3.14159;
	float rotate = 0;
	vec3 light = vec3(0, 10000, 0);
	unsigned int skyTextureId;

	float SPHERE_RADIUS = 1.0;
	float EYE_RADIUS = 2.0;
	const string feathers[9] = {
		"feather1",
		"feather2",
		"feather3",
		"feather4",
		"feather5",
		"feather6",
		"feather7",
		"feather8",
		"feather9",
	};
	vector<shared_ptr<CollectionSphere>> collectionSpheres;
	shared_ptr<Entity> bird;
	shared_ptr<Entity> hawk;
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

	// Deferred stuff 
	std::shared_ptr<Program> mergeProg;
	std::shared_ptr<Program> blur_prog;

	//reference to texture FBO
	GLuint gBuffer;
	GLuint gPosition, gNormal, gColorSpec;
	GLuint depthBuf;

	GLuint LframeBuf;
	GLuint LtexBuf;

	GLuint frameBuf[2];
	GLuint texBuf[2];

	bool FirstTime = true;
	bool Defer = true;

	//geometry for texture render
	GLuint quad_VertexArrayID;
	GLuint quad_vertexbuffer;


	physx::PxVec3 vec3GLMtoPhysx(vec3 vector)
	{
		return physx::PxVec3(vector.x, vector.y, vector.z);
	}

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
			bird->body->addForce(physx::PxVec3(0, 0, 100), physx::PxForceMode::eACCELERATION);
		}
		if (movingBackward) {
			vec3 direction = lookAtPoint - entity->position;
			vec3 w = normalize(direction);
			vec3 delta = PLAYER_SPEED * deltaTime * w;
			delta.y = 0;
			entity->velocity -= delta;
			bird->body->addForce(physx::PxVec3(0, 0, -100), physx::PxForceMode::eACCELERATION);
		}
		if (movingLeft) {
			vec3 direction = lookAtPoint - entity->position;
			vec3 w = normalize(direction);
			vec3 u = normalize(cross(up, w));
			entity->velocity += PLAYER_SPEED * deltaTime * u;
			bird->body->addForce(physx::PxVec3(100, 0, 0), physx::PxForceMode::eACCELERATION);
		}
		if (movingRight) {
			vec3 direction = lookAtPoint - entity->position;
			vec3 w = normalize(direction);
			vec3 u = normalize(cross(up, w));
			entity->velocity -= PLAYER_SPEED * deltaTime * u;
			bird->body->addForce(physx::PxVec3(-100, 0, 0), physx::PxForceMode::eACCELERATION);
		}
		if (movingUp) {
			/*vec3 direction = lookAtPoint - entity->position;
			vec3 w = normalize(direction);
			vec3 u = normalize(cross(up, w));*/
			entity->velocity += deltaTime * (up * vec3(40));
		}

		//"tweening" from the juice video
		vec3 target = lookAtPoint + vec3(0, 25, 0);
		eye += ((target - eye) * deltaTime * 3.f);
	} 

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS) {
			bird->body->addForce(physx::PxVec3(0, 0, 100), physx::PxForceMode::eACCELERATION);
			movingForward = true;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
			movingForward = false;
		}
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
			ragdoll->setPosition(startPosition);
			movingUp = true;
		}
		if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
			movingUp = false;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS) {
			bird->body->addForce(physx::PxVec3(0, 0, -100), physx::PxForceMode::eACCELERATION);
			movingBackward = true;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
			movingBackward = false;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS) {
			movingLeft = true;
			bird->body->addForce(physx::PxVec3(100, 0, 0), physx::PxForceMode::eACCELERATION);
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
			movingLeft = false;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS) {
			movingRight = true;
			bird->body->addForce(physx::PxVec3(-100, 0, 0), physx::PxForceMode::eACCELERATION);
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
			movingRight = false;
		}
		if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
			soundEngine->setAllSoundsPaused();
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
		if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
			featherParticle();
			DEBUG = !DEBUG;
		}
		if (key == GLFW_KEY_P && action == GLFW_PRESS) {
			Defer = !Defer;
		}
	}


	/**** geometry set up for a quad *****/
	void initQuad() {
		//now set up a simple quad for rendering FBO
		glGenVertexArrays(1, &quad_VertexArrayID);
		glBindVertexArray(quad_VertexArrayID);

		static const GLfloat g_quad_vertex_buffer_data[] =
		{
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			1.0f,  1.0f, 0.0f,
		};

		glGenBuffers(1, &quad_vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
	}

	void featherParticle() {
		int limit = rand() % 10 + 15;
		for (int i = 0; i < limit; i++) {
			float rrotx = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 180 - 90;
			float rroty = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 180 - 90;
			mat4 rotx = glm::rotate(mat4(1), glm::radians(rrotx), vec3(1, 0, 0));
			mat4 roty = glm::rotate(mat4(1), glm::radians(rroty), vec3(0, 1, 0));

			string feather_name = feathers[rand() % feathers->size()];
			vec3 particle_pos = bird->position + vec3(0,2,0);
			float random_rotation = (rand() % 4) - 2;
			vec3 velocity = vec4(bird->velocity, 1) * rotx * roty;
			float gravity_effect = 1;
			float life_length = rand() % 3 + 2.0f;
			float scale = 0.125f;
			particleSystem->addNewParticle(feather_name, "Feather", particle_pos, random_rotation, velocity, gravity_effect, life_length, scale);
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
					featherParticle();
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

	//Map generation constants
	double LINE_SLOPE = -8.0;
	float LINE_Y_OFFSET = 0.0f;
	float ROCK_OFFSET_MAX = 4.0f;
	float GRID_SCALE = 15; //the scale at which the rock generation grid is at
	float START_HEIGHT = -NUM_ROCKS*GRID_SCALE;
	float LINE_Z_OFFSET = START_HEIGHT;
	float COLLISION_PLANE_OFFSET = 12.0f;
	float BIRD_RADIUS = 3.5f;
	float MAP_WIDTH = 60.0f;


	void addRock(shared_ptr<Entity> rock) {
		const float bb_adjust_x = .75;
		const float bb_adjust_z = .65;

		vec3 u[3] = { vec3(1,0,0), vec3(0,1,0), vec3(0,0,1) };
		float e[3] = { rock->scale.x * GRID_SCALE * bb_adjust_x, rock->scale.y * GRID_SCALE, rock->scale.z * GRID_SCALE * bb_adjust_z };
		rock->colliders.push_back(make_shared<OBBCollider>(rock->position, u, e));
		entities.push_back(rock);
		physx::PxRigidStatic* pxRock = physx::PxCreateStatic(*mPhysics,
			physx::PxTransform(physx::PxVec3(rock->position.x, rock->position.y, rock->position.z)),
			physx::PxBoxGeometry(e[0], e[1], e[2]), *mMaterial);
		mScene->addActor(*pxRock);

	}

	//Gets random number between +- offesetBounds
	float randOffset(float offsetBounds) {
		float numer =(rand() % 100); 
		return numer / (100.0 / (2 * offsetBounds)) - offsetBounds;
	}

	//calculates point of line in the y-z plane.
	vec3 lineEquation(int z, int x = 0) {
		double y = LINE_SLOPE * (z + LINE_Z_OFFSET) + LINE_Y_OFFSET;
		return vec3(x, y, z);
	}

	void initWallEntities(string resourceDirectory) {

		float WALL_HEIGHT = 4.9;
		float WALL_WIDTH = 39;
		int NUM_WALLS_WIDE = 10;

		const vec3 WALL_SCALE = vec3(.2);
		const vec3 ROT_AXIS = vec3(1, 0, 0);
		const float ROT_ANGLE = (PI * 1.5 - atan(LINE_SLOPE));
		//const string OBJ_DIR = resourceDirectory + "/rockyCliff_uv_smooth.obj";

		//Initialize first wall with collider

		vec3 wallStart = lineEquation(START_HEIGHT) - vec3(0, GRID_SCALE, 0);
		shared_ptr<Entity> wall = make_shared<Entity>(ProgramManager::WALL_MESH, wallStart, WALL_SCALE, ROT_AXIS, true, ProgramManager::RED, ROT_ANGLE, ProgramManager::WALL);

		//Creates a plane along the slope of the line, and offsets it vertically based on COLLISIONS_PLANE_OFFSET
		wall->colliders.push_back(make_shared<PlaneCollider>(
			vec3(-1, wallStart.y + COLLISION_PLANE_OFFSET, wallStart.z),
			vec3(1, wallStart.y + COLLISION_PLANE_OFFSET, wallStart.z),
			vec3(0, wallStart.y - LINE_SLOPE + COLLISION_PLANE_OFFSET, wallStart.z - 1)));
		entities.push_back(wall);
		
		vec3 wallPos = wallStart;

		for (int curWallsWide = 0; curWallsWide < NUM_WALLS_WIDE; curWallsWide++) {
			wallPos = wallStart - vec3(WALL_WIDTH*((NUM_WALLS_WIDE / 2 - curWallsWide) / 2), 0, 0);


			for (int i = 0; i < (NUM_ROCKS * (WALL_WIDTH)) / NUM_WALLS_WIDE; i++) {
				wall = make_shared<Entity>(ProgramManager::WALL_MESH, wallPos, WALL_SCALE, ROT_AXIS, true, ProgramManager::RED, ROT_ANGLE, ProgramManager::WALL);
				entities.push_back(wall);
				wallPos += vec3(0, LINE_SLOPE*WALL_HEIGHT, WALL_HEIGHT); //Move down by slope 
			}
		}
		vec3 point1 = lineEquation(START_HEIGHT);
		vec3 point2 = lineEquation(0);
		vec3 point3 = lineEquation(0) + vec3(1, 0, 0);

		physx::PxPlane p1 = physx::PxPlane(vec3GLMtoPhysx(point1), vec3GLMtoPhysx(point2), vec3GLMtoPhysx(point3));
		physx::PxRigidStatic* plane = physx::PxCreatePlane(*mPhysics, p1, *mMaterial);

		if (!plane)
			throw "create plane failed!";
		mScene->addActor(*plane);
	}

	void initRockEntities(string resourceDirectory) {
		const float SPACE_BETWEEN_ROCKS = GRID_SCALE * 3;

		//const string OBJ_DIR = resourceDirectory + "/squareRock.obj";
		const vec3 ROCK_POS = lineEquation(START_HEIGHT);
		const vec3 ROCK_SCALE = vec3(.65, .2, 1);
		const vec3 ROT_AXIS = vec3(1, 0, 0);
		const float ROT_ANGLE = 0;
		const ProgramManager::Material ROCK_MAT = ProgramManager::BRASS;
		const int OFFSET_LEFT = 12*GRID_SCALE / 2; // Sum of widths at grid scale/2
		
		vec3 curPos;
		int omitRand;
		vector<int> widths{ 1, 2, 2, 3, 4 };
		int lastOmitted = widths.size()/2;


		//Starting rock
		addRock(make_shared<Entity>(ProgramManager::ROCK_MESH, ROCK_POS, ROCK_SCALE, ROT_AXIS, false, ROCK_MAT, ROT_ANGLE, ProgramManager::ROCK));

		for (int i = START_HEIGHT + GRID_SCALE; i <= 0; i += GRID_SCALE) {
			curPos = lineEquation(i) - vec3(OFFSET_LEFT, 0, 0);
			random_shuffle(widths.begin(), widths.end());
			do {
				omitRand = rand() % widths.size();
			} while (abs(lastOmitted - omitRand) > 1);
			for (int widthNdx = 0; widthNdx < widths.size(); widthNdx++) {
				curPos += vec3(widths[widthNdx] * GRID_SCALE / 2, 0, 0);
				lastOmitted = omitRand;
				if (widthNdx != omitRand) {
					addRock(make_shared<Entity>(ProgramManager::ROCK_MESH, curPos, ROCK_SCALE*vec3(widths[widthNdx], 1, 1), ROT_AXIS, false, ROCK_MAT, ROT_ANGLE, ProgramManager::ROCK));
				}
				curPos += vec3(widths[widthNdx] * GRID_SCALE / 2, 0, 0);
			}
		}
	}

	/* set up the FBO for storing the light's depth */
	void initShadow() {

		//generate the FBO for the shadow depth
		glGenFramebuffers(1, &depthMapFBO);

		//generate the texture
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, S_WIDTH, S_HEIGHT,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//bind with framebuffer's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		float pos= 500;
		light = bird->position + vec3(pos, pos, pos);
	}

	void initSound(const std::string& resourceDirectory)
	{
		std::string impactFile = (resourceDirectory + IMPACT_SOUND_FILE).c_str();
		impactSound = soundEngine->addSoundSourceFromFile(impactFile.c_str());
		impactSound->setDefaultVolume(2.0);

		std::string backgroundMusic = (resourceDirectory + BACKGROUND_MUSIC_FILE).c_str();
		irrklang::ISoundSource* music = soundEngine->addSoundSourceFromFile(backgroundMusic.c_str());
		music->setDefaultVolume(0.1);
		soundEngine->play2D(music, true);
	}

	void makeSphere(const std::string& resourceDirectory)
	{
		// Initialize the obj mesh VBOs etc
		meshsphere = make_shared<Shape>();
		string errStr;
		vector<tinyobj::shape_t> TOshapesObject;
		vector<tinyobj::material_t> objMaterialsObject;
		bool rc = tinyobj::LoadObj(TOshapesObject, objMaterialsObject, errStr, (resourceDirectory + "/sphere.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			meshsphere = make_shared<Shape>();
			meshsphere->createShape(TOshapesObject[0]);
			meshsphere->measure();
			meshsphere->init();
		}
	}

	void init(const std::string& resourceDirectory)
	{
		//ProgramManager::init();
		GLSL::checkVersion();

		vec3 rockStart = lineEquation(START_HEIGHT);
		startPosition = vec3(rockStart.x, rockStart.y + GRID_SCALE, rockStart.z + GRID_SCALE);
		
		ragdoll = make_shared<Ragdoll>(mPhysics, mScene, mMaterial);
		bird = Ragdoll::createBirdRagdoll(startPosition, entities, ragdoll, resourceDirectory);
		lookAtPoint = startPosition;
		eye = startPosition + vec3(0, 25, 0);


		initWallEntities(resourceDirectory);
		initRockEntities(resourceDirectory);
		initSound(resourceDirectory);

		hawk = make_shared<Entity>(ProgramManager::HAWK_MESH, bird->position + vec3(0, 100, 0), vec3(1.0f, 1.0f, 1.0f), vec3(1, 0, 0), false, ProgramManager::LIGHT_BLUE, 0.0f, ProgramManager::YELLOW);
		entities.push_back(hawk);
		physx::PxRigidDynamic* placeholder = NULL;
		shared_ptr<Entity> ground = make_shared<Entity>(ProgramManager::WALL_MESH, lineEquation(0), vec3(5, 5, 1), vec3(1, 0, 0), true, ProgramManager::LIGHT_BLUE, PI / 2, ProgramManager::WALL, placeholder, 1000.f);
		ground->colliders.push_back(make_shared<PlaneCollider>(vec3(0, ground->position.y, 1), vec3(1, ground->position.y, 0), vec3(-1, ground->position.y, 0)));
		entities.push_back(ground);

		vec3 point1 = vec3(0, ground->position.y, 1); 
		vec3 point2 = vec3(1, ground->position.y, 0);
		vec3 point3 = vec3(-1, ground->position.y, 0);

		physx::PxPlane p2 = physx::PxPlane(vec3GLMtoPhysx(point1), vec3GLMtoPhysx(point2), vec3GLMtoPhysx(point3));
		physx::PxRigidStatic* pxGround = physx::PxCreatePlane(*mPhysics, p2, *mMaterial);

		//physx::PxRigidStatic* pxGround = physx::PxCreatePlane(*mPhysics, physx::PxPlane(0,1,0,0), *mMaterial);
		if (!pxGround)
			throw "create plane failed!";
		mScene->addActor(*pxGround);


		// Set background color.
		glClearColor(.12f, .34f, .56f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);
		initTex(resourceDirectory);
		// Initialize the GLSL program.
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

		meshSkybox = make_shared<Shape>();
		string errStr;
		vector<tinyobj::shape_t> TOshapesObject;
		vector<tinyobj::material_t> objMaterialsObject;
		bool rc = tinyobj::LoadObj(TOshapesObject, objMaterialsObject, errStr, (resourceDirectory + "/spheresmooth.obj").c_str());
		if (!rc) {
			cerr << errStr << endl;
		}
		else {
			meshSkybox = make_shared<Shape>();
			meshSkybox->createShape(TOshapesObject[0]);
			meshSkybox->measure();
			meshSkybox->init();
		}

		int width, height, channels;
		char filepath[1000];

		string str = resourceDirectory + "/arcticpolished.jpg";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);

		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		psky = std::make_shared<Program>();
		psky->setVerbose(true);
		psky->setShaderNames(resourceDirectory + "/skyvertex.glsl", resourceDirectory + "/skyfrag.glsl");
		if (!psky->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		psky->addUniform("P");
		psky->addUniform("V");
		psky->addUniform("M");
		psky->addUniform("campos");
		psky->addAttribute("vertPos");
		psky->addAttribute("vertNor");
		psky->addAttribute("vertTex");

		ShadowProg = make_shared<Program>();
		ShadowProg->setVerbose(true);
		ShadowProg->setShaderNames(resourceDirectory + "/shadow_vert.glsl", resourceDirectory + "/shadow_frag.glsl");
		ShadowProg->init();
		ShadowProg->addUniform("P");
		ShadowProg->addUniform("M");
		ShadowProg->addUniform("V");
		ShadowProg->addUniform("LS");
		ShadowProg->addUniform("lightDir");
		ShadowProg->addAttribute("vertPos");
		ShadowProg->addAttribute("vertNor");
		ShadowProg->addAttribute("vertTex");
		ShadowProg->addUniform("Texture0");
		ShadowProg->addUniform("shadowDepth");

		DepthProg = make_shared<Program>();
		DepthProg->setVerbose(true);
		DepthProg->setShaderNames(resourceDirectory + "/depth_vert.glsl", resourceDirectory + "/depth_frag.glsl");
		DepthProg->init();
		DepthProg->addUniform("LP");
		DepthProg->addUniform("LV");
		DepthProg->addUniform("M");
		DepthProg->addAttribute("vertPos");
		//un-needed, better solution to modifying shape
		DepthProg->addAttribute("vertNor");
		DepthProg->addAttribute("vertTex");
		
		DepthProgDebug = make_shared<Program>();
		DepthProgDebug->setVerbose(true);
		DepthProgDebug->setShaderNames(resourceDirectory + "/depth_vertDebug.glsl", resourceDirectory + "/depth_fragDebug.glsl");
		DepthProgDebug->init();
		DepthProgDebug->addUniform("LP");
		DepthProgDebug->addUniform("LV");
		DepthProgDebug->addUniform("M");
		DepthProgDebug->addAttribute("vertPos");
		//un-needed, better solution to modifying shape
		DepthProgDebug->addAttribute("vertNor");
		DepthProgDebug->addAttribute("vertTex");
		window = windowManager->getHandle();

		srand(time(NULL));
		particleSystem = new ParticleSystem(resourceDirectory, "/particle_vert.glsl", "/particle_frag.glsl");
		initShadow();
		eye = bird->position + vec3(0, 10, 0);


		//set up the shaders to blur the FBO just a placeholder pass thru now
		//next lab modify and possibly add other shaders to complete blur
		mergeProg = make_shared<Program>();
		mergeProg->setVerbose(true);
		mergeProg->setShaderNames(
			resourceDirectory + "/pass_vert.glsl",
			resourceDirectory + "/tex_frag.glsl");
		if (!mergeProg->init()) {
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		mergeProg->addUniform("gBuf");
		mergeProg->addUniform("colorBuf");
		mergeProg->addUniform("norBuf");
		mergeProg->addUniform("lightBuf");
		mergeProg->addAttribute("vertPos");
		mergeProg->addUniform("Ldir");

		cout << "mergeprog " << mergeProg->getPID() << endl;

		//set up the shaders to blur the FBO decomposed just a placeholder pass thru now
		//TODO - modify and possibly add other shaders to complete blur
		blur_prog = make_shared<Program>();
		blur_prog->setVerbose(true);
		blur_prog->setShaderNames(resourceDirectory + "/blur_vert.glsl", resourceDirectory + "/blur_frag.glsl");
		blur_prog->init();
		blur_prog->addUniform("texBuf");
		blur_prog->addUniform("unblurredRadius");
		blur_prog->addUniform("fTime");
		blur_prog->addAttribute("vertPos");


		initBuffers();

		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glGenFramebuffers(1, &LframeBuf);
		glGenTextures(1, &LtexBuf);
		glGenRenderbuffers(1, &depthBuf);
		createFBO(LframeBuf, LtexBuf);

		//set up depth necessary since we are rendering a mesh that needs depth test
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
			width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

		//more FBO set up
		GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, DrawBuffers);

		//Initialize the geometry to render a quad to the screen
		initQuad();

		glGenFramebuffers(2, frameBuf);
		glGenTextures(2, texBuf);
		glGenFramebuffers(2, frameBuf);
		glGenTextures(2, texBuf);
		glGenRenderbuffers(1, &depthBuf);

		//create one FBO
		createFBO(frameBuf[0], texBuf[0]);
		createFBO(frameBuf[1], texBuf[1]);

		billboardUI = new Billboard(resourceDirectory, "/feather1.png", "/particle_vert.glsl", "/particle_frag.glsl");
	}

	mat4 SetOrthoMatrix(shared_ptr<Program> curShade) {
		//float edge = 500;
		//mat4 ortho = glm::ortho(-edge, edge, -edge, edge, 0.1f, 2 * edge);
		mat4 ortho = glm::ortho(-EDGE, EDGE, EDGE_BOT, EDGE_TOP, 0.1f, 2 * EDGE);
		glUniformMatrix4fv(curShade->getUniform("LP"), 1, GL_FALSE, value_ptr(ortho));
		return ortho;
	}

	/* TODO fix */
	mat4 SetLightView(shared_ptr<Program> curShade, vec3 pos, vec3 LA, vec3 up) {
		mat4 Cam = glm::lookAt(pos, LA, up);
		glUniformMatrix4fv(curShade->getUniform("LV"), 1, GL_FALSE, value_ptr(Cam));
		return Cam;
	}


	void initBuffers() {
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

		//initialize the buffers -- from learnopengl.com
		glGenFramebuffers(1, &gBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

		// - position color buffer
		glGenTextures(1, &gPosition);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

		// - normal color buffer
		glGenTextures(1, &gNormal);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

		// - color + specular color buffer
		glGenTextures(1, &gColorSpec);
		glBindTexture(GL_TEXTURE_2D, gColorSpec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColorSpec, 0);

		glGenRenderbuffers(1, &depthBuf);
		//set up depth necessary as rendering a mesh that needs depth test
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

		//more FBO set up
		GLenum DrawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, DrawBuffers);

	}

	void createFBO(GLuint& fb, GLuint& tex) {
		//initialize FBO
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);

		//set up framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
		//set up texture
		glBindTexture(GL_TEXTURE_2D, tex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			cout << "Error setting up frame buffer - exiting" << endl;
			exit(0);
		}
	}

	/*Motion blurs the texture texImageToBlur and outputs the result to targetFrameBuf.
	The larger the motionBlurIterations, the stronger the blurring effect.
	*/
	void motionBlur(GLuint texImageToBlur, GLuint targetFrameBuf, GLuint motionBlurIterations, float unblurredRadius)
	{
		CHECKED_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, targetFrameBuf));
		CHECKED_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		if (motionBlurIterations == 0)
		{
			ProcessDrawTex(texImageToBlur, INFINITY);
		}
		else if (motionBlurIterations == 1)
		{
			ProcessDrawTex(texImageToBlur, unblurredRadius);
		}
		else 
		{
			CHECKED_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, frameBuf[0]));
			CHECKED_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			ProcessDrawTex(texImageToBlur, unblurredRadius);

			for (int i = 0; i < motionBlurIterations - 2; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, frameBuf[(i + 1) % 2]);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				ProcessDrawTex(texBuf[i % 2], unblurredRadius);
			}

			//regardless NOW set up to render to the screen = 0
			CHECKED_GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, targetFrameBuf));
			CHECKED_GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
			/* now draw the actual output  to the default framebuffer - ie display */
			/* note the current base code is just using one FBO and texture - will need
			  to change that  - we pass in texBuf[0] right now */
			ProcessDrawTex(texBuf[motionBlurIterations % 2], unblurredRadius);
		}
	}

	void ProcessDrawTex(GLuint inTex, float unblurredRadius) {

		//set up inTex as my input texture
		CHECKED_GL_CALL(glActiveTexture(GL_TEXTURE0));
		CHECKED_GL_CALL(glBindTexture(GL_TEXTURE_2D, inTex));
		//example applying of 'drawing' the FBO texture
		//this shader just draws right now
		blur_prog->bind();
		CHECKED_GL_CALL(glUniform1i(blur_prog->getUniform("texBuf"), 0));
		CHECKED_GL_CALL(glUniform1f(blur_prog->getUniform("unblurredRadius"), unblurredRadius));
		CHECKED_GL_CALL(glUniform1f(blur_prog->getUniform("fTime"), glfwGetTime()));
		CHECKED_GL_CALL(glEnableVertexAttribArray(0));
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer));
		CHECKED_GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
		CHECKED_GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
		CHECKED_GL_CALL(glDisableVertexAttribArray(0));
		blur_prog->unbind();

	}

	void updateEntities() {
		for (shared_ptr<Entity> entity : entities) {
			if (entity->body) {
				Ragdoll::updateOrientation(entity);
			}
		}
		physx::PxVec3 velocity = bird->body->getLinearVelocity();
		float speed = velocity.magnitude();
		if (lastSpeed - speed > MIN_SPEED_CHANGE) {
			featherParticle();
			soundEngine->play2D(impactSound);
		}
		lastSpeed = speed;
		/*
		float HAWK_SPEED = 10.0f;
		vec3 target = bird->position;
		vec3 direction = target - hawk->position;
		float alternative = 0.5f * length(direction);
		hawk->position += normalize(direction) * fmax(HAWK_SPEED, alternative) * deltaTime;
		*/
	}

	/* VFC code starts here TODO - start here and fill in these functions!!!*/
	vec4 Left, Right, Bottom, Top, Near, Far;
	vec4 planes[6];

	void ExtractVFPlanes(mat4 P, mat4 V) {

		/* composite matrix */
		mat4 comp = P * V;
		vec3 n; //use to pull out normal
		float l; //length of normal for plane normalization

		Left.x = comp[0][3] + comp[0][0]; //m14 + m11
		Left.y = comp[1][3] + comp[1][0]; //m24 + m21
		Left.z = comp[2][3] + comp[2][0]; //m34 + m31
		Left.w = comp[3][3] + comp[3][0]; //m44 + m41
		n = vec3(Left.x, Left.y, Left.z);
		l = length(n);
		planes[0] = Left / l;
		Left = planes[0];

		Right.x = comp[0][3] - comp[0][0];
		Right.y = comp[1][3] - comp[1][0];
		Right.z = comp[2][3] - comp[2][0];
		Right.w = comp[3][3] - comp[3][0];
		n = vec3(Right.x, Right.y, Right.z);
		l = length(n);
		planes[1] = Right / l;
		Right = planes[1];

		Bottom.x = comp[0][3] + comp[0][1];
		Bottom.y = comp[1][3] + comp[1][1];
		Bottom.z = comp[2][3] + comp[2][1];
		Bottom.w = comp[3][3] + comp[3][1];
		n = vec3(Bottom.x, Bottom.y, Bottom.z);
		l = length(n);
		planes[2] = Bottom / l;
		Bottom = planes[2];

		Top.x = comp[0][3] - comp[0][1];
		Top.y = comp[1][3] - comp[1][1];
		Top.z = comp[2][3] - comp[2][1];
		Top.w = comp[3][3] - comp[3][1];
		n = vec3(Top.x, Top.y, Top.z);
		l = length(n);
		planes[3] = Top / l;

		Near.x = comp[0][3] + comp[0][2]; //m14 + m11
		Near.y = comp[1][3] + comp[1][2]; //m24 + m21
		Near.z = comp[2][3] + comp[2][2]; //m34 + m31
		Near.w = comp[3][3] + comp[3][2]; //m44 + m41
		n = vec3(Near.x, Near.y, Near.z);
		l = length(n);
		planes[4] = Near / l;

		Far.x = comp[0][3] - comp[0][0];
		Far.y = comp[1][3] - comp[1][0];
		Far.z = comp[2][3] - comp[2][0];
		Far.w = comp[3][3] - comp[3][0];
		n = vec3(Far.x, Far.y, Far.z);
		l = length(n);
		planes[5] = Far / l;
		Far = planes[5];
	}


	/* helper function to compute distance to the plane */
	/* TODO: fill in */
	float DistToPlane(float A, float B, float C, float D, vec3 point) {
		return A * point.x + B * point.y + C * point.z + D;
	}

	/* Actual cull on planes */
	//returns 1 to CULL
	int ViewFrustCull(vec3 center, float radius) {

		float dist;
		//cout << "testing against all planes" << endl;
		for (int i = 0; i < 6; i++) {
			dist = DistToPlane(planes[i].x, planes[i].y, planes[i].z, planes[i].w, center);
			//test against each plane
			if (dist < (-1) * radius) {
				return 1;
			}
		}
		return 0;
	}

	void render() {
		TimeManager::Instance()->Update();
		deltaTime = TimeManager::Instance()->DeltaTime();
		mScene->simulate(deltaTime);

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		
		// Create the matrix stacks - please leave these alone for now
		auto Projection = make_shared<MatrixStack>();
		mat4 View = glm::lookAt(eye, lookAtPoint, vec3(0, 0, 1));
		auto Model = make_shared<MatrixStack>();

		vec3 lightLA = bird->position;
		vec3 lightUp = vec3(0, 1, 0);
		
		if (SHADOW) {
			//set up light's depth map
			glViewport(0, 0, S_WIDTH, S_HEIGHT);

			//sets up the output to be out FBO
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glCullFace(GL_FRONT);

			//set up shadow shader and render the scene
			DepthProg->bind();
			//TODO you will need to fix these
			LP = SetOrthoMatrix(DepthProg);
			LV = SetLightView(DepthProg, light, lightLA, lightUp);
			LS = LP * LV;
			//drawScene(DepthProg, 0, 0);
			Model->pushMatrix();
			Model->loadIdentity();
			//Model->rotate(rotate, vec3(0, 1, 0));
			for (shared_ptr<Entity> entity : entities) {
				float difference = bird->position.y - entity->position.y;
				if (!entity->isPlane && (difference < -EDGE_BOT && difference > TOP_EDGE)) {
					entity->draw(Model, DepthProg);
				}
			}
			Model->popMatrix();
			DepthProg->unbind();

			//set culling back to normal
			glCullFace(GL_BACK);

			//this sets the output back to the screen
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Use the matrix stack for Lab 6
		float aspect = width/(float)height;

		// Apply perspective projection.
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 10000.0f);
		//Draw skybox

		psky->bind();

		float sangle = 3.1415926 / 2.;
		glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(-1.0f, 0.0f, 0.0f));

		glm::mat4 V, M, P;
		V = glm::mat4(1);
		M = glm::translate(glm::mat4(1.0f), eye + vec3(0, -1, 0)) * RotateXSky;
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width / (float)height), 0.1f, 1000.0f);

		vec3 campos = eye;

		//glm::vec3 camp = eye;
		//glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
		//glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(1, 1, 1));

		//M = TransSky * RotateXSky * SSky;

		//send the matrices to the shaders
		glUniformMatrix4fv(psky->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(psky->getUniform("V"), 1, GL_FALSE, value_ptr(View));
		glUniformMatrix4fv(psky->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(psky->getUniform("campos"), 1, &campos[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glDisable(GL_DEPTH_TEST);
		meshSkybox->draw(psky);
		glEnable(GL_DEPTH_TEST);
		
		psky->unbind();
		/*
		if (DEBUG) {
			DepthProgDebug->bind();
			//render scene from light's point of view
			SetOrthoMatrix(DepthProgDebug);
			SetLightView(DepthProgDebug, light, lightLA, lightUp);
			Model->pushMatrix();
			Model->loadIdentity();
			for (shared_ptr<Entity> entity : entities) {
				entity->draw(Model, DepthProgDebug);
			}
			Model->popMatrix();
			//drawScene(DepthProgDebug, ShadowProg->getUniform("Texture0"), 0);
			DepthProgDebug->unbind();
		}
		else {
			// Draw a stack of cubes with indiviudal transforms
			shared_ptr<Program> prog = ProgramManager::Instance()->progMat;
			prog->bind();
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			glUniform1i(prog->getUniform("shadowDepth"), 1);
			glUniformMatrix4fv(prog->getUniform("LS"), 1, GL_FALSE, value_ptr(LS));
			glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
			glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View));
			glUniform3f(prog->getUniform("lightDir"), light.x, light.y, light.z);
			ExtractVFPlanes(Projection->topMatrix(), View);
			Model->pushMatrix();
			Model->loadIdentity();
			//Model->rotate(rotate, vec3(0, 1, 0));
			for (shared_ptr<Entity> entity : entities) {
				float difference = bird->position.y - entity->position.y;

				if (!ViewFrustCull(entity->position, entity->cullRadius) && (entity->isPlane || (difference < 1500 && difference > -100))) {
					entity->draw(Model);
				}
			}
			Model->popMatrix();
			prog->unbind();
		}
		*/

		// Begin rendering objects in the scene
		// Bind to the gbuffer or to the screen (0)
		if (Defer) glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		else glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ProgramManager::Instance()->progMat->bind();
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glUniform1i(ProgramManager::Instance()->progMat->getUniform("shadowDepth"), 1);
		glUniformMatrix4fv(ProgramManager::Instance()->progMat->getUniform("LS"), 1, GL_FALSE, value_ptr(LS));
		glUniformMatrix4fv(ProgramManager::Instance()->progMat->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(ProgramManager::Instance()->progMat->getUniform("V"), 1, GL_FALSE, value_ptr(View));
		glUniform3f(ProgramManager::Instance()->progMat->getUniform("LightPos"), light.x, light.y, light.z);
		Model->pushMatrix();
		Model->loadIdentity();
		//Model->rotate(rotate, vec3(0, 1, 0));
		for (shared_ptr<Entity> entity : entities) {
			float difference = bird->position.y - entity->position.y;

			if (entity->isPlane || (difference < 1500 && difference > -100)) {
				entity->draw(Model);
			}
		}
		Model->popMatrix();
		ProgramManager::Instance()->progMat->unbind();
		
		if (Defer) {
			float blurVelocityRequirement = 150;

			physx::PxVec3 velocity = bird->body->getLinearVelocity();
			
			float unblurredRadius = abs(blurVelocityRequirement / velocity.y);

			// This facilitates a very smooth transition to showing the blur without making the blur show too early.
			if (unblurredRadius > 1)
			{
				unblurredRadius = pow(unblurredRadius, 16);
			}

			//cout << "unblurredRadius: " << "(" << unblurredRadius << ")" << endl;

			// The blur starts to show when the velocity.y > 125, but 150 is when it's really honed in on the bird
			motionBlur(gColorSpec, 0, 6, unblurredRadius);

			///* now draw the actual output */
			//glBindFramebuffer(GL_FRAMEBUFFER, 0);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			//// example applying of 'drawing' the FBO texture - change shaders
			//mergeProg->bind();
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, gPosition);
			//glActiveTexture(GL_TEXTURE0 + 1);
			//glBindTexture(GL_TEXTURE_2D, gNormal);
			//glActiveTexture(GL_TEXTURE0 + 2);
			//glBindTexture(GL_TEXTURE_2D, gColorSpec);
			//glActiveTexture(GL_TEXTURE0 + 3);
			//glBindTexture(GL_TEXTURE_2D, LtexBuf);
			//glUniform1i(mergeProg->getUniform("gBuf"), 0);
			//glUniform1i(mergeProg->getUniform("norBuf"), 1);
			//glUniform1i(mergeProg->getUniform("colorBuf"), 2);
			//glUniform1i(mergeProg->getUniform("lightBuf"), 3);
			////glUniform3f(texProg->getUniform("Ldir"), g_light.x, g_light.y, g_light.z);
			//glEnableVertexAttribArray(0);
			//glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
			//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
			//glDrawArrays(GL_TRIANGLES, 0, 6);
			//glDisableVertexAttribArray(0);
			//mergeProg->unbind();

			if (FirstTime)
			{
				assert(GLTextureWriter::WriteImage(gBuffer, "gBuf.png"));
				assert(GLTextureWriter::WriteImage(gPosition, "gPos.png"));
				assert(GLTextureWriter::WriteImage(gNormal, "gNorm.png"));
				assert(GLTextureWriter::WriteImage(gColorSpec, "gColorSpec.png"));
				FirstTime = false;
			}
		}

		particleSystem->setProjection(Projection->topMatrix());
		particleSystem->updateParticles(deltaTime);
		particleSystem->render(deltaTime, View, eye);
		

		billboardUI->setProjection(Projection->topMatrix());
		billboardUI->render(deltaTime, View, eye);

		//animation update example
		sTheta = sin(glfwGetTime());

		// Pop matrix stacks.
		Projection->popMatrix();
		//View->popMatrix();
		mScene->fetchResults();
		updateEntities();
		updateCamera(bird);
		light = bird->position + vec3(50, 50, 50);
		//cout << bird->position.y << endl;
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
