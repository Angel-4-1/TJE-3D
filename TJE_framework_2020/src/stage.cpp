#include "stage.h"
#include "scene.h"
#include "shader.h"
#include "camera.h"
#include "game.h"
#include "input.h"
#include "gamemap.h"
#include "bulletmanager.h"
#include "enemy.h"

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

void Stage::renderGUI(float x, float y, float sizex, float sizey, Vector4 range, Texture* texture, Shader* shader)
{
	Mesh quad;
	quad.createQuad(x, y, sizex, sizey, false);

	Camera cam2D;
	cam2D.setOrthographic(0, Game::instance->window_width, Game::instance->window_height, 0, -1, 1);

	shader->enable();
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_texture", texture, 0);
	shader->setUniform("u_tex_range", range);
	shader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);
	shader->setUniform("u_model", Matrix44());
	quad.render(GL_TRIANGLES);
}

bool Stage::renderButton(float x, float y, float sizex, float sizey, Vector4 range, Texture* texture, Shader* shader, Vector4 color_hover, Vector4 color_unclicked, bool active)
{
	Mesh quad;
	quad.createQuad(x, y, sizex, sizey, false);

	Camera cam2D;
	cam2D.setOrthographic(0, Game::instance->window_width, Game::instance->window_height, 0, -1, 1);

	Vector2 mouse = Input::mouse_position;

	bool hover = (mouse.x > (x - sizex * 0.5) && mouse.x < (x + sizex * 0.5)) && (mouse.y > (y - sizey * 0.5) && mouse.y < (y + sizey * 0.5));
	if (active) {
		hover = true;
	}

	shader->enable();
	shader->setUniform("u_color", hover ? color_hover : color_unclicked);
	shader->setUniform("u_texture", texture, 0);
	shader->setUniform("u_tex_range", range);
	shader->setUniform("u_viewprojection", cam2D.viewprojection_matrix);
	shader->setUniform("u_model", Matrix44());
	quad.render(GL_TRIANGLES);

	mouse = Input::last_click_position;
	bool clicked = (mouse.x > (x - sizex * 0.5) && mouse.x < (x + sizex * 0.5)) && (mouse.y > (y - sizey * 0.5) && mouse.y < (y + sizey * 0.5));

	if (clicked)
		Input::last_click_position.set(-100, -100);

	return clicked;
}

void Stage::onKeyDown(SDL_KeyboardEvent event) { }

void Stage::onKeyUp(SDL_KeyboardEvent event) { }

/********INTRO STAGE*******/
IntroStage::IntroStage() : Stage()
{
	stage_type = sType::INTRO_STAGE;
	selected = NONE_INTRO;
	texture_atlas = Texture::Get("data/atlasIntro.png");
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texturegui.fs");
	//shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
}

void IntroStage::render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	Camera* camera = Camera::current;
	camera->enable();

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	drawGrid();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	float window_height = Game::instance->window_height;
	float window_width = Game::instance->window_width;
	float window_centerx = window_width / 2.0;

	float aspect = window_width / window_height;

	renderGUI(window_centerx, 125 * aspect, 300 * aspect, 200 * aspect, menu_atlas[(int)TITLE_INTRO], texture_atlas, shader);

	Vector4 color_hover(1, 1, 1, 1);
	Vector4 color_transparent(1, 1, 1, 0.6);

	if (renderButton(window_centerx, 240 * aspect, 150 * aspect, 50 * aspect, menu_atlas[(int)START_INTRO], texture_atlas, shader, color_hover, color_transparent, selected == START_INTRO) ) {
		selected = START_INTRO;
		change = true;
		change_to = PLAY_STAGE;
	}
	if (renderButton(window_centerx, 300 * aspect, 150 * aspect, 50 * aspect, menu_atlas[(int)EDITOR_INTRO], texture_atlas, shader, color_hover, color_transparent, selected == EDITOR_INTRO)) {
		selected = EDITOR_INTRO;
		change = true;
		change_to = EDITOR_STAGE;
	}
	if (renderButton(window_centerx, 360 * aspect, 150 * aspect, 50 * aspect, menu_atlas[(int)EXIT_INTRO], texture_atlas, shader, color_hover, color_transparent, selected == EXIT_INTRO) ) {
		selected = EXIT_INTRO;
		change = true;
		change_to = EXIT_STAGE;
	}
}

