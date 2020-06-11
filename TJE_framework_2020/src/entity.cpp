#include "entity.h"
#include "input.h"
#include "scene.h"
#include "gamemap.h"

#include "light.h"
#include "bulletmanager.h"
#include "game.h"

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
	//model.scale(scale, scale, scale);
}

void Entity::addChild(Entity* child)
{
	children.push_back(child);
	child->parent = this;
}

void Entity::removeChild(Entity* child)
{
	// remove childs of the child
	for (int i = 0; i < child->children.size(); i++)
	{
		child->removeChild(child->children[i]);
	}

	// remove the child
	child->type = EMPTY;	//set to empty to avoid problems, it won't be taken into account in the rest of the game
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

void Entity::onBulletCollision(Bullet* bullet)
{
}

/********ENTITY MESH*******/
EntityMesh::EntityMesh(sProp* _prop) : Entity(_prop->index)
{
	prop = _prop;
}

void EntityMesh::render()
{
	if (type == EMPTY)
		return;	

	Mesh* mesh = prop->mesh;
	Texture* texture = prop->texture;
	Shader* shader = prop->shader;

	if (!shader)
		return;

	Camera* camera = Camera::current;
	Matrix44 model_mat = getGlobalMatrix();

	Scene* scene = Scene::getInstance();

	//enable shader
	shader->enable();
	scene->light->uploadToShader(shader);

	//upload uniforms
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_texture", texture, 0);
	shader->setUniform("u_model", model_mat);
	shader->setUniform("u_material_shininess", 30.0f);

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

void EntityMesh::onBulletCollision(Bullet* bullet)
{
	bullet->time_to_live = 0;
	std::cout << "shot" << std::endl;
}


/*******PLAYER*******/
Player::Player(sProp* _prop, float _health) : EntityMesh(_prop) 
{ 	
	position = Vector3(-330, 0 , 420);
	model.setTranslation(position.x, position.y, position.z);
	model.rotate(180 * DEG2RAD, Vector3(0, 1, 0));
	speed = 50;
	hasGun = false;
	angle = 0;
	time_animation = 0;
	isHit = false;
	when_was_hitted = 0;
	health = _health;

	gun = NULL;

	for (int i = 0; i < MAX_ANIMATIONS; i++) {
		animations[i] = NULL;
	}

	animations[STAND_UP] = Animation::Get("data/characters/animation/standing_up.skanim");
	animations[IDLE] = Animation::Get("data/characters/animation/idle.skanim");
	animations[WALK] = Animation::Get("data/characters/animation/walking.skanim");
	animations[RUN] = Animation::Get("data/characters/animation/running.skanim");
	animations[SHOOT_NOT_MOVING] = Animation::Get("data/characters/animation/gunplay.skanim");	//angle = -45
	animations[SHOOT_MOVING] = Animation::Get("data/characters/animation/shoot_moving.skanim"); //angle = -90
	animations[HIT] = Animation::Get("data/characters/animation/hit_reaction.skanim");
	animations[DEAD] = Animation::Get("data/characters/animation/dyingfront.skanim");
}

void Player::selectSkeleton(float time)
{
	Vector3 front = model.rotateVector(Vector3(0, 0, -1));
	//float FdotV = front.dot(velocity);
	//time_animation += FdotV;

	Animation* idle = animations[IDLE];
	idle->assignTime(time);

	Animation* walk = animations[WALK];
	//exact instance of time where are in the animation normalized
	float t = fmod(time, walk->duration) / walk->duration;
	walk->assignTime(t);

	Animation* shoot = animations[SHOOT_NOT_MOVING];
	//exact instance of time where are in the animation normalized
	float t2 = fmod(time, shoot->duration) / shoot->duration;
	shoot->assignTime(time);

	Animation* shoot_moving = animations[SHOOT_MOVING];
	//exact instance of time where are in the animation normalized
	float t3 = fmod(time, shoot_moving->duration) / shoot_moving->duration;
	shoot_moving->assignTime(time);

	Animation* run = animations[RUN];
	//now both animations are on the same cycle
	run->assignTime(t * run->duration);

	float speed = velocity.length() * 0.1;

	if (isHit) {
		Animation* hit_reaction = animations[HIT];
		float duration = hit_reaction->duration;
		float time_left = Game::instance->time - when_was_hitted;
		hit_reaction->assignTime(Game::instance->time);
		if (time_left <= duration) {
			result_skeleton = hit_reaction->skeleton;
		}
		else {
			isHit = false;
			//blendSkeleton(&hit_reaction->skeleton, &shoot->skeleton, speed, &result_skeleton);
		}
		
	}

	//hasGun = true;
	if (hasGun) {
		if (speed < 1) {
			blendSkeleton(&idle->skeleton, &shoot->skeleton, speed, &result_skeleton);
		}
		else {
			blendSkeleton(&shoot->skeleton, &shoot_moving->skeleton, speed - 1, &result_skeleton);
		}
	}
	else if ( isHit == false ){
		if (speed < 1) {
			blendSkeleton(&idle->skeleton, &walk->skeleton, speed, &result_skeleton);
		}
		else {
			blendSkeleton(&walk->skeleton, &run->skeleton, speed - 1, &result_skeleton);
		}
	}

	if (health <= 0.0) {
		Animation* dead = animations[DEAD];
		dead->assignTime(time, false);
		result_skeleton = dead->skeleton;
	}
}

void Player::render(float time)
{
	Mesh* mesh = prop->mesh;
	Texture* texture = prop->texture;
	Shader* shader = prop->shader;

	if (!shader)
		return;

	Camera* camera = Camera::current;
	Matrix44 model_mat = getGlobalMatrix();

	Scene* scene = Scene::getInstance();
	
	//enable shader
	shader->enable();
	scene->light->uploadToShader(shader);

	//upload uniforms
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_texture", texture, 0);
	shader->setUniform("u_model", model_mat);
	shader->setUniform("u_material_shininess", 30.0f);

	shader->setUniform("u_camera_pos", camera->eye);
	shader->setUniform("u_light_direction", Vector3(0.3, 0.6, 0.2).normalize());

	//check if it is on the camera frustum
	Vector3 pos = model_mat * mesh->box.center;
	if (camera->testSphereInFrustum(pos, 5 * scale)) {
		selectSkeleton(time);
		mesh->renderAnimated(GL_TRIANGLES, &result_skeleton);

		if (hasGun) {
			renderGun();
		}
	}

	//disable shader
	shader->disable();
}

void Player::renderGun()
{
	Matrix44 right_hand = result_skeleton.getBoneMatrix("mixamorig_RightHandIndex1", false);
	right_hand = right_hand * model;
	gun->model = right_hand;
	gun->model.rotate(90 * DEG2RAD, Vector3(0, 0, 1));
	gun->angle = angle;
	gun->model.rotate(-90 * DEG2RAD, Vector3(0, 1, 0));
	gun->render();
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

	Vector3 delta;

	//Target position
	Vector3 target = position;

	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed_walk *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_RSHIFT)) speed_walk /= 10; //move slower with right shift

	if (Input::isKeyPressed(SDL_SCANCODE_W))  delta = delta + front * speed;
	if (Input::isKeyPressed(SDL_SCANCODE_S))  delta = delta - front * (speed * 0.5);

	if (Input::isKeyPressed(SDL_SCANCODE_A)) { delta = delta - right; angle -= speed_turn; } //in degrees
	if (Input::isKeyPressed(SDL_SCANCODE_D)) { delta = delta + right; angle += speed_turn; }

	velocity = velocity + delta * seconds_elapsed;
	velocity = velocity - velocity * 2.8 * seconds_elapsed;
	target = position + velocity * seconds_elapsed;

	/**IS VALID POSITION?**/
	//get the center of the player
	Vector3 center = target + Vector3(0, 1, 0);

	GameMap* map = GameMap::getInstance();
	std::vector<Entity*> world_enitities = Scene::getInstance()->root.children;

	bool collided = false;

	//test with the entities of the scene
	for (int i = 0; i < world_enitities.size(); i++) 
	{
		Entity* ent = world_enitities[i];
		if (ent->type == PLAYER || ent->type == EMPTY)
			continue;

		Mesh* mesh = map->prototypes[(int)ent->type].mesh;

		Vector3 collision;	//point of collision
		Vector3 collision_normal;

		//test collision
		if (mesh->testSphereCollision(ent->model, center, 1 / 3.0, collision, collision_normal) == false) {
			continue;
		}

		if (ent->type == REVOLVER || ent->type == SHOTGUN || ent->type == MICROGUN) {
			pickGun(ent, &map->prototypes[(int)ent->type]);
			break;
		}

		Vector3 push_away = normalize(collision - center) * seconds_elapsed;
		target = position - push_away;
		target.y = 0;
		collided = true;
		break;
	}

	//test with the trees
	if (collided == false) {
		std::vector<Matrix44> world_trees = Scene::getInstance()->tree->vertices;
		eType tree_type = Scene::getInstance()->tree->getType();
		Mesh* mesh = GameMap::getInstance()->prototypes[(int)tree_type].mesh;
		
		for (int i = 0; i < world_trees.size(); i++) {
			Matrix44 tree_model = world_trees[i];

			Vector3 collision;	//point of collision
			Vector3 collision_normal;

			//test collision
			if (mesh->testSphereCollision(tree_model, center, 1 / 3.0, collision, collision_normal) == false) {
				continue;
			}

			Vector3 push_away = normalize(collision - center) * seconds_elapsed;
			target = position - push_away;
			target.y = 0;
			collided = true;
			break;
		}
	}

	//update player position
	position = target;

	model.setTranslation( position.x, position.y, position.z );
	model.rotate( (angle+180) * DEG2RAD, Vector3(0, 1, 0) );
}

