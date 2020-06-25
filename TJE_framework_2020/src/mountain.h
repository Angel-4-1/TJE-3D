#ifndef MOUNTAIN_H
#define MOUNTAIN_H

#include "includes.h"
#include "utils.h"
#include "framework.h"
#include "entity.h"

class Mountain
{
public:
	eType mountain_type;

	//vector for instancing
	std::vector<Matrix44>vertices;

	Mountain();

	void render();
	eType getType();
	void addMountain(Matrix44 mountain);
};
#endif