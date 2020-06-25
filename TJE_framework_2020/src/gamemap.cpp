#include "gamemap.h"
#include "extra/textparser.h"
#include "scene.h"

GameMap* GameMap::instance = NULL;

GameMap::GameMap()
{
	for (int i = 0; i < MAX_PROTOTYPES; i++) {
		prototypes[i].index = EMPTY;
		prototypes[i].mesh = NULL;
		prototypes[i].texture = NULL;
		prototypes[i].shader = Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs");
	}
	
	prototypes[(int)PLAYER].index = PLAYER;
	prototypes[(int)PLAYER].mesh = Mesh::Get("data/characters/animation/character3.mesh");
	prototypes[(int)PLAYER].texture = Texture::Get("data/characters/survivorMale.tga");
	prototypes[(int)PLAYER].shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/phong.fs");

	prototypes[(int)CHARACTER].index = CHARACTER;
	prototypes[(int)CHARACTER].mesh = Mesh::Get("data/characters/animation/character3.mesh");
	prototypes[(int)CHARACTER].texture = Texture::Get("data/characters/survivorFemale.tga");
	prototypes[(int)CHARACTER].shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/phong.fs");

	prototypes[(int)ENEMY].index = ENEMY;
	prototypes[(int)ENEMY].mesh = Mesh::Get("data/characters/animation/character3.mesh");
	prototypes[(int)ENEMY].texture = Texture::Get("data/characters/zombie.tga");
	prototypes[(int)ENEMY].shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/phong.fs");

	prototypes[(int)HOUSE].index = HOUSE;
	prototypes[(int)HOUSE].mesh = Mesh::Get("data/buildings/casa.OBJ");
	prototypes[(int)HOUSE].texture = Texture::Get("data/buildings/casa.tga");

	/*****NATURE*****/
	prototypes[(int)TREE].index = TREE;
	prototypes[(int)TREE].mesh = Mesh::Get("data/nature/tree/tree2.OBJ");
	prototypes[(int)TREE].texture = Texture::Get("data/nature/tree/tree2.tga");

	prototypes[(int)TREE3].index = TREE3;
	prototypes[(int)TREE3].mesh = Mesh::Get("data/nature/tree/tree3.OBJ");
	prototypes[(int)TREE3].texture = Texture::Get("data/nature/tree/tree3.tga");

	prototypes[(int)TREE4].index = TREE4;
	prototypes[(int)TREE4].mesh = Mesh::Get("data/nature/tree/treeorange.OBJ");
	prototypes[(int)TREE4].texture = Texture::Get("data/nature/tree/treeorange.tga");

	prototypes[(int)CACTUS].index = CACTUS;
	prototypes[(int)CACTUS].mesh = Mesh::Get("data/buildings/cactus.OBJ");
	prototypes[(int)CACTUS].texture = Texture::Get("data/buildings/cactus.tga");

	prototypes[(int)ORANGEGRASS].index = ORANGEGRASS;
	prototypes[(int)ORANGEGRASS].mesh = Mesh::Get("data/nature/grass/orangegrass.OBJ");
	prototypes[(int)ORANGEGRASS].texture = Texture::Get("data/nature/grass/orangegrass.tga");

	prototypes[(int)FIRE].index = FIRE;
	prototypes[(int)FIRE].mesh = Mesh::Get("data/nature/rocks/fire.OBJ");
	prototypes[(int)FIRE].texture = Texture::Get("data/nature/rocks/fire.tga");

	prototypes[(int)MOUNTAIN].index = MOUNTAIN;
	prototypes[(int)MOUNTAIN].mesh = Mesh::Get("data/nature/rocks/mountain.OBJ");
	prototypes[(int)MOUNTAIN].texture = Texture::Get("data/nature/rocks/mountain.tga");

	prototypes[(int)MOLINO].index = MOLINO;
	prototypes[(int)MOLINO].mesh = Mesh::Get("data/nature/others/molino.OBJ");
	prototypes[(int)MOLINO].texture = Texture::Get("data/nature/others/molino.tga");

	prototypes[(int)ASPA].index = ASPA;
	prototypes[(int)ASPA].mesh = Mesh::Get("data/nature/others/aspa.OBJ");
	prototypes[(int)ASPA].texture = Texture::Get("data/nature/others/aspa.tga");

	/******HOUSES******/
	prototypes[(int)WESTHOUSE].index = WESTHOUSE;
	prototypes[(int)WESTHOUSE].mesh = Mesh::Get("data/buildings/westhouse.OBJ");
	prototypes[(int)WESTHOUSE].texture = Texture::Get("data/buildings/westhouse.tga");

	prototypes[(int)WESTHOUSE2].index = WESTHOUSE2;
	prototypes[(int)WESTHOUSE2].mesh = Mesh::Get("data/buildings/house2.OBJ");
	prototypes[(int)WESTHOUSE2].texture = Texture::Get("data/buildings/house2.tga");

	prototypes[(int)WESTHOUSE3].index = WESTHOUSE3;
	prototypes[(int)WESTHOUSE3].mesh = Mesh::Get("data/buildings/house3.OBJ");	
	prototypes[(int)WESTHOUSE3].texture = Texture::Get("data/buildings/house3.tga");

	prototypes[(int)WESTHOUSE4].index = WESTHOUSE4;
	prototypes[(int)WESTHOUSE4].mesh = Mesh::Get("data/buildings/house4.OBJ");
	prototypes[(int)WESTHOUSE4].texture = Texture::Get("data/buildings/house4.tga");

	prototypes[(int)HOUSECORNER].index = HOUSECORNER;
	prototypes[(int)HOUSECORNER].mesh = Mesh::Get("data/buildings/housecorner.OBJ");
	prototypes[(int)HOUSECORNER].texture = Texture::Get("data/buildings/housecorner.tga");

	prototypes[(int)TIENDACAMP].index = TIENDACAMP;
	prototypes[(int)TIENDACAMP].mesh = Mesh::Get("data/buildings/tiendacamp.OBJ");
	prototypes[(int)TIENDACAMP].texture = Texture::Get("data/buildings/tiendacamp.tga");

	prototypes[(int)TIENDACAMP2].index = TIENDACAMP2;
	prototypes[(int)TIENDACAMP2].mesh = Mesh::Get("data/buildings/tiendacamp2.OBJ");
	prototypes[(int)TIENDACAMP2].texture = Texture::Get("data/buildings/tiendacamp2.tga");

	/*****FENCES*****/
	prototypes[(int)FENCE].index = FENCE;
	prototypes[(int)FENCE].mesh = Mesh::Get("data/nature/fence/fence.OBJ");
	prototypes[(int)FENCE].texture = Texture::Get("data/nature/fence/fence.tga");

	prototypes[(int)FENCEBROKEN].index = FENCEBROKEN;
	prototypes[(int)FENCEBROKEN].mesh = Mesh::Get("data/nature/fence/fencebroken.OBJ");
	prototypes[(int)FENCEBROKEN].texture = Texture::Get("data/nature/fence/fencebroken.tga");

	prototypes[(int)FENCECURVE].index = FENCECURVE;
	prototypes[(int)FENCECURVE].mesh = Mesh::Get("data/nature/fence/fencecurve.OBJ");
	prototypes[(int)FENCECURVE].texture = Texture::Get("data/nature/fence/fencecurve.tga");

	prototypes[(int)FENCEDOOR].index = FENCEDOOR;
	prototypes[(int)FENCEDOOR].mesh = Mesh::Get("data/nature/fence/fencedoor.OBJ");
	prototypes[(int)FENCEDOOR].texture = Texture::Get("data/nature/fence/fencedoor.tga");

	/*****WEAPONS*****/
	prototypes[(int)BULLET].index = BULLET;
	prototypes[(int)BULLET].mesh = Mesh::Get("data/weapons/bullet.OBJ");
	prototypes[(int)BULLET].texture = Texture::Get("data/weapons/bullet.tga");

	prototypes[(int)REVOLVER].index = REVOLVER;
	prototypes[(int)REVOLVER].mesh = Mesh::Get("data/weapons/revolver.OBJ");
	prototypes[(int)REVOLVER].texture = Texture::Get("data/weapons/revolver.tga");

	prototypes[(int)SHOTGUN].index = SHOTGUN;
	prototypes[(int)SHOTGUN].mesh = Mesh::Get("data/weapons/shotgun.OBJ");
	prototypes[(int)SHOTGUN].texture = Texture::Get("data/weapons/shotgun.tga");

	prototypes[(int)MICROGUN].index = MICROGUN;
	prototypes[(int)MICROGUN].mesh = Mesh::Get("data/weapons/microgun.OBJ");
	prototypes[(int)MICROGUN].texture = Texture::Get("data/weapons/microgun.tga");

	/*******ITEMS*******/
	prototypes[(int)HEART].index = HEART;
	prototypes[(int)HEART].mesh = Mesh::Get("data/items/heart.OBJ");
	prototypes[(int)HEART].texture = Texture::Get("data/items/heart.tga");
}


