#ifndef SCENE_H
#define SCENE_H

#include "framework.h"
#include "entity.h"

class Scene
{
private:
	static Scene* instance;
	// Private constructor to prevent instancing
	Scene();

public:
	// Container of all the entities of our world
	Entity root;

	static Scene* getInstance();
};

#endif