void IntroStage::update(double seconds_elapsed)
{
}

void IntroStage::onKeyDown(SDL_KeyboardEvent event)
{
}


/********EDITOR STAGE*******/
EditorStage::EditorStage(Mesh* _plane) : Stage()
{
	stage_type = sType::EDITOR_STAGE;
	previous_stage_type = sType::INTRO_STAGE;
	object = 1;
	selected = NULL;
	plane = _plane;
	isPlaneDrawing = false;
	texture_atlas = Texture::Get("data/atlasEditor.png");
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texturegui.fs");
	selected_opt = NONE_EDITOR;
	isTree = false;
	tree_index_selected = 0;
}

void EditorStage::render(void)
{
	std::string text = "MODO EDITOR\nControles basicos:\nF: posicionar entitad en cursor\nU: seleccionar entidad en cursor\nG: dejar de manipular la entidad\nO: siguiente tipo\nP: tipo anterior\nL,J: mover en eje x\nI,K: mover en eje z\nN,M: mover en eje y\nY: rotar en eje y";

	//Render the world in a simple way
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	Camera* camera = Camera::current;
	camera->enable();

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//render the scene + bullets
	glEnable(GL_CULL_FACE);
	if (isPlaneDrawing) {
		Shader* shader;
		shader = Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs");
		shader->enable();
		Scene::getInstance()->light->uploadToShader(shader);
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_texture", Texture::Get("data/plane.png"), 0);
		shader->setUniform("u_model", Matrix44());
		shader->setUniform("u_material_shininess", 1000000.0f);
		plane->render(GL_TRIANGLES);
		shader->disable();
	}
	Scene::getInstance()->root.render();
	Scene::getInstance()->tree->render();

	if (selected != NULL)
		selected->render();

	Scene::getInstance()->player->render();
	EnemyManager::getInstance()->render();
	glDisable(GL_CULL_FACE);

	//Draw the floor grid
	drawGrid();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	float window_height = Game::instance->window_height;
	float window_width = Game::instance->window_width;
	float window_centerx = window_width / 2.0;
	window_centerx += window_width / 3.0;

	Vector4 color_green(0, 1, 0, 1);
	Vector4 color_red(1, 0, 0, 0.7);

	float aspect = window_width / window_height;
	

	if (renderButton(window_centerx, 50 * aspect, 150 * aspect, 50 * aspect, menu_atlas[(int)PLANE_EDITOR], texture_atlas, shader, color_green, color_red, isPlaneDrawing)) {
		isPlaneDrawing = !isPlaneDrawing;
	}

	if (renderButton(window_centerx, 110 * aspect, 150 * aspect, 50 * aspect, menu_atlas[(int)SAVE_EDITOR], texture_atlas, shader, color_green, color_red, selected_opt == SAVE_EDITOR)) {
		GameMap::getInstance()->saveMap();
		selected_opt = SAVE_EDITOR;
	}

	Vector4 color_hover(1, 1, 1, 1);
	Vector4 color_transparent(1, 1, 1, 0.6);

	if (renderButton(window_centerx, 170 * aspect, 150 * aspect, 50 * aspect, menu_atlas[(int)RETURN_EDITOR], texture_atlas, shader, color_hover, color_transparent, false)) {
		change = true;
		change_to = previous_stage_type;
	}

	drawText(2, 20, text, Vector3(1, 1, 1), 2);
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
		updateValuesSelected(pos, selected->scale, selected->angle);
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
		//update values
		updateValuesSelected(selected->getPosition(), scale, selected->angle);
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
		case 'y':	//change in the direction of axis y
			pos.y += value;
			break;
		default:
			break;
	}

	//update model with new values
	updateValuesSelected(pos, selected->scale, selected->angle);
}