void Player::onBulletCollision(Bullet* bullet)
{
	bullet->time_to_live = 0;
	isHit = true;
	health -= bullet->power;
	when_was_hitted = Game::instance->time;
}

void Player::pickGun(Entity* ent, sProp* prop)
{
		hasGun = true;
		ent->type = EMPTY;
		gun = new Gun(ent->getPosition(), 100, prop);
}

void Player::dropGun()
{
	hasGun = false;
}

Gun::Gun(Vector3 _pos, int _bullets, sProp* _prop) : EntityMesh(_prop)
{
	position = _pos;
	position.y = 1;
	num_bullets = _bullets;
	//scope = _scope;
	//damage = _damage;

	switch (_prop->index)
	{
		case REVOLVER:
			scope = 0.5;
			damage = 15;
			break;
		case SHOTGUN:
			scope = 0.25;
			damage = 30;
			break;
		case MICROGUN:
			scope = 2;
			damage = 5;
			break;
		default:
			scope = 0;
			damage = 0;
			break;
	}
}

/********CHARACTER**********/
Character::Character(sProp* _prop, Vector3 _pos, float _health, Vector3* _player_position) : EntityMesh(_prop)
{
	position = _pos;
	health = _health;
	velocity = Vector3();
	player_position = _player_position;
	speed = 50;
	model.setTranslation(position.x, position.y, position.z);
	angle = 0;
	isSaved = false;
	angle_player = NULL;

	for (int i = 0; i < MAX_ANIMATIONS; i++) {
		animations[i] = NULL;
	}

	animations[IDLE_C] = Animation::Get("data/characters/animation/idle.skanim");
	animations[WALK_C] = Animation::Get("data/characters/animation/walking.skanim");
	animations[RUN_C] = Animation::Get("data/characters/animation/running.skanim");
	animations[WAVING_C] = Animation::Get("data/characters/animation/waving.skanim");
}

