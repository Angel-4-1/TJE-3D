#include "stage.h"
#include "scene.h"
#include "shader.h"
#include "camera.h"
#include "game.h"
#include "input.h"
#include "gamemap.h"

Stage::Stage()
{
	change = false;
	stage_type = sType::STAGE;
}

void Stage::render(void) { }

void Stage::update(double seconds_elapsed) { }

sType Stage::whoAmI()
{
	return stage_type;
}

void Stage::onKeyDown(SDL_KeyboardEvent event) { }

void Stage::onKeyUp(SDL_KeyboardEvent event) { }

/********INTRO STAGE*******/
IntroStage::IntroStage() : Stage()
{
	stage_type = sType::INTRO_STAGE;
}

void IntroStage::render(void)
{
}

void IntroStage::update(double seconds_elapsed)
{
}

void IntroStage::onKeyDown(SDL_KeyboardEvent event)
{
}


/********EDITOR STAGE*******/
EditorStage::EditorStage() : Stage()
{
	stage_type = sType::EDITOR_STAGE;
	object = 1;
	selected = NULL;
}

void EditorStage::render(void)
{
}

void EditorStage::update(double seconds_elapsed)
{
}

//add a new object on the scene
void EditorStage::addObjectInFront()
{
	//get instances
	Scene* scene = Scene::getInstance();
	Camera* camera = Camera::getInstance();
	Game* game = Game::instance;
	GameMap* map = GameMap::getInstance();

	//get position and origin of the ray
	Vector3 pos = camera->center;
	pos.y = 0;
	Vector3 origin = camera->eye;
	
	//compute the direction of the ray
	Vector3 dir = camera->getRayDirection(Input::mouse_position.x, Input::mouse_position.y, game->window_width, game->window_height);
	
	//point of collision
	pos = RayPlaneCollision(Vector3(), Vector3(0, 1, 0), origin, dir);

	//create a new entitymesh on that position
	if (selected == NULL) {
		//choose the prototype according to the object variable
		sProp* prop = &map->prototypes[object];
		//if we arrive to an empty prototype, select the first of the list (not the player)
		if ((int)prop->index == (int)EMPTY)
			prop = &map->prototypes[2];

		//create the entity
		selected = new EntityMesh(prop);	
		selected->model.setTranslation(pos.x, pos.y, pos.z);

		//add the entity to the scene
		scene->root.addChild(selected);
	}
	//if we have one entity selected, translate it to the new position
	else {
		selected->model.setTranslation(pos.x, pos.y, pos.z);
	}
}

//change the scale of the entity model
void EditorStage::changeScaleSelected(bool value)
{
	if (selected != NULL)
	{
		float scale = selected->getScaleFactor();
		
		//value = true --> increase scale
		if (value) {
			scale += 0.5;
		}
		//value = false --> decrease scale
		else {
			scale -= 0.5;
		}

		//update scale
		selected->setScaleFactor(scale);
		selected->model.m[0] = scale;
		selected->model.m[5] = scale;
		selected->model.m[10] = scale;
	}
}

//to change the position of the selected entity
void EditorStage::changePositionSelected(char var, float value)
{
	if (selected == NULL)
		return;

	//get position and angle of the selected entity
	Vector3 pos = selected->getPosition();
	float angle_rad = selected->angle * DEG2RAD;

	switch (var)
	{
		case 'x':	//change in the direction of axis x
			pos.x += value;
			break;
		case 'z':	//change in the direction of axis z
			pos.z += value;
			break;
		default:
			break;
	}

	//update model with new values
	selected->model.setTranslation(pos.x, pos.y, pos.z);
	
	selected->model.m[0] = selected->scale;
	selected->model.m[5] = selected->scale;
	selected->model.m[10] = selected->scale;

	selected->model.rotate(selected->angle * DEG2RAD, Vector3(0, 1, 0));
}

//change the prototype object to the next on the list of prototypes saved on gamemap
void EditorStage::changePropSelected(int* prop)
{
	if (selected == NULL)
		return;

	sProp* prototype = &GameMap::getInstance()->prototypes[*prop];
	
	//not empty prototypes
	if ((int)prototype->index == (int)EMPTY) {
		prototype = &GameMap::getInstance()->prototypes[2];
		*prop = 2;
	}

	selected->prop = prototype;
}