//change the prototype object to the next on the list of prototypes saved on gamemap
void EditorStage::changePropSelected(int* prop)
{
	if (selected == NULL)
		return;

	sProp* prototype = &GameMap::getInstance()->prototypes[*prop];
	
	//not empty prototypes
	if ((int)prototype->index == (int)EMPTY) {
		prototype = &GameMap::getInstance()->prototypes[4];
		*prop = 4;
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

void EditorStage::updateValuesSelected(Vector3 pos, float scale, float angle)
{
	selected->model.setTranslation(pos.x, pos.y, pos.z);

	selected->model.m[0] = scale;
	selected->model.m[5] = scale;
	selected->model.m[10] = scale;

	selected->model.rotate(angle * DEG2RAD, Vector3(0, 1, 0));
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

	//entity selected exists?
	bool found = false;
	isTree = false;

	//look if it is a world entity
	for (int i = 0; i < world_enitities.size(); i++)
	{
		Entity* ent = world_enitities[i];
		if (ent->type == PLAYER || ent->type == EMPTY)
			continue;

		Mesh* mesh = map->prototypes[(int)ent->type].mesh;
		Vector3 collision;
		Vector3 collision_normal;

		//check if the ray collides with a mesh
		if (mesh->testRayCollision(ent->model, origin, dir, collision, collision_normal) == false)
			continue;

		object = (int)ent->type;

		//create a new EntityMesh
		selected = new EntityMesh(&map->prototypes[(int)ent->type]);
		selected->prop = &map->prototypes[(int)ent->type];
		selected->model.setTranslation(ent->getPosition().x, ent->getPosition().y, ent->getPosition().z);

		selected->angle = ent->angle;
		selected->scale = ent->scale;

		selected->model.m[0] = ent->scale;
		selected->model.m[5] = ent->scale;
		selected->model.m[10] = ent->scale;

		selected->model.rotate(ent->angle * DEG2RAD, Vector3(0, 1, 0));

		scene->root.children[i] = selected;
		found = true;
		break;
	}

	//look if it was a tree
	if (found == false) {
		//get all the trees of the world
		std::vector<Matrix44> world_trees = scene->tree->vertices;
		eType tree_type = scene->tree->getType();
		Mesh* mesh = GameMap::getInstance()->prototypes[(int)tree_type].mesh;
		//initially no tree selected
		tree_index_selected = 0;
		for (int i = 0; i < world_trees.size(); i++) {
			Matrix44 tree_model = world_trees[i];
			Vector3 collision;
			Vector3 collision_normal;

			//check if the ray collides with a mesh
			if (mesh->testRayCollision(tree_model, origin, dir, collision, collision_normal) == false)
				continue;
			
			object = (int)tree_type;

			selected = new EntityMesh(&map->prototypes[(int)tree_type]);
			selected->prop = &map->prototypes[(int)tree_type];
			selected->model = tree_model;

			//store index position for later assign back the updated matrix
			tree_index_selected = i;
			//do not show the tree of vertices[i]
			scene->tree->vertices[i] = Matrix44();
			scene->tree->vertices[i].scale(0,0,0);
			found = true;
			isTree = true; //the entity selected is a tree
			break;
		}
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
			if (selected != NULL) {
				selected->type = (eType)object;
				if (isTree) {	//if it is a tree saved it back to the vertices vector of the scene
					Scene::getInstance()->tree->vertices[tree_index_selected] = selected->model;
					isTree = false;
				}
			}
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
		case SDLK_n:
			changePositionSelected('y', -0.5);
			break;
		case SDLK_m:
			changePositionSelected('y', 0.5);
			break;
		case SDLK_y:	//rotate entity 90 degress in y axis
			changeRotationSelected('y');
			break;
		case SDLK_u:	//select a entity from the scene doing a ray test collision
			selectEntity();
			break;
	}
}

/*********PLAY STAGE********/
PlayStage::PlayStage(Player* _player, Camera* _camera, bool* _free_cam, SkyBox* _sky, Mesh* _plane, Character* _character) : Stage()
{
	player = _player;
	character = _character;
	stage_type = sType::PLAY_STAGE;
	camera = _camera;
	free_cam = _free_cam;
	active_fbo = false;
	time = &Game::instance->time;
	sky = _sky;
	plane = _plane;

	fbo = new FBO();
	fbo->create(Game::instance->window_width, Game::instance->window_height);

	fxshader = Shader::Get("data/shaders/quad.vs", "data/shaders/pruebas.fs");
	audio_ambient = Audio::Get("data/audio/OutlawsFromTheWest.mp3");
	channel_ambient = audio_ambient->playSound(0);
	volume_ambient = 0.0;
	texture_atlas = Texture::Get("data/atlasPlay.png");
}

void PlayStage::render(void)
{
	/*****CAMERA FOLLOWS PLAYER*****/
	Matrix44 m = player->getGlobalMatrix();
	Vector3 eye = m * Vector3(0, 6, -12);
	//float height = 6 + sin(time * 8.0) * 0.5 * clamp(player.velocity.length() * 0.1,0.0.,1.0.)
	//Vector3 eye = m * Vector3(0, height, 12);
	Vector3 center = m * Vector3(0, 3, 0);
	Vector3 up = m.rotateVector(Vector3(0, 1, 0));
	if (*free_cam == false)
		camera->lookAt(eye, center, up);
	/******************************/

	if (active_fbo == false) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderWorld();
	}
	else {
		renderWithFBO();
	}

	if (selected == NONE_PLAY) {
		//some buttons
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		float window_height = Game::instance->window_height;
		float window_width = Game::instance->window_width;
		float window_centerx = window_width / 2.0;
		float aspect = window_width / window_height;
		Vector4 color_hover(1, 1, 1, 1);
		Vector4 color_transparent(1, 1, 1, 0.6);
		Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texturegui.fs");
		if (renderButton((window_centerx + (window_centerx / 1.15)), 30 * aspect, 50 * aspect, 50 * aspect, menu_atlas[(int)PAUSE_PLAY], texture_atlas, shader, color_hover, color_transparent, false)) {
			change = true;
			change_to = PAUSE_STAGE;
			selected = PAUSE_PLAY;
		}
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
}

//render world using the FBO
void PlayStage::renderWithFBO()
{
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	fbo->bind();

	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderWorld();

	fbo->unbind();

	glDisable(GL_DEPTH_TEST);
	
	fxshader->enable();
	fxshader->setUniform("u_time", *time);
	fbo->color_textures[0]->toViewport(fxshader);
}

void PlayStage::renderWorld()
{
	//set the camera as default
	camera->enable();

	//set flags
	glDisable(GL_BLEND);
	//glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	/***********SKY*************/
	glDisable(GL_DEPTH_TEST);
	sky->render(camera->viewprojection_matrix);
	glEnable(GL_DEPTH_TEST);
	/**********PLANE************/
	Shader* shader;
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/textureTerrain.fs");
	shader->enable();
	Scene::getInstance()->light->uploadToShader(shader);
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_camera_pos", camera->eye);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_texture", Texture::Get("data/plane.png"), 0);
	shader->setUniform("u_detail_texture", Texture::Get("data/detail.TGA"), 1);
	shader->setUniform("u_detail2_texture", Texture::Get("data/detail2.TGA"), 2);
	shader->setUniform("u_model", Matrix44());
	shader->setUniform("u_material_shininess", 1000000.0f);
	plane->render(GL_TRIANGLES);
	shader->disable();

	//render all the scene + bullets
	glEnable(GL_CULL_FACE);
	player->render(*time);
	character->render(*time);
	Scene::getInstance()->root.render();
	Scene::getInstance()->tree->render();
	BulletManager::getInstance()->render();
	EnemyManager::getInstance()->render(*time);
	glDisable(GL_CULL_FACE);

}

void PlayStage::saveCharacter()
{
	float distanceWithCharacter = player->position.distance(character->position);

	if (distanceWithCharacter <= 10.0 && distanceWithCharacter >= 0) {
		character->isSaved = true;
		character->angle_player = &player->angle;
	}
}

void PlayStage::update(double seconds_elapsed)
{
	player->update(seconds_elapsed);
	character->update(seconds_elapsed);
	saveCharacter();
	audio_ambient->changeVolume(channel_ambient, volume_ambient);

	//update bullets
	BulletManager::getInstance()->update(seconds_elapsed);
	BulletManager::getInstance()->hasCollisioned();

	//update enemies
	EnemyManager::getInstance()->update(seconds_elapsed);

	//update aspas
	Scene::getInstance()->updateAspas(seconds_elapsed);

	if (player->health <= 0) {
		change = true;
		change_to = GAMEOVER_STAGE;
	}
}

void PlayStage::onKeyDown(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
		case SDLK_z: 
			player->setSpeed(150); 
			break;
		case SDLK_SPACE:
			if (player->hasGun == true)
			{
				BulletManager* bm = BulletManager::getInstance();
				//get position of the gun
				Vector3 pos = player->gun->getPosition();
				pos.z = pos.z - 1;
				//obtain direction and create the bullet
				Matrix44 R;
				R.setRotation(player->gun->angle * DEG2RAD, Vector3(0, 1, 0));
				Vector3 front = R.rotateVector(Vector3(0, 0, -1));
				Vector3 vel = front * 150;
				bm->createBullet(pos, vel , player->gun->scope, player->gun->damage, eAuthor::PLAYER_BULLET, 1, player->gun->angle);

				//player knockback
				player->position = player->position - vel * 0.001;

				//screen shake
				//Camera* current = Camera::current;
				//current->eye = current->eye - 3*Vector3(0, 1, 0);
				
				if (player->gun->prop->index == eType::SHOTGUN) {
					//up right
					Matrix44 R1;
					R1.setRotation((player->gun->angle + 5) * DEG2RAD, Vector3(0, 1, 0));
					Vector3 front1 = R1.rotateVector(Vector3(0.05, 0.05, -1));
					Vector3 vel1 = front1 * 150;
					bm->createBullet(pos, vel1, player->gun->scope, player->gun->damage, eAuthor::PLAYER_BULLET, 1, player->gun->angle + 5);

					//midle right
					R1.setRotation((player->gun->angle + 5) * DEG2RAD, Vector3(0, 1, 0));
					front1 = R1.rotateVector(Vector3(0.05, 0, -1));
					vel1 = front1 * 150;
					bm->createBullet(pos, vel1, player->gun->scope, player->gun->damage, eAuthor::PLAYER_BULLET, 1, player->gun->angle + 5);

					//up left
					R1.setRotation((player->gun->angle - 5) * DEG2RAD, Vector3(0, 1, 0));
					front1 = R1.rotateVector(Vector3(-0.05, 0.05, -1));
					vel1 = front1 * 150;
					bm->createBullet(pos, vel1, player->gun->scope, player->gun->damage, eAuthor::PLAYER_BULLET, 1, player->gun->angle - 5);

					//midle left
					R1.setRotation((player->gun->angle - 5) * DEG2RAD, Vector3(0, 1, 0));
					front1 = R1.rotateVector(Vector3(-0.05, 0, -1));
					vel1 = front1 * 150;
					bm->createBullet(pos, vel1, player->gun->scope, player->gun->damage, eAuthor::PLAYER_BULLET, 1, player->gun->angle - 5);

					//up midle
					R1.setRotation(player->gun->angle * DEG2RAD, Vector3(0, 1, 0));
					front1 = R1.rotateVector(Vector3(0, 0.05, -1));
					vel1 = front1 * 150;
					bm->createBullet(pos, vel1, player->gun->scope, player->gun->damage, eAuthor::PLAYER_BULLET, 1, player->gun->angle);

					player->position = player->position - vel * 0.002;

					Audio* audio = Audio::Get("data/audio/shotgun_shot2.wav");
					audio->playSound();
				}
				else if (player->gun->prop->index == eType::REVOLVER){
					Audio* audioR = Audio::Get("data/audio/revolver_shot2.wav");
					audioR->playSound();
				}
				else {	//microgun
					Audio* audioM = Audio::Get("data/audio/microgun_shot.wav");
					audioM->playSound();
				}
			}
			break;
		case SDLK_6:	//render the world with an FBO
			active_fbo = !active_fbo;
			break;
	}
}

