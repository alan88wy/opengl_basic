#pragma once

#include <GL/glew.h>
#include "CommonValues.h"

class Texture
{
public:
	Texture();
	Texture(const char* fileLoc);

	bool LoadTexture();  // RGB
	bool LoadTextureA(); // RGBA, Texture with Alpha Channel

	void UseTexture();
	void ClearTexture();

	~Texture();

private:
	GLuint textureID;
	int width, height, bitDepth;

	const char* fileLocation;
};

