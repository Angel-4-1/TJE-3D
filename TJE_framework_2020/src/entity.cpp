#include "entity.h"
#include "input.h"
#include "scene.h"
#include "gamemap.h"

/**********ENTITY*********/
Entity::Entity()
{
	parent = NULL;
	type = EMPTY;
	scale = 1.0;
	angle = 0.0;
}

Entity::Entity(eType _type)
{
	parent = NULL;
	type = _type;
	scale = 1.0;
	angle = 0.0;
}

Entity::~Entity()
{
}

void Entity::render()
{
	for (int i = 0; i < children.size(); i++)
		children[i]->render();
}

void Entity::update(double seconds_elapsed)
{
}

Vector3 Entity::getPosition()
{
	return model.getTranslation();
}

float Entity::getScaleFactor()
{
	return scale;
}

void Entity::setScaleFactor(float _scale)
{
	scale = _scale;
	model.scale(scale, scale, scale);
}

void Entity::addChild(Entity* child)
{
	children.push_back(child);
	child->parent = this;
}

void Entity::removeChild(Entity* child)
{
	// remove childs of the child
	/*
	for (int i = 0; i < child->children.size(); i++)
	{
		child->removeChild(child->children[i]);
	}*/

	// remove the child
	//children.pop_back();
	///delete child;
	/*
	for (auto it = children.begin(); it != children.end(); it++) {
		if ((*it)->children.size()) {
			(*it)->removeChild();
		}
		else {
			delete(*it);
		}
	}

	children.clear();
	*/
	/*
	auto it = std::find(children.begin(), children.end(), child);
	if (it == children.end())
		return;
	
	delete(*it);
	*/
}

Matrix44 Entity::getGlobalMatrix()
{
	// Concatenate with parent model
	if (parent != NULL)
	{
		return model * parent->getGlobalMatrix();
	}

	return model;
}

/********ENTITY MESH*******/
EntityMesh::EntityMesh(sProp* _prop) : Entity(_prop->index)
{
	prop = _prop;
}

void EntityMesh::render()
{
	Mesh* mesh = prop->mesh;
	Texture* texture = prop->texture;
	Shader* shader = prop->shader;

	if (!shader)
		return;

	Camera* camera = Camera::current;
	Matrix44 model_mat = getGlobalMatrix();

	//enable shader
	shader->enable();

	//upload uniforms
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_texture", texture, 0);
	shader->setUniform("u_model", model_mat);

	shader->setUniform("u_camera_pos", camera->eye);
	shader->setUniform("u_light_direction", Vector3(0.3, 0.6, 0.2).normalize());

	//check if it is on the camera frustum
	Vector3 pos = model_mat * mesh->box.center;
	if(camera->testSphereInFrustum(pos, mesh->radius * scale))
		mesh->render(GL_TRIANGLES);

	//render childs
	for (int i = 0; i < children.size(); i++)
	{
		children[i]->render();
	}

	//disable shader
	shader->disable();
}

void EntityMesh::update(double seconds_elapsed)
{
}


/*******PLAYER*******/
Player::Player(sProp* _prop) : EntityMesh(_prop) 
{ 	
	position = Vector3(-300, 0 , 400);
	model.setTranslation(position.x, position.y, position.z);
	speed = 10;
	hasItem = false;
}

void Player::setSpeed(float value)
{
	speed = value;
}

void Player::update(double seconds_elapsed)
{
	float speed_walk = seconds_elapsed * speed;
	float speed_turn = seconds_elapsed * 90;

	Matrix44 R;
	R.setRotation(angle * DEG2RAD, Vector3(0, 1, 0));	//yaw , rotation in degrees
	Vector3 front = R.rotateVector(Vector3(0, 0, -1));
	Vector3 right = R.rotateVector(Vector3(1, 0, 0));

	//Target position
	Vector3 target = position;

	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed_walk *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_RSHIFT)) speed_walk /= 10; //move slower with right shift

	if (Input::isKeyPressed(SDL_SCANCODE_W))  target = target + front * speed_walk;
	if (Input::isKeyPressed(SDL_SCANCODE_S))  target = target - front * speed_walk;

	if (Input::isKeyPressed(SDL_SCANCODE_A))  angle -= speed_turn; //in degrees
	if (Input::isKeyPressed(SDL_SCANCODE_D))  angle += speed_turn;
	
	/**IS VALID POSITION?**/
	//get the center of the player
	Vector3 center = target + Vector3(0, 1, 0);

	GameMap* map = GameMap::getInstance();
	std::vector<Entity*> world_enitities = Scene::getInstance()->root.children;

	//test with the entities of the scene
	for (int i = 0; i < world_enitities.size(); i++) 
	{
		Entity* ent = world_enitities[i];
		if (ent->type == PLAYER)
			continue;

		Mesh* mesh = map->prototypes[(int)ent->type].mesh;

		Vector3 collision;	//point of collision
		Vector3 collision_normal;

		//test collision
		if (mesh->testSphereCollision(ent->model, center, 1/3.0, collision, collision_normal) == false)
			continue;

		/******/
		if (ent->type == CACTUS)
			hasItem = true;
		/******/

		Vector3 push_away = normalize(collision - center) * seconds_elapsed;
		target = position - push_away;
		target.y = 0;
		break;
	}

	//update player position
	position = target;

	model.setTranslation( position.x, position.y, position.z );
	model.rotate( angle * DEG2RAD, Vector3(0, 1, 0) );
}