void PlayStage::onKeyUp(SDL_KeyboardEvent event)
{
	switch (event.keysym.sym)
	{
	case SDLK_z:
		player->setSpeed(50);
		break;
	default:
		break;
	}
}

/*********PAUSE STAGE******/
PauseStage::PauseStage(PlayStage* _playstage) : Stage()
{
	stage_type = sType::PAUSE_STAGE;
	play_stage = _playstage;
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texturegui.fs");
	texture_atlas = Texture::Get("data/atlasPause.png");
	texture_atlas_controls = Texture::Get("data/atlasControls.png");
	fxshader = Shader::Get("data/shaders/quad.vs", "data/shaders/pause.fs");
	ps_fxshader = NULL;
	volume_bars = (int)(play_stage->volume_ambient * 10.0f);
}

void PauseStage::render(void)
{
	play_stage->active_fbo = true;
	play_stage->fxshader = fxshader;
	play_stage->render();

	//some buttons
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	float window_height = Game::instance->window_height;
	float window_width = Game::instance->window_width;
	
	if (selected == CONTROLS_PAUSE) {	//cshow controls
		renderControls(window_width, window_height);
	}
	else if (selected == MUSIC_PAUSE) {	//volume options
		renderVolume(window_width, window_height);
	}
	else {	//main menu of pause
		float window_centerx = window_width / 2.0;
		float aspect = window_width / window_height;
		Vector4 color_hover(1, 1, 1, 1);
		Vector4 color_transparent(1, 1, 1, 0.6);

		renderGUI(window_centerx, 105 * aspect, 250 * aspect, 125 * aspect, menu_atlas[(int)TITLE_PAUSE], texture_atlas, shader);

		if (renderButton((window_centerx+(window_centerx/1.5)), 105 * aspect, 50 * aspect, 50 * aspect, menu_atlas[(int)MUSIC_PAUSE], texture_atlas, shader, color_hover, color_transparent, false)) {
			selected = MUSIC_PAUSE;
		}

		if (renderButton(window_centerx, 200 * aspect, 150 * aspect, 50 * aspect, menu_atlas[(int)EDITOR_PAUSE], texture_atlas, shader, color_hover, color_transparent, false)) {
			changeState(EDITOR_STAGE);
		}

		if (renderButton(window_centerx, 260 * aspect, 150 * aspect, 50 * aspect, menu_atlas[(int)CONTROLS_PAUSE], texture_atlas, shader, color_hover, color_transparent, false)) {
			selected = CONTROLS_PAUSE;
		}

		if (renderButton(window_centerx, 320 * aspect, 150 * aspect, 50 * aspect, menu_atlas[(int)RETURN_PAUSE], texture_atlas, shader, color_hover, color_transparent, false)) {
			changeState(PLAY_STAGE);
		}

		if (renderButton(window_centerx, 380 * aspect, 150 * aspect, 50 * aspect, menu_atlas[(int)EXIT_PAUSE], texture_atlas, shader, color_hover, color_transparent, false)) {
			changeState(EXIT_STAGE);
		}

	}	
}

