#include "game.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"
#include "entity.h"
#include "scene.h"
#include "stage.h"
#include "gamemap.h"
#include "skybox.h"
#include "bulletmanager.h"
#include "enemy.h"
#include "tree.h"
#include "audio.h"
#include <cmath>

//some globals
Shader* shader = NULL;
Animation* anim = NULL;
float angle = 0;
FBO* fbo = NULL;

Game* Game::instance = NULL;

// Mygame
bool free_cam = false;
Scene* scene = NULL;
Player* player = NULL;
GameMap* gamemap = NULL;
Character* ch = NULL;

// Stages
Stage* current_stage = NULL;
LoadingStage* loading_stage = NULL;
IntroStage* intro_stage = NULL;
TutorialStage* tutorial_stage = NULL;
EditorStage* editor_stage = NULL;
PlayStage* play_stage = NULL;
PauseStage* pause_stage = NULL;
FinalStage* final_stage = NULL;

SkyBox* sky = NULL;

//plane
Mesh* plane = NULL;
Texture* plane_text = NULL;
std::vector<Matrix44> grass;

bool active_fbo = false;
BulletManager* bulletmanger = NULL;
EnemyManager* enemymanager = NULL;

//if we want to render GPU stats or not
bool showGPUStats = false;


Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	//create our camera
	camera = new Camera();
	camera->lookAt(Vector3(0.f,100.f, 100.f),Vector3(0.f,0.f,0.f), Vector3(0.f,1.f,0.f)); //position the camera and point to 0,0,0
	camera->setPerspective(70.f,window_width/(float)window_height,0.1f,10000.f); //set the projection, we want to be perspective
	camera->enable();

	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs");

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse

	//Inicializamos BASS al arrancar el juego (id_del_device, muestras por segundo, ...)
	if (BASS_Init(-1, 44100, 0, 0, NULL) == false) //-1 significa usar el por defecto del sistema operativo
	{
		//error abriendo la tarjeta de sonido...
		std::cout << "Error openning target sound" << std::endl;
	}

	showGPUStats = false;

	/*******CREATE THE WORLD*******/
	//get instance of the scene
	//scene = Scene::getInstance();
	//scene->tree = new Tree();

	//load the map
	/*
	gamemap = GameMap::getInstance();
	
	bool isLoaded = false;
	isLoaded = gamemap->loadMap("data/mymap.txt");

	//if the map was not loaded create a basic map
	if (isLoaded == false) {
		gamemap->createBasicMap();
	}
	*/

	//create the player
	//player = new Player(&gamemap->prototypes[(int)PLAYER], 200);
	//scene->player = player;

	//skybox for simulating the sky
	//sky = new SkyBox();

	//plane
	//plane = new Mesh();
	//plane->createPlane(512);	//size in units --> each pixel of the image is 1 unit in our world
								//centered in 0,0
	//plane_text = Texture::Get("data/plane.png");

	//fbo = new FBO();
	//fbo->create(window_width, window_height);

	/*
	scene->light = new Light();
	scene->light->diffuse_color.set(1, 1, 1);
	scene->light->specular_color.set(1, 1, 1);
	scene->light->position.set(-300, 80, 420);
	*/

	//prepare bullets and the enemies
	//bulletmanger = BulletManager::getInstance();
	//enemymanager = EnemyManager::getInstance();
	//enemymanager->createEnemy(Vector3(-300, 0, 400), Vector3(0, 0, 0), &player->position, 10, 50, 1, 0);
	//enemymanager->setDifficulty(eDifficultyEnemy::EASY_ENEMY, &player->position);
	//ch = new Character(&gamemap->prototypes[(int)CHARACTER], Vector3(-200, 0, 400), 10, &player->position);

	//init stages
	loading_stage = new LoadingStage();
	//intro_stage = new IntroStage();
	//tutorial_stage = new TutorialStage(&player->position);
	//editor_stage = new EditorStage(plane);
	//play_stage = new PlayStage(player, camera, &free_cam, sky, plane, ch);
	//pause_stage = new PauseStage(play_stage);
	//final_stage = new FinalStage(play_stage);
	current_stage = loading_stage;
	//current_stage = intro_stage;
}

//what to do when the image has to be draw
void Game::render(void)
{
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	//cal render method of current stage
	current_stage->render();
	
	//render the FPS, Draw Calls, etc
	if (showGPUStats) { drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2); }

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{
	float speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant

	//example
	angle += (float)seconds_elapsed * 10.0f;

	current_stage->update(seconds_elapsed);

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked ) //is left button pressed?
	{
		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f,-1.0f,0.0f));
		camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector( Vector3(-1.0f,0.0f,0.0f)));
	}

	if (free_cam == true)
	{
		//async input to move the camera around
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
		if (Input::isKeyPressed(SDL_SCANCODE_RSHIFT)) speed /= 10; //move slower with right shift
		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_Q)) camera->move(Vector3(0.0f, 1.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_T)) camera->move(Vector3(0.0f, -1.0f, 0.0f) * speed);
	}

	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
	
	//change state
	if (current_stage->change) {
		changeState();
	}
}

