#include "bulletmanager.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "gamemap.h"
#include "scene.h"
#include "enemy.h"
#include <algorithm>

BulletManager* BulletManager::instance = NULL;

BulletManager::BulletManager() { initBullets(); initHitMarks(); tolerance = LOW; }

BulletManager* BulletManager::getInstance()
{
	if (instance == NULL)
	{
		instance = new BulletManager();
	}

	return instance;
}

//initialize the array of bullets
void BulletManager::initBullets()
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		Bullet* bullet = &bullets[i];

		bullet->setBulletValues(Vector3(), Vector3(), 0, 0, eAuthor::EMPTY_BULLET, 0, 0, false);
	}
}

void BulletManager::render()
{
	/**Render the bullets using instancing**/
	
	vertices.clear();
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		Bullet* bullet = &bullets[i];

		//only bullets in use go to the vector
		if (bullet->isUsed == false)
			continue;

		Matrix44 m;
		m.setTranslation(bullet->position.x, bullet->position.y, bullet->position.z);
		m.rotate(bullet->angle * DEG2RAD, Vector3(0, 1, 0));
		vertices.push_back(m);
	}

	Shader* shader_instanced = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");
	Camera* camera = Camera::current;

	if (vertices.size() != 0) {
		//enable shader
		shader_instanced->enable();

		//upload uniforms
		shader_instanced->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader_instanced->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader_instanced->setUniform("u_texture", GameMap::getInstance()->prototypes[(int)BULLET].texture, 0);
		shader_instanced->setUniform("u_camera_pos", camera->eye);

		Mesh* mesh = GameMap::getInstance()->prototypes[(int)BULLET].mesh;
		mesh->renderInstanced(GL_TRIANGLES, &vertices[0], vertices.size());

		//disable shader
		shader_instanced->disable();
	}

	renderHitMarks();
}

void BulletManager::update(double seconds_elapsed)
{
	//move each bullet
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		Bullet* bullet = &bullets[i];

		if (bullet->isUsed == false)
			continue;
		
		//update position
		bullet->position = bullet->position + bullet->velocity * seconds_elapsed;
		
		//decrease ttl
		bullet->time_to_live -= seconds_elapsed;

		//remove bullets that exceeds its ttl
		if (bullet->time_to_live < 0.0)
		{
			//remove bullet
			bullet->setBulletValues(Vector3(), Vector3(), 0, 0, EMPTY_BULLET, 0, 0, false);
		}
	}

	//hit marks
	for (int i = 0; i < MAX_HITS; i++)
	{
		sHitMark& mark = marks[i];
		if (mark.ttl <= 0)
			continue;

		mark.ttl -= seconds_elapsed;
	}
}

//create a bullet on the next empty space of the bullets array
void BulletManager::createBullet(Vector3 _pos, Vector3 _vel, float _ttl, float _power, eAuthor _author, int _type, float _angle)
{
	for (int i = 0; i < MAX_BULLETS; i++)
	{
		Bullet* bullet = &bullets[i];

		if (bullet->isUsed)
			continue;
		
		//if not in use, set new values
		bullet->setBulletValues( _pos, _vel, _ttl, _power, _author, _type, _angle, true);

		break;
	}
}

