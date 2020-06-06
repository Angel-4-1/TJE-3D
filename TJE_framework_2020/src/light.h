#ifndef LIGHT_H
#define LIGHT_H

#include "framework.h"
#include "shader.h"

//Properties of the light
class Light
{
public:

	Vector3 position; //where is the light
	Vector3 diffuse_color; //the amount (and color) of diffuse
	Vector3 specular_color; //the amount (and color) of specular

	Light();

	//upload uniforms to shader
	void uploadToShader(Shader* shader);
};


#endif
