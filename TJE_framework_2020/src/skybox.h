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
	static const int NUM_TEXTURES = 6;
	Uint8* data_files[NUM_TEXTURES];
	SkyBox();
	Shader* skyShader;
	Texture* texture;
	Image* img1;
	Image* img2;
	Image* img3;
	Image* img4;
	Image* img5;
	Image* img6;
	Mesh* cube;

	void render(Matrix44 viewprojection) {
		//glDisable(GL_CULL_FACE);
		int size = 512;
		//texture->createCubemap(size,size, data_files);
		texture= Texture::Get("data/sky/skybox.png");

		Camera* camera = Camera::current;
		Matrix44 m;
		m.setTranslation(camera->eye.x, camera->eye.y, camera->eye.z);
        skyShader->enable();
        //draw skybox
		
		skyShader->setUniform("u_color", Vector4(1, 1, 1, 1));
        skyShader->setUniform("u_viewprojection", camera->viewprojection_matrix);
        skyShader->setUniform("u_direction", camera->center);
        skyShader->setUniform("u_position", camera->eye);
		skyShader->setUniform("u_skybox", texture, 0);
		skyShader->setUniform("u_model", m);

		skyShader->setUniform("u_position", camera->eye);
		skyShader->setUniform("u_direction", camera->center);
		cube->render(GL_TRIANGLES);
		skyShader->disable();
		//glEnable(GL_CULL_FACE);
	}
};

#endif