GameMap* GameMap::getInstance()
{
	if (instance == NULL)
	{
		instance = new GameMap();
	}

	return instance;
}

bool GameMap::loadMap(const char* filename)
{
	std::cout << "Reading map file " << filename << " . . . ";
	TextParser tp;
	if (tp.create(filename) == false) {
		std::cout << "File not found" << std::endl;
		return false;
	}

	//number of elements we need to read for creating an entity (type, position, rotation, scale)
	int num_elements = 4;
	int obtained = 0;
	Scene* scene = Scene::getInstance();

	//get enemy manager for creating the enemies
	EnemyManager* enemy_manager = EnemyManager::getInstance();

	//variables to get the elements
	int type = 0;
	float x = 0.0;	//position of the mesh
	float y = 0.0;
	float z = 0.0;
	float rot = 0.0; //rotation
	float sc = 0.0;	 //scale

	char* w;
	while (w = tp.getword()) //returns null at end
	{
		std::string str(w); //convert to string
		if (str == "TYPE") //compare
		{
			type = tp.getint();
			obtained = 1;	//we indicate that we have the type
		}
		else if (str == "POSITION")
		{
			if (obtained == 1) {
				x = tp.getfloat();
				y = tp.getfloat();
				z = tp.getfloat();
				obtained = 2;	//we have the position
			}
		}
		else if (str == "ROTATION")
		{
			if (obtained == 2) {
				rot = tp.getfloat();
				obtained = 3;	//we have the rotation
			}
		}
		else if (str == "SCALE")
		{
			if (obtained == 3) {
				sc = tp.getfloat();
				obtained = 4;	//we have the scale
			}
		}
		else
		{
			//std::cout << "unknown: " << w << std::endl;
			//tp.nextline();
		}

		//if we have all the elements needed to create the entity
		if (obtained == num_elements) {
			sProp* prop = &prototypes[type];	//prototype which contains info about its mesh, texture and shader

			if (prop->index == ENEMY) {
				enemy_manager->createEnemy(Vector3(x, 0, z), Vector3(0, 0, 0), NULL, 10, 50, 1, rot);
			}
			else {

				EntityMesh* ent = new EntityMesh(prop);
				ent->model.setTranslation(x, y, z);
				ent->angle = rot;
				ent->model.rotate(rot * DEG2RAD, Vector3(0, 1, 0));
				ent->setScaleFactor(sc);
				if (ent->prop->index == TREE4) {
					scene->tree->addTree(ent->model);
				}
				else if (ent->prop->index == MOUNTAIN) {
					scene->mountain->addMountain(ent->model);
				}
				else {
					scene->root.addChild(ent);
				}
				obtained = 0;
				if (ent->prop->index == ASPA) {
					scene->addAspas(&ent->model);
				}
				else if (ent->prop->index == HEART || ent->prop->index == REVOLVER || ent->prop->index == SHOTGUN || ent->prop->index == MICROGUN) {
					scene->addItems(&ent->model);
				}
			}
		}

	}
	
	std::cout << "Map loaded" << std::endl;
	return true;
}