void PauseStage::renderControls(float width, float height)
{
	float window_centerx = width / 2.0;
	float aspect = width / height;
	float window_quart = window_centerx / 2.0;
	Vector4 color_hover(1, 1, 1, 1);
	Vector4 color_transparent(1, 1, 1, 0.6);

	renderGUI(window_centerx, 105 * aspect, 250 * aspect, 125 * aspect, controls_atlas[(int)TITLE_CONTROLS], texture_atlas_controls, shader);

	renderGUI(window_centerx, 230 * aspect, 250 * aspect, 90 * aspect, controls_atlas[(int)MOVE_CONTROLS], texture_atlas_controls, shader);

	renderGUI(window_centerx - window_quart, 310 * aspect, 250 * aspect, 50 * aspect, controls_atlas[(int)SHOOT_CONTROLS], texture_atlas_controls, shader);

	renderGUI(window_centerx + window_quart, 310 * aspect, 230 * aspect, 40 * aspect, controls_atlas[(int)RUN_CONTROLS], texture_atlas_controls, shader);

	if (renderButton(window_centerx, 400 * aspect, 150 * aspect, 50 * aspect, controls_atlas[(int)RETURN_CONTROLS], texture_atlas_controls, shader, color_hover, color_transparent, false)) {
		selected = NONE_PAUSE;
	}
}

