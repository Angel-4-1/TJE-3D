#ifndef ENEMY_H
#define ENEMY_H

#include "includes.h"
#include "utils.h"
#include "framework.h"
#include "entity.h"

struct sEnemy {
	Vector3 position;
	Vector3 velocity;
	Vector3* player_position;
	float speed;
	float health;
	float bullet_damage;
	float angle;
	bool isActive;
	bool isDead;
	sProp* prop;
	Skeleton result_skeleton;

	void setEnemyValues(Vector3 _pos, Vector3 _vel, Vector3* _player_pos, float _speed, float _health, float _bullet_damage, float _angle, bool _isActive, sProp* _prop);
	void setEnemyValues(Vector3 _pos, Vector3 _vel, Vector3* _player_pos, float _speed, float _health, float _bullet_damage, float _angle, bool _isActive);
};

class EnemyManager
{
private:
	static EnemyManager* instance;
	// Private constructor to prevent instancing
	EnemyManager();

public:
	static const int MAX_ENEMIES = 100;

	//pool of enemies
	sEnemy enemies[MAX_ENEMIES];
	float time;

	//vector for instancing
	std::vector<Matrix44>vertices;

	enum eAnimEnemy { IDLE_E, SHOOT_E, SHOOT_MOVING_E, DEAD_E };
	static const int MAX_ANIMATIONS = 4;
	Animation* animations[MAX_ANIMATIONS];

	static EnemyManager* getInstance();

	void initEnemies();
	void render(float time = 0);
	void update(double seconds_elapsed);
	void createEnemy(Vector3 _pos, Vector3 _vel, Vector3* _player_pos, float _speed, float _life, float _bullet_damage, float _angle);
	void selectSkeleton(sEnemy* enemy, float time = 0);
	void onBulletCollision(Bullet* bullet, sEnemy* enemy);
};
#endif