void EditorStage::changeRotationSelected(char var)
{
	if (selected == NULL)
		return;

	//direction vector
	Vector3 dir;

	switch (var)
	{
		case 'x':
			dir = Vector3(1, 0, 0);
			break;
		case 'y':
			dir = Vector3(0, 1, 0);
			break;
		case 'z':
			dir = Vector3(0, 0, 1);
			break;
		default:
			break;
	}

	//update rotation
	selected->angle = selected->angle + angle == 360 ? 0 : selected->angle + angle;
	selected->model.rotate(angle * DEG2RAD, dir);
}

void EditorStage::selectEntity() {

	if (selected != NULL)
		return;

	//get instances
	Scene* scene = Scene::getInstance();
	Camera* camera = Camera::getInstance();
	Game* game = Game::instance;
	GameMap* map = GameMap::getInstance();

	//get position and origin of the ray
	Vector3 pos = camera->center;
	pos.y = 0;
	Vector3 origin = camera->eye;

	//compute the direction of the ray
	Vector3 dir = camera->getRayDirection(Input::mouse_position.x, Input::mouse_position.y, game->window_width, game->window_height);

	//get all entities of the scene
	std::vector<Entity*> world_enitities = scene->root.children;

	for (int i = 0; i < world_enitities.size(); i++)
	{
		Entity* ent = world_enitities[i];
		if (ent->type == PLAYER)
			continue;

		Mesh* mesh = map->prototypes[(int)ent->type].mesh;
		Vector3 collision;
		Vector3 collision_normal;

		//check if the ray collides with a mesh
		if (mesh->testRayCollision(ent->model, origin, dir, collision, collision_normal) == false)
			continue;

		selected = new EntityMesh(&map->prototypes[(int)ent->type]);
		selected->model.setTranslation(ent->getPosition().x, ent->getPosition().y, ent->getPosition().z);

		selected->angle = ent->angle;
		selected->scale = ent->scale;

		selected->model.m[0] = ent->scale;
		selected->model.m[5] = ent->scale;
		selected->model.m[10] = ent->scale;

		selected->model.rotate(ent->angle * DEG2RAD, Vector3(0, 1, 0));

		scene->root.children[i] = selected;
		break;
	}
}

void EditorStage::onKeyDown(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
		case SDLK_f: addObjectInFront(); break;
		case SDLK_o: 
			object += 1; //next prototype
			changePropSelected(&object);
			break;
		case SDLK_p:
			object -= 1; //previous prototype
			changePropSelected(&object);
			break;
		case SDLK_PLUS:	//increase scale
			changeScaleSelected(true);
			break;
		case SDLK_MINUS: //decrease scale
			changeScaleSelected(false);
			break;
		case SDLK_g:	//set the selected entity to place
			selected = NULL;
			break;
		case SDLK_l:	//change position of the selected entity
			changePositionSelected('x', 0.5);
			break;
		case SDLK_j:
			changePositionSelected('x', -0.5);
			break;
		case SDLK_i:
			changePositionSelected('z', -0.5);
			break;
		case SDLK_k:
			changePositionSelected('z', 0.5);
			break;
		case SDLK_y:	//rotate entity 90 degress in y axis
			changeRotationSelected('y');
			break;
		case SDLK_u:	//select a entity from the scene doing a ray collision
			selectEntity();
			break;
	}
}

/*********PLAY STAGE********/
PlayStage::PlayStage(Player* _player) : Stage()
{
	player = _player;
	stage_type = sType::PLAY_STAGE;
}

void PlayStage::render(void)
{
	//player->render();
}

void PlayStage::update(double seconds_elapsed)
{
	player->update(seconds_elapsed);
}

void PlayStage::onKeyDown(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
		case SDLK_z: 
			player->setSpeed(90); 
			break;
		default:
			break;
	}
}

void PlayStage::onKeyUp(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
	case SDLK_z:
		player->setSpeed(10);
		break;
	default:
		break;
	}
}

/********FINAL STAGE*******/
FinalStage::FinalStage() : Stage()
{
	isWin = true;
	stage_type = sType::WIN_STAGE;
}

void FinalStage::render(void)
{
	if (isWin) {
		renderWin();
	}
	else {
		renderGameOver();
	}
}

void FinalStage::renderWin(void)
{
}

void FinalStage::renderGameOver(void)
{
}

void FinalStage::onKeyDown(SDL_KeyboardEvent event)
{
}

void FinalStage::changeWin(bool state)
{
	isWin = state;
}
