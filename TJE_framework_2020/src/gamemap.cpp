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
		prototypes[i].shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	}

	prototypes[(int)PLAYER].index = PLAYER;
	prototypes[(int)PLAYER].mesh = Mesh::Get("data/characters/characterFlipY.OBJ");
	prototypes[(int)PLAYER].texture = Texture::Get("data/characters/survivorMale.tga");

	prototypes[(int)PLANE].index = PLANE;
	prototypes[(int)PLANE].mesh = Mesh::Get("data/spitfire/spitfire.ASE");
	prototypes[(int)PLANE].texture = Texture::Get("data/spitfire/spitfire_color_spec.tga");

	prototypes[(int)TORPEDO].index = TORPEDO;
	prototypes[(int)TORPEDO].mesh = Mesh::Get("data/bomb/torpedo.ASE");
	prototypes[(int)TORPEDO].texture = Texture::Get("data/bomb/torpedo.tga");

	prototypes[(int)BRIDGE].index = BRIDGE;
	prototypes[(int)BRIDGE].mesh = Mesh::Get("data/bridge/bridge.OBJ");
	prototypes[(int)BRIDGE].texture = Texture::Get("data/bridge/bridge.tga");

	prototypes[(int)HOUSE].index = HOUSE;
	prototypes[(int)HOUSE].mesh = Mesh::Get("data/buildings/casa.OBJ");
	prototypes[(int)HOUSE].texture = Texture::Get("data/buildings/casa.tga");

	prototypes[(int)TREE].index = TREE;
	prototypes[(int)TREE].mesh = Mesh::Get("data/nature/tree/tree2.OBJ");
	prototypes[(int)TREE].texture = Texture::Get("data/nature/tree/tree2.tga");

	prototypes[(int)WESTHOUSE].index = WESTHOUSE;
	prototypes[(int)WESTHOUSE].mesh = Mesh::Get("data/buildings/westhouse.OBJ");
	prototypes[(int)WESTHOUSE].texture = Texture::Get("data/buildings/westhouse.tga");

	prototypes[(int)WESTHOUSE2].index = WESTHOUSE2;
	prototypes[(int)WESTHOUSE2].mesh = Mesh::Get("data/buildings/house2.OBJ");
	prototypes[(int)WESTHOUSE2].texture = Texture::Get("data/buildings/house2.tga");

	prototypes[(int)TREE3].index = TREE3;
	prototypes[(int)TREE3].mesh = Mesh::Get("data/nature/tree/tree3.OBJ");
	prototypes[(int)TREE3].texture = Texture::Get("data/nature/tree/tree3.tga");

	prototypes[(int)CACTUS].index = CACTUS;
	prototypes[(int)CACTUS].mesh = Mesh::Get("data/buildings/cactus.OBJ");
	prototypes[(int)CACTUS].texture = Texture::Get("data/buildings/cactus.tga");

	prototypes[(int)TIENDACAMP].index = TIENDACAMP;
	prototypes[(int)TIENDACAMP].mesh = Mesh::Get("data/buildings/tiendacamp.OBJ");
	prototypes[(int)TIENDACAMP].texture = Texture::Get("data/buildings/tiendacamp.tga");

	prototypes[(int)TIENDACAMP2].index = TIENDACAMP2;
	prototypes[(int)TIENDACAMP2].mesh = Mesh::Get("data/buildings/tiendacamp2.OBJ");
	prototypes[(int)TIENDACAMP2].texture = Texture::Get("data/buildings/tiendacamp2.tga");

	prototypes[(int)ORANGEGRASS].index = ORANGEGRASS;
	prototypes[(int)ORANGEGRASS].mesh = Mesh::Get("data/nature/grass/orangegrass.OBJ");
	prototypes[(int)ORANGEGRASS].texture = Texture::Get("data/nature/grass/orangegrass.tga");

	prototypes[(int)TREE4].index = TREE4;
	prototypes[(int)TREE4].mesh = Mesh::Get("data/nature/tree/treeorange.OBJ");
	prototypes[(int)TREE4].texture = Texture::Get("data/nature/tree/treeorange.tga");

	prototypes[(int)FIRE].index = FIRE;
	prototypes[(int)FIRE].mesh = Mesh::Get("data/nature/rocks/fire.OBJ");
	prototypes[(int)FIRE].texture = Texture::Get("data/nature/rocks/fire.tga");

	prototypes[(int)MOUNTAIN].index = MOUNTAIN;
	prototypes[(int)MOUNTAIN].mesh = Mesh::Get("data/nature/rocks/mountain.OBJ");
	prototypes[(int)MOUNTAIN].texture = Texture::Get("data/nature/rocks/mountain.tga");
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

	//number of elements we need to read for creating an entity (type, position, scale)
	int num_elements = 4;
	int obtained = 0;
	Scene* scene = Scene::getInstance();
	
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
				obtained = 3;
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
			//EntityMesh* ent = new EntityMesh(prop.index, prop.texture, prop.mesh, prop.shader);
			EntityMesh* ent = new EntityMesh(prop);
			ent->model.setTranslation(x, y, z);
			ent->angle = rot;
			ent->model.rotate(rot * DEG2RAD, Vector3(0, 1, 0));
			ent->setScaleFactor(sc);
			scene->root.addChild(ent);
			obtained = 0;
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

	for (int i = 0; i < sroot.children.size(); i++) {
		saveEntity(fp, sroot.children[i]);
	}

	fclose(fp);
	std::cout << "Saved!" << std::endl;
}

void GameMap::saveEntity(FILE* filename, Entity* ent) {
	
	//extract useful info from the enity
	int type = (int)ent->type;
	if (type == (int)PLAYER)	//not save the player
		return;

	Vector3 pos = ent->getPosition();
	float scale = ent->getScaleFactor();
	float rotation = ent->angle;
	
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

//in case there is no file, create a basic map
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
