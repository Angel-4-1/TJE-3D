#ifndef SKYBOX_H
#define SKYBOX_H

#include "framework.h"
#include "includes.h"
#include "utils.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "camera.h"

class SkyBox
{
public:
	SkyBox();
	Shader* skyShader;
	Texture* texture;

	Mesh* cube;

	void render(Matrix44 viewprojection);
};

#endif