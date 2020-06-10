#include "GuiTexture.h"

GLuint GuiTexture::loadTexture(string path) {
	GLuint texID;
	CHECKED_GL_CALL(glGenTextures(1, &texID));


	// Generate Textures
	int width, height, channels;
	char filepath[1000];
	string tex_file_path = path;
	strcpy(filepath, tex_file_path.c_str());
	unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
	CHECKED_GL_CALL(glActiveTexture(GL_TEXTURE0));
	CHECKED_GL_CALL(glBindTexture(GL_TEXTURE_2D, texID));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	CHECKED_GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	CHECKED_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
	//CHECKED_GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, P_TEX_WIDTH, P_TEX_HEIGHT, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data)); // What is luminance?
	CHECKED_GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

	this->texID = texID;	// save copy of texture to this object
	return texID;
}