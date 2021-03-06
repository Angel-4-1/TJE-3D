#include "enemy.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "gamemap.h"
#include "scene.h"
#include "bulletmanager.h"
#include "game.h"
#include "audio.h"

EnemyManager* EnemyManager::instance = NULL;

EnemyManager::EnemyManager() { 
	initEnemies(); 

	current_difficulty = EASY_ENEMY;

	for (int i = 0; i < MAX_ANIMATIONS; i++) {
		animations[i] = NULL;
	}

	animations[IDLE_E] = Animation::Get("data/characters/animation/idle.skanim");
	animations[SHOOT_E] = Animation::Get("data/characters/animation/gunplay.skanim");
	animations[SHOOT_MOVING_E] = Animation::Get("data/characters/animation/shoot_moving.skanim");
	animations[DEAD_E] = Animation::Get("data/characters/animation/dyingfront.skanim");
}

EnemyManager* EnemyManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new EnemyManager();
	}

	return instance;
}

//initialize the array of enemies
void EnemyManager::initEnemies()
{
	GameMap* map = GameMap::getInstance();
	sProp* prop = &map->prototypes[(int)ENEMY];

	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		sEnemy* enemy = &enemies[i];

		enemy->setEnemyValues(Vector3(), Vector3(), NULL, 0, 0, 0, 0, false, prop);
	}
}

void EnemyManager::render(float time)
{
	Scene* scene = Scene::getInstance();
	Camera* camera = Camera::current;

	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		sEnemy* enemy = &enemies[i];

		if (enemy->isActive == false)
			continue;

		Matrix44 m;
		m.setTranslation(enemy->position.x, enemy->position.y, enemy->position.z);
		m.rotate((enemy->angle+180) * DEG2RAD, Vector3(0, 1, 0));

		Mesh* mesh = enemy->prop->mesh;
		Texture* texture = enemy->prop->texture;
		Shader* shader = enemy->prop->shader;

		if (!shader)
			return;

		//enable shader
		shader->enable();
		scene->light->uploadToShader(shader);

		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_texture", texture, 0);
		shader->setUniform("u_model", m);
		shader->setUniform("u_material_shininess", 30.0f);

		shader->setUniform("u_camera_pos", camera->eye);
		shader->setUniform("u_light_direction", Vector3(0.3, 0.6, 0.2).normalize());

		//check if it is on the camera frustum
		Vector3 pos = m * mesh->box.center;
		if (camera->testSphereInFrustum(pos, 5)) {
			selectSkeleton(enemy, time);
			mesh->renderAnimated(GL_TRIANGLES, &enemy->result_skeleton);
		}

		//disable shader
		shader->disable();
	}
}

//render enemies in a simplified version using instancing
void EnemyManager::renderSimplified(Camera* camera, float _scale)
{
	vertices.clear();
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		sEnemy* enemy = &enemies[i];

		//only active enemies and not dead
		if (enemy->isActive == false || enemy->isDead == true)
			continue;

		Matrix44 m;
		m.setTranslation(enemy->position.x, enemy->position.y, enemy->position.z);
		m.scale(_scale, _scale, _scale);
		vertices.push_back(m);
	}
		
	if (vertices.size() != 0) {
		Mesh* sphere = Mesh::Get("data/sphere.obj");

		Shader* shader_instanced = Shader::Get("data/shaders/instanced.vs", "data/shaders/flat.fs");
		//enable shader
		shader_instanced->enable();

		//upload uniforms
		shader_instanced->setUniform("u_color", Vector4(1, 0, 0, 1));
		shader_instanced->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader_instanced->setUniform("u_model", Matrix44());

		shader_instanced->setUniform("u_camera_pos", camera->eye);

		sphere->renderInstanced(GL_TRIANGLES, &vertices[0], vertices.size());

		//disable shader
		shader_instanced->disable();
	}
}

void EnemyManager::update(double seconds_elapsed)
{
	BulletManager* bm = BulletManager::getInstance();
	float time = Game::instance->time;

	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		sEnemy* enemy = &enemies[i];

		if (enemy->isActive == false)
			continue;

		if (enemy->isDead == false) {
			float distanceWithPlayer = enemy->position.distance(*enemy->player_position);
			Vector3 to_target = normalize(*enemy->player_position - enemy->position);

			if (distanceWithPlayer <= 50.0 && distanceWithPlayer >= 5) {
				Vector3 pos = enemy->position;
				pos.z = pos.z - 1;
				pos.y = 2;

				Matrix44 R;
				R.setRotation(enemy->angle * DEG2RAD, Vector3(0, 1, 0));	//yaw , rotation in degrees
				Vector3 front = R.rotateVector(Vector3(0, 0, -1));

				front = front.normalize();
				float FdotT = clamp(front.dot(to_target), -1, 1);
				if (FdotT > 0.5) {
					float angle = acos(FdotT) * RAD2DEG;
					//if (abs(enemy->angle - angle) >= 5) { enemy->angle = angle; }	//to not change a lot the enemy angle
					if (time - enemy->previous_time_shot >= 0.5) {
						enemy->previous_time_shot = time;
						bm->createBullet(pos, to_target * 7500 * seconds_elapsed, 10.0, enemy->bullet_damage, eAuthor::ENEMY_BULLET, 1, angle);
						Audio* shooting = Audio::Get("data/audio/microgun_shot.wav");
						shooting->playSound(0.1);
					}
					//update position
					enemy->velocity = to_target * 5;
					enemy->position = enemy->position + enemy->velocity * seconds_elapsed;
				}
			}
			else if (distanceWithPlayer <= 80.0) {
				enemy->velocity = enemy->velocity * 0.9;
				enemy->position = enemy->position + enemy->velocity * seconds_elapsed;
			}
			else {
				/*****LOOK FOR THE PLAYER*****
				GameMap* map = GameMap::getInstance();
				std::vector<Entity*> world_enitities = Scene::getInstance()->root.children;
				//test with the entities of the scene
				bool collided = false;
				Vector3 center = enemy->position + Vector3(0, 1, 0);

				for (int i = 0; i < world_enitities.size(); i++)
				{
					Entity* ent = world_enitities[i];
					if (ent->type == EMPTY)
						continue;

					Mesh* mesh = map->prototypes[(int)ent->type].mesh;

					Vector3 collision;	//point of collision
					Vector3 collision_normal;

					//test collision
					if (mesh->testSphereCollision(ent->model, center, 1 / 3.0, collision, collision_normal) == false) {
						continue;
					}

					collided = true;
					enemy->velocity = enemy->velocity * Vector3(random(5), 0, random(5));
					break;
				}

				enemy->position = enemy->position + enemy->velocity * seconds_elapsed;
				*/
			}
			

			if (enemy->health <= 0.0)
			{
				//the body will remain in the scene it will not disappear
				enemy->isDead = true;
			}
		}
	}
}

