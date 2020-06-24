#include "scene.h"

Scene* Scene::instance = NULL;

Scene::Scene() { initAspas(); initItems(); }

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

void Scene::initAspas()
{
	for (int i = 0; i < MAX_ASPAS; i++) {
		aspas[i] = NULL;
	}
}

void Scene::updateItems(float seconds_elapsed)
{
	for (int i = 0; i < MAX_ITEMS; i++) {
		if (items[i] == NULL)
			continue;

		Matrix44* item = items[i];
		Vector3 pos = item->getTranslation();
		item->rotate((75 * seconds_elapsed) * DEG2RAD, Vector3(0, 1, 0));
	}
}

void Scene::addItems(Matrix44* itemMatrix)
{
	for (int i = 0; i < MAX_ITEMS; i++) {
		if (items[i] != NULL) {
			continue;
		}
		items[i] = itemMatrix;
		break;
	}
}

void Scene::initItems()
{
	for (int i = 0; i < MAX_ITEMS; i++) {
		items[i] = NULL;
	}
}

void Scene::resetScene()
{
	for (int i = 0; i < root.children.size(); i++)
	{
		Entity* ent = root.children[i];
		if (ent->type == EMPTY || ent == NULL)
			continue;

		ent->isActive = true;
	}
}


