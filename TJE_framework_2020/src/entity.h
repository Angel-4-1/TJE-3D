#ifndef ENTITY_H
#define ENTITY_H

#include "framework.h"
#include <string>
#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"
#include "bulletmanager.h"
#include "animation.h"
//#include "item.h"

enum eType { EMPTY, PLAYER, CHARACTER, HEART, ENEMY, HOUSE, TREE, WESTHOUSE, WESTHOUSE2 , TREE3, CACTUS, 
	TIENDACAMP, TIENDACAMP2, ORANGEGRASS, TREE4, FIRE, MOUNTAIN, WESTHOUSE3, HOUSECORNER, WESTHOUSE4,
	FENCE, FENCEBROKEN, FENCECURVE, FENCEDOOR,
	BULLET, REVOLVER, SHOTGUN, MICROGUN,
	MOLINO, ASPA };

struct sProp {
	eType index;
	Mesh* mesh;
	Texture* texture;
	Shader* shader;
};

class Entity
{
public:
	Entity();
	Entity(eType _type);
	virtual ~Entity();

	Matrix44 model;
	float angle;	//yaw

	eType type;
	BoundingBox aabb;

	bool isActive;
	
	float scale;

	virtual void render();
	virtual void update(double seconds_elapsed);

	Vector3 getPosition();
	float getScaleFactor();
	void setScaleFactor(float _scale);

	// To implement a tree
	Entity* parent;
	std::vector<Entity*> children;

	void addChild(Entity* child);
	void removeChild(Entity* child);

	// Transform matrix will be the concanetation of all the transform that affects this entity
	Matrix44 getGlobalMatrix();

	virtual void onBulletCollision(Bullet* bullet);
};

// Object of the scene with visual representation
class EntityMesh : public Entity
{
public:
	EntityMesh(sProp* _prop);
	sProp* prop;

	void render();
	void update(double seconds_elapsed);
	void onBulletCollision(Bullet* bullet);
};

class Gun : public EntityMesh
{
public:
	Vector3 position;
	int num_bullets;
	float scope;	//alcance
	float damage;
	//float cadency;

	Gun(Vector3 _pos, int _bullets, sProp* _prop);
};

class Player : public EntityMesh
{
public:
	Vector3 position;
	Vector3 velocity;
	float speed;
	bool hasGun;
	bool isHit;
	float health;
	float max_health;
	float when_was_hitted;

	enum eGunType { REVOLVER_GUN, SHOTGUN_GUN, MICROGUN_GUN };
	Gun* gun;
	int current_gun;	//index of the array which gun points to
	static const int MAX_GUNS = 3;
	Gun* guns[MAX_GUNS];

	enum eAnim { STAND_UP, IDLE, WALK, RUN, SHOOT_NOT_MOVING, SHOOT_MOVING, HIT, DEAD };
	static const int MAX_ANIMATIONS = 10;
	Animation* animations[MAX_ANIMATIONS];
	Skeleton result_skeleton;

	float time_animation;

	Player(sProp* _prop, float _health);

	void render(float time = 0);
	void renderGun();
	void selectSkeleton(float time = 0);
	void setSpeed(float value);
	void update(double seconds_elapsed);

	void onBulletCollision(Bullet* bullet);

	void pickGun(Entity* ent, sProp* prop);
	void changeGun();
	void pickHeart(Entity* ent, sProp* prop);
	void dropGun();
};

class Character : public EntityMesh
{
public:
	Vector3 position;
	Vector3 velocity;
	float speed;
	float health;
	bool isSaved;
	float* angle_player;

	enum eAnimCharacter { IDLE_C, WALK_C, RUN_C, WAVING_C };
	static const int MAX_ANIMATIONS = 4;
	Animation* animations[MAX_ANIMATIONS];
	Skeleton result_skeleton;

	Vector3* player_position;

	//float time_animation;

	Character(sProp* _prop, Vector3 _pos, float _health, Vector3* _player_position);
	void reset(Vector3 _pos, Vector3* _player_position);

	void render(float time = 0);
	void selectSkeleton(float time = 0);
	void update(double seconds_elapsed);
	void onBulletCollision(Bullet* bullet);
};

#endif