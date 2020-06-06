#ifndef MATERIAL_H
#define MATERIAL_H

#include "framework.h"
#include "shader.h"

class Material
{
public:
	Vector3 ambient; //reflected ambient light
	Vector3 diffuse; //reflected diffuse light
	Vector3 specular; //reflected specular light
	float shininess; //glosiness coefficient (plasticity)

	Material();
	
	void uploadToShader(Shader* shader);
};

#endif