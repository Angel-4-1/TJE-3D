#ifndef ITEM_H
#define ITEM_H

#include "includes.h"
#include "utils.h"
#include "framework.h"

/*
enum eItem { REVOLVER, SHOTGUN, MICROGUN };

struct sGun {
	Vector3 position;
	int num_bullets;
	float scope;	//alcance
	float damage;
	eItem type;
	//sProp* prop;
	//float cadency;

	void setValues(Vector3 _pos, int _bullets, float _scope, float _damage, eItem _type);
};

/*
class Item
{
public:
	virtual void render();
};

class Gun : public Item
{
public:
	static const int MAX_GUNS = 20;
	sGun guns[MAX_GUNS];

	//vector for instancing
	std::vector<Matrix44>vertices;

	void render();
	void initGuns();
	void createGun(Vector3 _pos, int _bullets, float _scope, float _damage, eType _type);
};*/
#endif