#ifndef BULLETMANAGER_H
#define BULLETMANAGER_H

#include "includes.h"
#include "utils.h"
#include "framework.h"

enum eAuthor { EMPTY_BULLET, PLAYER_BULLET, ENEMY_BULLET };

struct Bullet {
	Vector3 position;
	Vector3 last_position;
	Vector3 velocity;
	float time_to_live;	//time left before die
	float power;		//damage that causes
	eAuthor author;			//who shot?
	int type;			//type of bullet
	bool isUsed;
	float angle;		//orientation bullet

	void setBulletValues(Vector3 _pos, Vector3 _vel, float _ttl, float _power, eAuthor _author, int _type, float _angle, bool _isUsed);
};

struct sHitMark {
	Vector3 pos;
	float size;
	float ttl;
};

enum eTolerance { LOW = 8, MEDIUM = 6, HIGH = 4 };

class BulletManager
{
private:
	static BulletManager* instance;
	// Private constructor to prevent instancing
	BulletManager();

public:
	static const int MAX_BULLETS = 100;

	eTolerance tolerance;

	//pool of bullets
	Bullet bullets[MAX_BULLETS];

	//vector for instancing
	std::vector<Matrix44>vertices;

	static BulletManager* getInstance();

	void initBullets();
	
	void render();
	void update(double seconds_elapsed);
	void createBullet(Vector3 _pos, Vector3 _vel, float _ttl, float _power, eAuthor _author, int _type, float _angle);
	
	void hasCollisioned();
	
	/****HIT MARKS****/
	static const int MAX_HITS = 100;
	//sHitMark hit_marks[MAX_HITS];
	std::vector<sHitMark> marks;

	void initHitMarks();
	void createHitMark(Vector3 _pos);
	void renderHitMarks();
};
#endif