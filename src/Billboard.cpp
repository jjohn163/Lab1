#include "Billboard.h"



Billboard::Billboard(string resource_dir, string tex_file, string vs_file_path, string fs_file_path) {
	this->resource_dir = resource_dir;
	this->vs_file_path = resource_dir + vs_file_path;
	this->fs_file_path = resource_dir + fs_file_path;
	this->tex_file = resource_dir + tex_file;

	// Set up mesh and attribute properties
	unsigned int VBO;
	float quad[] = {
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
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW));
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

	// Generate Textures
	CHECKED_GL_CALL(glGenTextures(1, &tex_id));

	int width, height, channels;
	char filepath[1000];
	string tex_file_path = tex_file;
	strcpy(filepath, tex_file_path.c_str());
	unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
	CHECKED_GL_CALL(glActiveTexture(GL_TEXTURE0));
	CHECKED_GL_CALL(glBindTexture(GL_TEXTURE_2D, tex_id));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	CHECKED_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
	//CHECKED_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, P_TEX_WIDTH, P_TEX_HEIGHT, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data)); // What is luminance?
	CHECKED_GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

}


void Billboard::render(float delta_frame, mat4 V, vec3 camera) {

	// setup
	prog->bind();

	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(false);

	
	mat4 M = mat4(1);
	//M = translate(mat4(1), particles.at(i)->position);

	// M = T(V) --> models always facing direction of view
	M[0][0] = V[0][0];
	M[0][1] = V[1][0];
	M[0][2] = V[2][0];
	M[1][0] = V[0][1];
	M[1][1] = V[1][1];
	M[1][2] = V[2][1];
	M[2][0] = V[0][2];
	M[2][1] = V[1][2];
	M[2][2] = V[2][2];
	//M *= rotate(mat4(1), particles.at(i)->rotation, vec3(0, 1, 0));
	//mat4 sM = scale(mat4(1), vec3(particles.at(i)->scale, particles.at(i)->scale, particles.at(i)->scale));
	//M = M * sM;
	//theta++;


	CHECKED_GL_CALL(glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V)));
	CHECKED_GL_CALL(glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M)));
	CHECKED_GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));

	

	// cleanup
	glDepthMask(true);
	glDisable(GL_BLEND);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	prog->unbind();

}

void Billboard::setProjection(mat4 P) {
	prog->bind();
	CHECKED_GL_CALL(glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P)));
	prog->unbind();
};