#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "framework.h"
#include "entity.h"

class GameMap
{
private:
	static GameMap* instance;
	
public:
	static const int MAX_PROTOTYPES = 100;
	sProp prototypes[MAX_PROTOTYPES];
	
	GameMap();
	static GameMap* getInstance();

	bool loadMap(const char* filename);
	void saveMap();
	void saveEntity(FILE* filename, Entity* ent);

	void createBasicMap();
};

#endif