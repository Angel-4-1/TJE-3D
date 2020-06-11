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

void Scene::updateAspas(float seconds_elapsed)
{
	for (int i = 0; i < MAX_ASPAS; i++) {
		if (aspas[i] == NULL)
			continue;

		Matrix44* asp = aspas[i];
		Vector3 pos = asp->getTranslation();
		asp->rotate((250*seconds_elapsed) * DEG2RAD, Vector3(1, 0, 0));
	}
}

void Scene::initAspas()
{
	for (int i = 0; i < MAX_ASPAS; i++) {
		aspas[i] = NULL;
	}
}

void Scene::addAspas(Matrix44* aspaMatrix)
{
	for (int i = 0; i < MAX_ASPAS; i++) {
		if (aspas[i] != NULL) {
			continue;
		}
		aspas[i] = aspaMatrix;
		break;
	}
}
