#ifndef SCENE_H
#define SCENE_H

#include "framework.h"
#include "entity.h"
#include "tree.h"
#include "mountain.h"
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
	Mountain* mountain;

	//trees
	static const int MAX_ASPAS = 30;
	static const int MAX_ITEMS = 200;
	
	Matrix44* aspas[MAX_ASPAS];
	Matrix44* items[MAX_ITEMS];

	static Scene* getInstance();

	void updateAspas(float seconds_elapsed = 1);
	void addAspas(Matrix44* aspaMatrix);
	void initAspas();

	void updateItems(float seconds_elapsed = 1);
	void addItems(Matrix44* itemMatrix);
	void initItems();

	void resetScene();
};

#endif