void GameMap::saveMap()
{
	FILE* fp = fopen("data/mymap.txt", "w");
	if (fp == NULL)
	{
		std::cout << "ERROR while saving to file" << std::endl;
		return;
	}

	std::cout << "Start saving to file . . ." << std::endl;

	Scene* scene = Scene::getInstance();
	Entity sroot = scene->root;
	

	//save world entities
	for (int i = 0; i < sroot.children.size(); i++) {
		saveEntity(fp, sroot.children[i]);
	}

	//save world trees
	for (int i = 0; i < scene->tree->vertices.size(); i++) {
		Matrix44 tree_model = scene->tree->vertices[i];
		Entity* ent = new Entity(scene->tree->getType());
		ent->model = tree_model;
		saveEntity(fp, ent);
	}

	//save world mountains
	for (int i = 0; i < scene->mountain->vertices.size(); i++) {
		Matrix44 mountain_model = scene->mountain->vertices[i];
		Entity* ent = new Entity(scene->mountain->getType());
		ent->model = mountain_model;
		saveEntity(fp, ent);
	}

	//save enemies
	EnemyManager* enemy_manager = EnemyManager::getInstance();
	saveEnemies(fp, enemy_manager);	

	fclose(fp);
	std::cout << "Saved!" << std::endl;
}

