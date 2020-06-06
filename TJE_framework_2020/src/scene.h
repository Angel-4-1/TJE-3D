#ifndef SCENE_H
#define SCENE_H

#include "framework.h"
#include "entity.h"
#include "tree.h"
#include "light.h"

class Scene
{
private:
	static Scene* instance;
	// Private constructor to prevent instancing
	Scene();

public:
	// Container of all the entities of our world
	Entity root;
	Player* player;
	Light* light;
	Tree* tree;

	//trees
	static const int MAX_ASPAS = 30;
	
	Matrix44* aspas[MAX_ASPAS];
	//pool of trees
	//Bullet bullets[MAX_BULLETS];

	static Scene* getInstance();

	void updateAspas();
	void addAspas(Matrix44* aspaMatrix);
	void initAspas();
};

#endif