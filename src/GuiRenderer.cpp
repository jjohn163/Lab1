#include "GuiRenderer.h"


void GuiRenderer::init() {
	// Create VAO object that contains the quad we draw into
	unsigned int VBO;
	float quad[] = {
		-1.0, 1.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,

		-1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f, 0.0f
	};

	CHECKED_GL_CALL(glGenVertexArrays(1, &this->quadVAO));
	CHECKED_GL_CALL(glGenBuffers(1, &VBO));
	CHECKED_GL_CALL(glBindVertexArray(this->quadVAO));
	// fill mesh buffer
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW));
	// set mesh attributes
	CHECKED_GL_CALL(glEnableVertexAttribArray(0));
	CHECKED_GL_CALL(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0));
	CHECKED_GL_CALL(glBindVertexArray(0));	// unbind

	// Setup shaders
	this->prog = make_shared<Program>();
	this->prog->setVerbose(true);
	this->prog->setShaderNames(this->vs_file_path, this->fs_file_path);
	this->prog->init();
	this->prog->addUniform("P");
	this->prog->addUniform("V");
	this->prog->addUniform("M");
	this->prog->addUniform("offset");
	this->prog->addUniform("color");
	this->prog->addUniform("scale");
	this->prog->addAttribute("vertPos");
}

void GuiRenderer::render(vector<GuiTexture*> guis, float delta_frame, mat4 V, mat4 P, vec3 camera) {
	
	this->prog->bind();

	// Send to shader once per render call
	//glm::mat4 projection = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
	CHECKED_GL_CALL(glUniformMatrix4fv(this->prog->getUniform("P"), 1, GL_FALSE, value_ptr(P)));
	CHECKED_GL_CALL(glUniformMatrix4fv(this->prog->getUniform("V"), 1, GL_FALSE, value_ptr(V)));

	// Get ready to draw gui textures
	glBindVertexArray(quadVAO);		// load VAO
	glEnableVertexAttribArray(0);	// load VAO attributes
	
	// Some shader settings
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(false);

	static float q = 0.0f;
	q += delta_frame;

	// move gui in front of the camera
	vec3 gui_pos = camera;
	//gui_pos.y -= 24.0f;

	
	cout << "gui_pos: " << gui_pos.x << ", " << gui_pos.y << ", " << gui_pos.z << endl;



	// Draw the gui textures
	for (int i = 0; i < guis.size(); i++) {
		gui_pos.x = camera.x + guis[i]->getPosition().x;
		gui_pos.z = camera.z + guis[i]->getPosition().y;
		
		mat4 M = mat4(1);
		mat4 trans = glm::translate(mat4(1), gui_pos);					// position in front of camera
		M *= trans;
		M = faceCamera(M, V);

		CHECKED_GL_CALL(glUniform2f(prog->getUniform("scale"), guis[i]->getScale().x, guis[i]->getScale().y));
		CHECKED_GL_CALL(glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M)));
		CHECKED_GL_CALL(glBindTexture(GL_TEXTURE_2D, guis[i]->getTexID()));
		CHECKED_GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 6));
		
		
	}

	// cleanup
	glDepthMask(true);
	glDisable(GL_BLEND);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	this->prog->unbind();
}


mat4 GuiRenderer::faceCamera(mat4 model, mat4 view) {
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