// Select to which state go
void Game::changeState()
{
	current_stage->change = false;
	switch (current_stage->change_to)
	{
	case sType::INTRO_STAGE:
		/***AFTER LOAD HAS FINISHED, CREATE THE STAGES***/
		if (loading_stage->hasFinished)
		{
			player = loading_stage->player;
			ch = loading_stage->ch;
			plane = loading_stage->plane;
			plane_text = loading_stage->plane_text;
			sky = loading_stage->sky;
			fbo = loading_stage->fbo;
			if (intro_stage == NULL) { intro_stage = new IntroStage(); }
			if (tutorial_stage == NULL) { tutorial_stage = new TutorialStage(&player->position); }
			if (editor_stage == NULL) { editor_stage = new EditorStage(plane); }
			if (play_stage == NULL) { 
				play_stage = new PlayStage(player, camera, &free_cam, sky, plane, ch); 
			}
			if (pause_stage == NULL) { pause_stage = new PauseStage(play_stage); }
			if (final_stage == NULL) { final_stage = new FinalStage(play_stage); }
			loading_stage->hasFinished = false;
		}
		/****************/
		// If we go back to the Intro stage that means that we are going to restart the game
		intro_stage->selected = eOptionIntro::NONE_INTRO;
		current_stage = intro_stage;
		free_cam = true;
		break;
	case sType::TUTORIAL_STAGE:
		tutorial_stage->selected = eOptionTutorial::NONE_TUTORIAL;
		current_stage = tutorial_stage;
		break;
	case sType::EDITOR_STAGE:
		camera->enable();
		editor_stage->previous_stage_type = current_stage->whoAmI();
		current_stage = editor_stage;
		free_cam = true;
		break;
	case sType::PLAY_STAGE:
		current_stage = play_stage;
		play_stage->selected = eOptionPlay::NONE_PLAY;
		free_cam = false;
		if (play_stage->hasStarted == false) {
			play_stage->channel_ambient = play_stage->audio_ambient->playSound(0);
			bool isDone = play_stage->changeVolume(0.2);
			//change bars of pause according to the volume
			if (isDone) { pause_stage->volume_bars = 2; }
			play_stage->hasStarted = true;
			play_stage->showMessage = true;
			play_stage->phrase_selected = ePhrases::FIND_PHRASE;
			play_stage->time_started_message = time;
			//play_stage->character->reset(play_stage->generateRandomPositionCharacter(), &play_stage->player->position);
		}
		break;
	case sType::PAUSE_STAGE:
		current_stage = pause_stage;
		//save fbo shader of play_stage
		pause_stage->ps_fxshader = pause_stage->play_stage->fxshader;
		//load new fbo shader
		pause_stage->play_stage->active_fbo = true;
		pause_stage->play_stage->fxshader = pause_stage->fxshader;
		free_cam = false;
		break;
	case sType::WIN_STAGE:
		final_stage->changeWin(true);
		current_stage = final_stage;

		final_stage->ps_fxshader = final_stage->play_stage->fxshader;
		final_stage->play_stage->active_fbo = true;
		final_stage->play_stage->selected = eOptionPlay::GAMEWIN_PLAY;
		final_stage->play_stage->fxshader = final_stage->fxshader;
		free_cam = false;
		break;
	case sType::GAMEOVER_STAGE:
		final_stage->changeWin(false);
		current_stage = final_stage;
		
		final_stage->ps_fxshader = final_stage->play_stage->fxshader;
		final_stage->play_stage->active_fbo = true;
		final_stage->play_stage->selected = eOptionPlay::GAMEOVER_PLAY;
		final_stage->play_stage->fxshader = final_stage->fxshader;
		free_cam = false;
		break;
	case sType::EXIT_STAGE:	// Exit the game
		must_exit = true;
		break;
	}
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	current_stage->onKeyDown(event);

	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: 
			if (current_stage->whoAmI() != sType::INTRO_STAGE)
			{
				//reset play stage
				play_stage->hasStarted = false;
				play_stage->resetGame();
				play_stage->audio_ambient->StopSound(play_stage->channel_ambient);
				//change to intro stage
				current_stage->change = true;
				current_stage->change_to = sType::INTRO_STAGE;
			}
			break; //ESC key, kill the app
		case SDLK_TAB: free_cam = !free_cam; break;
		case SDLK_F1: Shader::ReloadAll(); break; 
		case SDLK_1: showGPUStats = !showGPUStats; break;
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
	current_stage->onKeyUp(event);
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse
	{
		Input::last_click_position.set(event.x, event.y);
	}

	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse
	{
		Input::last_click_position.set(-100, -100);
	}
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
}

void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}

