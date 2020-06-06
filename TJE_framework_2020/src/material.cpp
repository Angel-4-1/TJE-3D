#include "material.h"

Material::Material()
{
	ambient.set(1,1,1); //reflected ambient light
	diffuse.set(1, 1, 1); //reflected diffuse light
	specular.set(1, 1, 1); //reflected specular light
	shininess = 30.0; //glosiness coefficient (plasticity)
}

void Material::uploadToShader(Shader* shader) {
	shader->setUniform("material_ambient", ambient);
	shader->setUniform("material_diffuse", diffuse);
	shader->setUniform("material_specular", specular);
	shader->setUniform("material_shininess", shininess);
}


