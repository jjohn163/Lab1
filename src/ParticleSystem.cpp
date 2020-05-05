#include "ParticleSystem.h"

void ParticleSystem::update(float delta_frame) {

	prog->bind();
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(false);

	for (Particle particle : particles) {

	}

}