#include "light.h"

Light::Light()
{
	position.set(50, 50, 0);
	diffuse_color.set(0.6f,0.6f,0.6f);
	specular_color.set(0.6f, 0.6f, 0.6f);
}

void Light::uploadToShader(Shader* shader) {
	shader->setUniform("light_position", position);
	shader->setUniform("light_diffuse", diffuse_color);
	shader->setUniform("light_specular", specular_color);
}