void GameMap::saveEntity(FILE* filename, Entity* ent) {
	
	//extract useful info from the enity
	int type = (int)ent->type;
	if (type == (int)PLAYER || type == (int)EMPTY)	//not save the player or empty entities
		return;

	Vector3 pos = ent->getPosition();
	float scale = ent->getScaleFactor();
	float rotation = ent->angle;

	//the enemy object will be rotated 180 once is included in the enemymanager
	//because the mesh is fliped
	if (ent->type == ENEMY) {
		rotation = ent->angle + 180;
		if (rotation >= 360) {
			rotation = abs((float)360 - rotation);
		}
	}
	
	//save to file
	fprintf(filename, "TYPE %d ", type);
	fprintf(filename, "POSITION %f %f %f ", pos.x, pos.y, pos.z);
	fprintf(filename, "ROTATION %f ", rotation);
	fprintf(filename, "SCALE %f \n", scale);

	//save its children
	for (int i = 0; i < ent->children.size(); i++) {
		saveEntity(filename, ent->children[i]);
	}
}

void GameMap::saveEnemies(FILE* filename, EnemyManager* enemy_manager) {

	for (int i = 0; i < enemy_manager->MAX_ENEMIES; i++) {
		sEnemy* enemy = &enemy_manager->enemies[i];
		if (enemy->isActive == false)
			continue;
		
		//information of the enemy to save
		Vector3 pos = enemy->position;
		float rotation = enemy->initial_angle;
		float scale = 1;
		int type = (int)enemy->prop->index;

		//save to file
		fprintf(filename, "TYPE %d ", type);
		fprintf(filename, "POSITION %f %f %f ", pos.x, pos.y, pos.z);
		fprintf(filename, "ROTATION %f ", rotation);
		fprintf(filename, "SCALE %f \n", scale);
	}
}

void GameMap::reLoadMap(const char* filename)
{
	delete(instance);
	instance = new GameMap();
	loadMap(filename);
}

//create a basic map just in case the map file does not exist
void GameMap::createBasicMap()
{
	Scene* scene = Scene::getInstance();

	int plane_size = 512;

	/****TREES****/
	Image* img_trees = new Image();
	img_trees->loadPNG("data/trees2.png");
	for (int x = -plane_size; x < plane_size; x+=15)
		for (int z = -plane_size; z < plane_size; z+=15) {
			if (rand() % 100 < 80)	//to not have lots of trees
				continue;

			Color pixel = img_trees->getPixel(x + plane_size, z + plane_size);
			if (pixel.r > random() * 255)
				continue;

			//create the entity
			EntityMesh* ent = new EntityMesh(&prototypes[(int)TREE4]);
			ent->model.setTranslation(x, 0, z);
			ent->model.rotate(random() * PI, Vector3(0, 1, 0));
			float scale = 1.0 + random() * 0.3;
			ent->scale = scale;
			ent->model.scale(scale, scale, scale);

			//add to the scene
			scene->root.addChild(ent);
		}

	/****MOUNTAINS****/
	Image* img = new Image();
	img->loadPNG("data/mountain.png");	//mountains only appear on the black zone of this image

	for (int x = -plane_size; x < plane_size; x += 5)
		for (int z = -plane_size; z < plane_size; z += 5) {
			if (rand() % 100 < 95)
				continue;

			//get pixel color of the image img in order to decide if on that point there is going to be a mountain
			Color pixel = img->getPixel(x + plane_size, z + plane_size);
			if (pixel.r > random() * 255)
				continue;

			//create the entity
			EntityMesh* ent = new EntityMesh(&prototypes[(int)MOUNTAIN]);
			ent->model.setTranslation(x, 0, z);
			ent->model.rotate(random() * PI, Vector3(0, 1, 0));
			float scale = 1.0 + random() * 0.5;
			ent->scale = scale;
			ent->model.scale(scale, scale, scale);

			//add to the scene
			scene->root.addChild(ent);
		}
}