void PauseStage::renderVolume(float width, float height)
{
	float window_centerx = width / 2.0;
	float aspect = width / height;
	float window_quart = window_centerx / 2.0;
	Vector4 color_hover(1, 1, 1, 1);
	Vector4 color_transparent(1, 1, 1, 0.6);

	//volume buttons
	if (renderButton(window_quart, 230 * aspect, 50 * aspect, 50 * aspect, menu_atlas[(int)MUSIC_LOW_PAUSE], texture_atlas, shader, color_hover, color_transparent, false)) {
		float target = play_stage->volume_ambient - 0.1;
		if (target > 0.0) {
			play_stage->volume_ambient = target;
			volume_bars -= 1;
		}		
	}

	if (renderButton(window_centerx + window_quart, 230 * aspect, 50 * aspect, 50 * aspect, menu_atlas[(int)MUSIC_HIGH_PAUSE], texture_atlas, shader, color_hover, color_transparent, false)) {
		float target = play_stage->volume_ambient + 0.1;
		if (target < 1.1) {
			play_stage->volume_ambient = target;
			volume_bars += 1;
		}
	}

	//compute distance between bars volume
	float dist = (window_centerx + window_quart - 50 * aspect) - (window_quart + 50 * aspect);
	dist = dist / 9.0;

	//draw the bars volume
	for (int i = 0; i < volume_bars; i++) {
		renderGUI((window_quart + 50 * aspect) + (i*dist), 230 * aspect, 50 * aspect, 50 * aspect, menu_atlas[(int)MUSIC_BAR_PAUSE], texture_atlas, shader);
	}

	//return button
	if (renderButton(window_centerx, 300 * aspect, 150 * aspect, 50 * aspect, menu_atlas[(int)RETURN_PAUSE], texture_atlas, shader, color_hover, color_transparent, false)) {
		selected = NONE_PAUSE;
	}
}