void BulletManager::hasCollisioned()
{
	//std::vector<Entity*> world_enitities = Scene::getInstance()->root.children;
	GameMap* map = GameMap::getInstance();

	Entity* player = Scene::getInstance()->player;
	Mesh* mesh = map->prototypes[(int)player->type].mesh;
	EnemyManager* enemyMan = EnemyManager::getInstance();

	int radius_tolerance = (int)tolerance;

	for (int i = 0; i < MAX_BULLETS; i++)
	{
		Bullet* bullet = &bullets[i];
		if (!bullet->isUsed)
			break;

		if (bullet->author == eAuthor::PLAYER_BULLET)
			continue;

		Vector3 collision;	//point of collision
		Vector3 collision_normal;

		//test collision
		if (mesh->testSphereCollision(player->model, bullet->position, 3, collision, collision_normal) == false)
			continue;

		player->onBulletCollision(bullet);
		
	}

	int maximum = enemyMan->MAX_ENEMIES;
	for (int i = 0; i < maximum; i++)
	{
		sEnemy enemy = enemyMan->enemies[i];

		//only bullets in use go to the vector
		if (enemy.isActive == false || enemy.isDead == true)
			continue;

		Matrix44 m;
		m.setTranslation(enemy.position.x, enemy.position.y, enemy.position.z);
		m.rotate((enemy.angle+180) * DEG2RAD, Vector3(0, 1, 0));

		for (int j = 0; j < MAX_BULLETS; j++)
		{
			Bullet* bullet = &bullets[j];
			if (!bullet->isUsed)
				break;

			if (bullet->author == eAuthor::ENEMY_BULLET)
				continue;

			Vector3 collision;	//point of collision
			Vector3 collision_normal;

			//test collision
			if (mesh->testSphereCollision(m, bullet->position, radius_tolerance, collision, collision_normal) == false)
				continue;

			enemyMan->onBulletCollision(bullet, &enemy, collision);
			createHitMark(Vector3(bullet->position.x, bullet->position.y, bullet->position.z));
			enemyMan->enemies[i] = enemy;
			break;
		}
	}
}

//set values to the bullet
void Bullet::setBulletValues(Vector3 _pos, Vector3 _vel, float _ttl, float _power, eAuthor _author, int _type, float _angle, bool _isUsed)
{
	position = _pos;
	last_position = _pos;
	velocity = _vel;
	time_to_live = _ttl;
	power = _power;
	author = _author;
	type = _type;
	isUsed = _isUsed;
	angle = _angle;
}


/********HIT MARKS*******/
bool compare_distance_func(const sHitMark& a, const sHitMark& b)
{
	Camera* cam = Camera::current;
	float distA = a.pos.distance(cam->eye);
	float distB = b.pos.distance(cam->eye);
	return distA > distB;
}

void BulletManager::initHitMarks()
{
	marks.resize(100);
	for (int i = 0; i < MAX_HITS; i++)
	{
		sHitMark& mark = marks[i];

		mark.pos = Vector3();
		mark.size = 0;
		mark.ttl = 0;
	}
}

void BulletManager::createHitMark(Vector3 _pos)
{
	for (int i = 0; i < MAX_HITS; i++)
	{
		sHitMark& mark = marks[i];
		if (mark.ttl > 0)
			continue;

		mark.pos = _pos;
		mark.size = 0.5;
		mark.ttl = 0.1;
		break;
	}
}

void BulletManager::renderHitMarks()
{
	Mesh quad;
	Camera* camera = Camera::current;

	Vector3 right = camera->getLocalVector(Vector3(1, 0, 0));
	Vector3 top = camera->getLocalVector(Vector3(0, 1, 0));
	Vector3 bottom = camera->getLocalVector(Vector3(0, -1, 0));

	std::sort(marks.begin(), marks.end(), compare_distance_func);

	for (int i = 0; i < marks.size(); i++) {
		sHitMark& hm = marks[i];
		if (hm.ttl <= 0)
			continue;
		Vector3 pos = hm.pos;
		float size = hm.size;

		quad.vertices.push_back(pos + (top - right) * size);
		quad.uvs.push_back(Vector2(0, 1));
		quad.vertices.push_back(pos + (bottom - right) * size);
		quad.uvs.push_back(Vector2(0, 0));
		quad.vertices.push_back(pos + (bottom + right) * size);
		quad.uvs.push_back(Vector2(1, 0));

		quad.vertices.push_back(pos + (top - right) * size);
		quad.uvs.push_back(Vector2(0, 1));
		quad.vertices.push_back(pos + (bottom + right) * size);
		quad.uvs.push_back(Vector2(1, 0));
		quad.vertices.push_back(pos + (top + right) * size);
		quad.uvs.push_back(Vector2(1, 1));
	}

	if (quad.vertices.size() > 0) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//read from z buffer but not write on it
		glDepthMask(false);
		Matrix44 qm;
		//enable shader
		Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
		shader->enable();

		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_texture", Texture::Get("data/hit.tga"), 0);
		shader->setUniform("u_model", qm);

		shader->setUniform("u_camera_pos", camera->eye);
		quad.render(GL_TRIANGLES);
		glDisable(GL_BLEND);
		glDepthMask(true);
	}
}