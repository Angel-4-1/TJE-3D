#ifndef ENTITY_H
#define ENTITY_H

#include "framework.h"
#include <string>
#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"

enum eType { EMPTY, PLAYER, PLANE, TORPEDO, BRIDGE, HOUSE, TREE, WESTHOUSE, WESTHOUSE2 , TREE3, CACTUS, 
	TIENDACAMP, TIENDACAMP2, ORANGEGRASS, TREE4, FIRE, MOUNTAIN };

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
};

// Object of the scene with visual representation
class EntityMesh : public Entity
{
public:
	//EntityMesh(eType _type, Texture* _texture, Mesh* _mesh, Shader* _shader);
	
	//Mesh* mesh;
	//Texture* texture;
	//Shader* shader;
	
	EntityMesh(sProp* _prop);
	sProp* prop;

	void render();
	void update(double seconds_elapsed);
};

class Player : public EntityMesh
{
public:
	Vector3 position;
	float speed;
	bool hasItem;
	
	Player(sProp* _prop);

	void setSpeed(float value);
	void update(double seconds_elapsed);
};

#endif