void Character::render(float time)
{
	Mesh* mesh = prop->mesh;
	Texture* texture = prop->texture;
	Shader* shader = prop->shader;

	if (!shader)
		return;

	Camera* camera = Camera::current;
	Matrix44 model_mat = getGlobalMatrix();

	Scene* scene = Scene::getInstance();

	//enable shader
	shader->enable();
	scene->light->uploadToShader(shader);

	//upload uniforms
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_texture", texture, 0);
	shader->setUniform("u_model", model_mat);
	shader->setUniform("u_material_shininess", 30.0f);

	shader->setUniform("u_camera_pos", camera->eye);
	shader->setUniform("u_light_direction", Vector3(0.3, 0.6, 0.2).normalize());

	//check if it is on the camera frustum
	Vector3 pos = model_mat * mesh->box.center;
	if (camera->testSphereInFrustum(pos, 6 * scale)) {
		selectSkeleton(time);
		mesh->renderAnimated(GL_TRIANGLES, &result_skeleton);
	}

	//disable shader
	shader->disable();
}

void Character::selectSkeleton(float time)
{
	if (isSaved == false) {
		Animation* waving = animations[WAVING_C];
		waving->assignTime(time);

		result_skeleton = waving->skeleton;
	}
	else {
		Vector3 front = model.rotateVector(Vector3(0, 0, -1));
		float FdotV = front.dot(velocity);

		Animation* idle = animations[IDLE_C];
		idle->assignTime(time);

		Animation* walk = animations[WALK_C];
		float t = fmod(time, walk->duration) / walk->duration;
		walk->assignTime(t);

		Animation* run = animations[RUN_C];
		//now both animations are on the same cycle
		run->assignTime(t * run->duration);

		float speed = velocity.length() * 0.1;

		if (speed < 1) {
			blendSkeleton(&idle->skeleton, &walk->skeleton, speed, &result_skeleton);
		}
		else {
			blendSkeleton(&walk->skeleton, &run->skeleton, speed - 1, &result_skeleton);
		}
	}
}

void Character::update(double seconds_elapsed)
{
	if (isSaved == false) {
		return;
	}

	float distanceWithPlayer = position.distance(*player_position);
	Vector3 to_target = normalize(*player_position - position);

	if (distanceWithPlayer <= 300.0 && distanceWithPlayer >= 3) {
		Matrix44 R;
		R.setRotation(angle * DEG2RAD, Vector3(0, 1, 0));	//yaw , rotation in degrees
		Vector3 front = R.rotateVector(Vector3(0, 0, -1));
		Vector3 delta = delta + front;// *speed;
		velocity = to_target * speed * 12 * seconds_elapsed;
		float FdotT = clamp(front.dot(normalize(velocity)), -1, 1);
		float angleFT = acos(FdotT) * RAD2DEG;
		/*
		if (angleFT >= 10 || angleFT <= -10) {
			angle = angleFT;// *seconds_elapsed;
			angle = angle >= 360 ? 0 : angle;
			std::cout << angle << std::endl;
		}*/
		//if(Fdot )
		angle = *angle_player + 180;
	}

	velocity = velocity - velocity * 2.8 * seconds_elapsed;
	position = position + velocity * seconds_elapsed;

	model.setTranslation(position.x + 3, position.y, position.z + 3);
	model.rotate(angle * DEG2RAD, Vector3(0, 1, 0));
}

void Character::onBulletCollision(Bullet* bullet)
{
}
