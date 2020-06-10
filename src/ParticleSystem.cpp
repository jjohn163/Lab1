#include "ParticleSystem.h"
#include "FeatherParticle.h"
#include "ConfettiParticle.h"
#include <stdlib.h>

vector<Particle*> ParticleSystem::particles;

ParticleSystem::ParticleSystem(string resource_dir, string vs_file_path, string fs_file_path) {
	this->resource_dir = resource_dir;
	this->vs_file_path = resource_dir + vs_file_path;
	this->fs_file_path = resource_dir + fs_file_path;

	// Set up mesh and attribute properties
	unsigned int VBO;
	float particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	CHECKED_GL_CALL(glGenVertexArrays(1, &this->VAO));
	CHECKED_GL_CALL(glGenBuffers(1, &VBO));
	CHECKED_GL_CALL(glBindVertexArray(this->VAO));
	// fill mesh buffer
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW));
	// set mesh attributes
	CHECKED_GL_CALL(glEnableVertexAttribArray(0));
	CHECKED_GL_CALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0));
	CHECKED_GL_CALL(glBindVertexArray(0));
	
	// Setup shaders
	prog = make_shared<Program>();
	prog->setVerbose(true);
	prog->setShaderNames(this->vs_file_path, this->fs_file_path);
	prog->init();
	prog->addUniform("P");
	prog->addUniform("V");
	prog->addUniform("M");
	prog->addUniform("offset");
	prog->addUniform("color");
	prog->addAttribute("vertPos");

}

Particle* ParticleSystem::addNewParticle(string particle_name, string particle_type, vec3 position, float rotation, vec3 velocity, float gravityEffect, float lifeLength, float scale) {
	
	// If the particle has not yet been initialized (created texture for it, etc, like below)
	// then do this initialization now
	if (!particle_dictionary.count(particle_name)) {
		GLuint current_id;
		CHECKED_GL_CALL(glGenTextures(1, &current_id));
		particle_dictionary[particle_name] = current_id;

		// Generate Textures
		int width, height, channels;
		char filepath[1000];
		string tex_file_path = get_particle_resource(particle_name);
		strcpy(filepath, tex_file_path.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		CHECKED_GL_CALL(glActiveTexture(GL_TEXTURE0));
		CHECKED_GL_CALL(glBindTexture(GL_TEXTURE_2D, current_id));
		CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
		CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
		CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		CHECKED_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
		//CHECKED_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, P_TEX_WIDTH, P_TEX_HEIGHT, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data)); // What is luminance?
		CHECKED_GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
	}

	if (particle_type == "Confetti") {
		Particle* p = new ConfettiParticle(particle_name, position, rotation, velocity, gravityEffect, lifeLength, scale);
		particles.push_back(p);
		return p;
	}
	else if (particle_type == "Feather") {
		Particle* p = new FeatherParticle(particle_name, position, rotation, velocity, gravityEffect, lifeLength, scale);
		particles.push_back(p);
		return p;
	}
	else {
		Particle* p = new Particle(particle_name, position, rotation, velocity, gravityEffect, lifeLength, scale);
		particles.push_back(p);
		return p;
	}
}

void ParticleSystem::updateParticles(float delta_frame) {
	vector<Particle*>::iterator p_iter = particles.begin();

	while (p_iter != particles.end()) {		
		bool still_alive = (*p_iter)->update(delta_frame);
		if (!still_alive) {
			p_iter = particles.erase(p_iter);
		}
		else {
			p_iter++;
		}
	}
	
}	

mat4 ParticleSystem::faceCamera(mat4 model, mat4 view) {
	mat4 trans_model = model;

	trans_model[0][0] = view[0][0];
	trans_model[0][1] = view[1][0];
	trans_model[0][2] = view[2][0];
	trans_model[1][0] = view[0][1];
	trans_model[1][1] = view[1][1];
	trans_model[1][2] = view[2][1];
	trans_model[2][0] = view[0][2];
	trans_model[2][1] = view[1][2];
	trans_model[2][2] = view[2][2];

	return trans_model;
}

float theta = 0;
void ParticleSystem::render(float delta_frame, mat4 V, vec3 camera) {

	// setup
	prog->bind();

	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(false);

	int i = 0;
	for (i = 0; i < particles.size(); i++) {
		mat4 M = mat4(1);
		M = translate(mat4(1), particles.at(i)->position);
		mat4 trans_M = faceCamera(M, V);
		trans_M *= rotate(mat4(1), particles.at(i)->rotation, vec3(0, 1, 0));
		mat4 sM = scale(mat4(1), vec3(particles.at(i)->scale, particles.at(i)->scale, particles.at(i)->scale));
		//mat4 sM = scale(mat4(1), vec3(1.0f, 1.0f, 1.0f)); // debuggin remove later
		trans_M *= sM;
		//theta++;

		Particle* particle = particles.at(i);
		string name = particles.at(i)->name;

		//M = M * translate(mat4(1), -camera);

		CHECKED_GL_CALL(glActiveTexture(GL_TEXTURE0));
		CHECKED_GL_CALL(glBindTexture(GL_TEXTURE_2D, particle_dictionary[name]));
		CHECKED_GL_CALL(glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V)));
		CHECKED_GL_CALL(glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(trans_M)));
		CHECKED_GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
	}

	// cleanup
	glDepthMask(true);
	glDisable(GL_BLEND);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	prog->unbind();

}

void ParticleSystem::setProjection(mat4 P) {
	prog->bind();
	CHECKED_GL_CALL(glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P)));
	prog->unbind();
};

// Only png is supported at the moment
string ParticleSystem::get_particle_resource(string particle_name) {
	return resource_dir + "/" + particle_name + ".png";
}
