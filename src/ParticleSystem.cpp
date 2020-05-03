#include "ParticleSystem.h"

void ParticleSystem::init() {

	// Generate Textures
	int width, height, channels;
	char filepath[1000];

	strcpy(filepath, tex_file_path.c_str());
	unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
	glGenTextures(1, &particle_tex_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, particle_tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, P_TEX_WIDTH, P_TEX_HEIGHT,0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data); // What is luminance?
	glGenerateMipmap(GL_TEXTURE_2D);

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
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	// fill mesh buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
	// set mesh attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	// Create default particle instances
	for (int i = 0; i < max_particle_count; ++i) {
		particles.push_back(Particle());
	}


	// Setup shaders
	prog = make_shared<Program>();
	prog->setVerbose(true);
	prog->setShaderNames(vs_file_path,fs_file_path);
	prog->init();
	prog->addUniform("P");
	prog->addUniform("offset");
	prog->addUniform("color");
	prog->addAttribute("vertPos");
}

int new_particle_count = 2;		// # of particles to spawn each time
vec3 offset = vec3(0);				// Offset spawn position
void ParticleSystem::update(float delta_time) {

	// Spawn several new particles
	for (unsigned int i = 0; i < new_particle_count; ++i)
	{
		int unusedParticle = firstUnusedParticle();
		respawnParticle(particles[unusedParticle], offset);
	}

	// Update all particles
	for (unsigned int i = 0; i < max_particle_count; ++i)
	{
		Particle &p = particles[i];
		p.life -= delta_time; // reduce life
		if (p.life > 0.0f)
		{	// particle is alive, thus update
			p.position -= p.velocity * delta_time;
			p.color.a -= delta_time * 2.5f;
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	prog->bind();
	for (Particle particle : particles)
	{
		if (particle.life > 0.0f)
		{
			
			glUniform3fv(prog->getUniform("offset"), 1, &particle.position[0]);
			glUniform4fv(prog->getUniform("color"), 1, &particle.color[0]);
			glBindTexture(GL_TEXTURE_2D, particle_tex_id);
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

unsigned int lastUsedParticle = 0;
unsigned int ParticleSystem::firstUnusedParticle()
{
	// search from last used particle, this will usually return almost instantly
	for (unsigned int i = lastUsedParticle; i < max_particle_count; ++i) {
		if (particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// otherwise, do a linear search
	for (unsigned int i = 0; i < lastUsedParticle; ++i) {
		if (particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	// override first particle if all others are alive
	lastUsedParticle = 0;
	return 0;
}

void ParticleSystem::respawnParticle(Particle &particle, glm::vec3 offset)
{
	float random = ((rand() % 100) - 50) / 10.0f;
	float rColor = 0.5f + ((rand() % 100) / 100.0f);
	particle.position = parent->position + random + offset;
	particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.life = 1.0f;
	particle.velocity = parent->velocity * 0.1f;
}