#ifndef TREE_H
#define TREE_H

#include "includes.h"
#include "utils.h"
#include "framework.h"
#include "entity.h"

class Tree
{
public:
	eType trees_type;

	//vector for instancing
	std::vector<Matrix44>vertices;

	Tree();

	void render();
	eType getType();
	void addTree(Matrix44 tree);
};
#endif