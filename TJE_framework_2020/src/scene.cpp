#include "scene.h"

Scene* Scene::instance = NULL;

Scene::Scene() { }

Scene* Scene::getInstance()
{
	if (instance == NULL)
	{
		instance = new Scene();
	}

	return instance;
}