//create an enemy on the next empty space of the enemies array
void EnemyManager::createEnemy(Vector3 _pos, Vector3 _vel, Vector3* _player_pos, float _speed, float _life, float _bullet_damage, float _angle)
{
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		sEnemy* enemy = &enemies[i];

		if (enemy->isActive != false)
			continue;
		
		//if not in use, set new values
		enemy->setEnemyValues( _pos, _vel, _player_pos, _speed, _life, _bullet_damage, _angle, true);

		break;
	}
}

void sEnemy::setEnemyValues(Vector3 _pos, Vector3 _vel, Vector3* _player_pos, float _speed, float _health, float _bullet_damage, float _angle, bool _isActive, sProp* _prop)
{
	position = _pos;
	initial_position = _pos;
	velocity = _vel;
	player_position = _player_pos;
	speed = _speed;
	health = _health;
	bullet_damage = _bullet_damage;
	angle = _angle;
	initial_angle = _angle;
	isActive = _isActive;
	prop = _prop;
	isDead = false;
	previous_time_shot = 0;
}

void sEnemy::setEnemyValues(Vector3 _pos, Vector3 _vel, Vector3* _player_pos, float _speed, float _health, float _bullet_damage, float _angle, bool _isActive)
{
	position = _pos;
	initial_position = _pos;
	velocity = _vel;
	player_position = _player_pos;
	speed = _speed;
	health = _health;
	bullet_damage = _bullet_damage;
	angle = _angle;
	initial_angle = _angle;
	isActive = _isActive;
	isDead = false;
	previous_time_shot = 0;
}

void EnemyManager::selectSkeleton(sEnemy* enemy, float time)
{
	Animation* idle = animations[IDLE_E];
	idle->assignTime(time);

	Animation* shoot = animations[SHOOT_E];
	float t_shoot = fmod(time, shoot->duration) / shoot->duration;
	shoot->assignTime(t_shoot);

	Animation* shoot_moving = animations[SHOOT_MOVING_E];
	float t_shoot_moving = fmod(time, shoot_moving->duration) / shoot_moving->duration;
	shoot_moving->assignTime(t_shoot_moving);

	//enemy->result_skeleton = idle->skeleton;

	float speed = enemy->velocity.length() * 0.1;

	if (enemy->isDead == false) {
		if (speed < 0.5) {
			//blendSkeleton(&idle->skeleton, &shoot->skeleton, speed, &enemy->result_skeleton);
			enemy->result_skeleton = idle->skeleton;
		}
		else {
			//blendSkeleton(&idle->skeleton, &shoot_moving->skeleton, speed - 1, &enemy->result_skeleton);
			enemy->result_skeleton = shoot_moving->skeleton;
		}
	}
	else {
		Animation* dead = animations[DEAD_E];
		//float t = fmod(time, dead->duration) / dead->duration;
		dead->assignTime(time, false);
		enemy->result_skeleton = dead->skeleton;
		//blendSkeleton(&idle->skeleton, &dead->skeleton, speed, &enemy->result_skeleton);
	}
}

void EnemyManager::onBulletCollision(Bullet* bullet, sEnemy* enemy, Vector3 point_collision)
{
	bullet->time_to_live = 0;

	enemy->health -= bullet->power;

	//bullet impact afect a bit the position of the enemy
	enemy->position = enemy->position + bullet->velocity * 0.01;
}

void EnemyManager::resetEnemies()
{
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		sEnemy* enemy = &enemies[i];

		if (enemy->isActive == false)
			continue;

		enemy->setEnemyValues(enemy->initial_position, Vector3(0, 0, 0), NULL, enemy->speed, 50, 10, enemy->initial_angle, true);
	}
}

void EnemyManager::setDifficulty(eDifficultyEnemy diff, Vector3* _player_pos)
{
	//set values depending on the difficulty
	float _health = 10;
	float _bullet_damage = 10;

	//if diff = DEFAULT mantain difficulty otherwise update it
	if (diff != DEFAULT_ENEMY) { current_difficulty = diff; }

	//set values according to the difficulty
	switch (current_difficulty)
	{
		case EASY_ENEMY:
			_health = 50;
			_bullet_damage = 5;
			break;
		case MEDIUM_ENEMY:
			_health = 75;
			_bullet_damage = 10;
			break;
		case HARD_ENEMY:
			_health = 75;
			_bullet_damage = 15;
			break;
		default:
			break;
	}

	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		sEnemy* enemy = &enemies[i];

		if (enemy->isActive == false)
			continue;

		enemy->player_position = _player_pos;
		enemy->health = _health;
		enemy->bullet_damage = _bullet_damage;
	}
}
