#include "skybox.h"

SkyBox::SkyBox() 
{
    skyShader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
    
    texture = Texture::Get("data/sky/skybox.png");
    cube = Mesh::Get("data/sky/boxsky3.obj");
}

void SkyBox::render(Matrix44 viewprojection) {
	//glDisable(GL_CULL_FACE);
	int size = 512;

	Camera* camera = Camera::current;
	Matrix44 m;
	m.setTranslation(camera->eye.x, camera->eye.y, camera->eye.z);
	m.rotate(180 * DEG2RAD, Vector3(0, 0, 1));
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