void PauseStage::changeState(sType objective)
{
	//stop rendering world with fbo
	play_stage->active_fbo = false;
	//restore fbo shader
	play_stage->fxshader = ps_fxshader;
	change = true;
	change_to = objective;
}

void PauseStage::update(double seconds_elapsed)
{
	play_stage->audio_ambient->changeVolume(play_stage->channel_ambient, play_stage->volume_ambient);
}

void PauseStage::onKeyDown(SDL_KeyboardEvent event)
{
}

/********FINAL STAGE*******/
FinalStage::FinalStage() : Stage()
{
	isWin = true;
	stage_type = sType::WIN_STAGE;
}

void FinalStage::render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (isWin) {
		renderWin();
	}
	else {
		renderGameOver();
	}
}

void FinalStage::renderWin(void)
{
	Camera* camera = Camera::current;
	camera->enable();

	//set flags
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	float window_height = Game::instance->window_height;
	float window_width = Game::instance->window_width;
	float window_centerx = window_width / 2.0;
	float window_centery = window_height / 2.0;

	float aspect = window_width / window_height;

	renderGUI(window_centerx, window_centery, window_centerx, window_centery, Vector4(0,0,1,1), Texture::Get("data/backgroundWin.png"), Shader::Get("data/shaders/basic.vs", "data/shaders/texturegui.fs"));
	
	/*
	Vector4 color_hover(1, 1, 1, 1);
	Vector4 color_transparent(1, 1, 1, 0.6);

	if (renderButton(window_centerx, 240 * aspect, 150 * aspect, 50 * aspect, menu_atlas[(int)START_INTRO], texture_atlas, shader, color_hover, color_transparent, selected == START_INTRO)) {
		selected = START_INTRO;
		change = true;
		change_to = PLAY_STAGE;
	}
	*/
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void FinalStage::renderGameOver(void)
{
	float width = Game::instance->window_width;
	width = width / 2.0;
	float height = Game::instance->window_height;
	height = height / 2.0;
	//drawText(width, height, "YOU DIED, GAME OVER", Vector3(1, 1, 1), 2);


	Camera* camera = Camera::current;
	camera->enable();

	//set flags
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	float window_height = Game::instance->window_height;
	float window_width = Game::instance->window_width;
	float window_centerx = window_width / 2.0;
	float window_centery = window_height / 2.0;

	float aspect = window_width / window_height;

	renderGUI(window_centerx, window_centery, window_centerx * aspect, window_centery * aspect, Vector4(0, 0, 1, 1), Texture::Get("data/backgroundWin.png"), Shader::Get("data/shaders/basic.vs", "data/shaders/texturegui.fs"));

	/*
	Vector4 color_hover(1, 1, 1, 1);
	Vector4 color_transparent(1, 1, 1, 0.6);

	if (renderButton(window_centerx, 240 * aspect, 150 * aspect, 50 * aspect, menu_atlas[(int)START_INTRO], texture_atlas, shader, color_hover, color_transparent, selected == START_INTRO)) {
		selected = START_INTRO;
		change = true;
		change_to = PLAY_STAGE;
	}
	*/
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void FinalStage::onKeyDown(SDL_KeyboardEvent event)
{
}

void FinalStage::changeWin(bool state)
{
	isWin = state;
}
