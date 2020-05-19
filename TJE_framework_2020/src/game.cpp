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
#include <cmath>

//some globals
Mesh* mesh = NULL;
Texture* texture = NULL;
Shader* shader = NULL;
Animation* anim = NULL;
float angle = 0;
FBO* fbo = NULL;

Game* Game::instance = NULL;

// Mygame
bool free_cam = false;
Scene* scene = NULL;
Player* avion = NULL;
GameMap* gamemap = NULL;

// Stages
Stage* current_stage = NULL;
IntroStage* intro_stage = NULL;
EditorStage* editor_stage = NULL;
PlayStage* play_stage = NULL;
FinalStage* final_stage = NULL;

SkyBox* sky = NULL;

//plane
Mesh* plane = NULL;
Texture* plane_text = NULL;
std::vector<Matrix44> grass;

bool active_fbo = false;

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

	//load one texture without using the Texture Manager (Texture::Get would use the manager)
	texture = new Texture();
 	texture->load("data/texture.tga");

	// example of loading Mesh from Mesh Manager
	mesh = Mesh::Get("data/box.ASE");

	// example of shader loading using the shaders manager
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse

	/*******CREATE THE WORLD*******/
	// Get instance of the scene
	scene = Scene::getInstance();

	gamemap = new GameMap();
	bool isLoaded = false;
	isLoaded = gamemap->loadMap("data/mymap.txt");

	if (isLoaded == false) {
		gamemap->createBasicMap();
	}

	// Create some entities
	avion = new Player(&gamemap->prototypes[(int)PLAYER]);
	scene->root.addChild(avion);

	//init stages
	intro_stage = new IntroStage();
	editor_stage = new EditorStage();
	play_stage = new PlayStage(avion);
	current_stage = play_stage;

	sky = new SkyBox();

	//plane
	plane = new Mesh();
	plane->createPlane(512);	//size in units --> each pixel of the image is 1 unit in our world
								//centered in 0,0
	plane_text = Texture::Get("data/plane.png");
	//pixel the texture
	//plane_text->bind();
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	/***********************/

	//Grass instanced
	for (int i = 0; i < 1000; i++)
	{
		Matrix44 m;
		m.setTranslation(random(128), 0, random(128));
		m.rotate(random(180) * DEG2RAD, Vector3(0, 1, 0));
		m.scale(10, 10, 10);
		grass.push_back(m);
	}

	fbo = new FBO();
	fbo->create(window_width, window_height);
}

//what to do when the image has to be draw
void Game::render(void)
{
	//current_stage->render();
	/*****CAMERA FOLLOWS PLAYER*****/
	if (current_stage->whoAmI() == sType::PLAY_STAGE)
	{
		Matrix44 m = play_stage->player->getGlobalMatrix();
		Vector3 eye = m * Vector3(0, 6, 12);
		Vector3 center = m * Vector3(0, 3, 0);
		Vector3 up = m.rotateVector(Vector3(0, 1, 0));
		if (!free_cam)
			camera->lookAt(eye, center, up);
	}
	/******************************/
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);
	
	if (!active_fbo) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderWorld();
	}
	else {
		renderWithFBO();
	}

	//render the FPS, Draw Calls, etc
	drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::renderWithFBO()
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
	Shader* fxshader = Shader::Get("data/shaders/quad.vs", "data/shaders/pruebas.fs");
	fxshader->enable();
	fxshader->setUniform("u_time", time);
	fbo->color_textures[0]->toViewport(fxshader);
}

void Game::renderWorld() 
{
	//set the camera as default
	camera->enable();

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	/***********SKY*************
	glDisable(GL_DEPTH_TEST);
	sky->render(camera->viewprojection_matrix);
	glEnable(GL_DEPTH_TEST);
	/***************************/
	//plane
	shader->enable();
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_texture", plane_text, 0);
	shader->setUniform("u_model", Matrix44());
	plane->render(GL_TRIANGLES);
	shader->disable();
	/******INSTANCE*********
	Shader* shader_instanced = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");

	//enable shader
	shader_instanced->enable();

	//upload uniforms
	shader_instanced->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader_instanced->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader_instanced->setUniform("u_texture", gamemap->prototypes[(int)ORANGEGRASS].texture, 0);

	shader_instanced->setUniform("u_camera_pos", camera->eye);
	shader_instanced->setUniform("u_light_direction", Vector3(0.3, 0.6, 0.2).normalize());

	Vector3 offset = camera->eye;
	offset.y = 0.0;
	offset.x = round(offset.x / 128.0f) + 128.0f;
	offset.z = round(offset.z / 128.0f) + 128.0f;
	//shader_instanced->setUniform("u_offset", Vector3(0, 0, 0));

	//Vector3 pos = model_mat * mesh->box.center;
	Mesh* mesh = gamemap->prototypes[(int)ORANGEGRASS].mesh;
	shader_instanced->setUniform("u_offset", offset + Vector3(0, 0, 0));
	mesh->renderInstanced(GL_TRIANGLES, &grass[0], grass.size());
	shader_instanced->setUniform("u_offset", offset + Vector3(-128, 0, 0));
	mesh->renderInstanced(GL_TRIANGLES, &grass[0], grass.size());
	shader_instanced->setUniform("u_offset", offset + Vector3(0, 0, -128));
	mesh->renderInstanced(GL_TRIANGLES, &grass[0], grass.size());
	shader_instanced->setUniform("u_offset", offset + Vector3(-128, 0, -128));
	mesh->renderInstanced(GL_TRIANGLES, &grass[0], grass.size());

	//disable shader
	shader_instanced->disable();
	/***********************/
	
	//render all the scene
	glEnable(GL_CULL_FACE);
	scene->root.render();
	glDisable(GL_CULL_FACE);

	//Draw the floor grid
	drawGrid();
}

void Game::update(double seconds_elapsed)
{
	float speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant

	//example
	angle += (float)seconds_elapsed * 10.0f;

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked ) //is left button pressed?
	{
		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f,-1.0f,0.0f));
		camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector( Vector3(-1.0f,0.0f,0.0f)));
	}

	if (free_cam == false)
	{
		//if (current_stage->whoAmI() == sType::PLAY_STAGE)
		//	current_stage->update(seconds_elapsed);
		play_stage->update(seconds_elapsed);
	}
	else 
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

	if(play_stage->player->hasItem)
		active_fbo = true;
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	current_stage->onKeyDown(event);

	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_TAB: free_cam = !free_cam; break;
		case SDLK_F1: Shader::ReloadAll(); break; 
		case SDLK_1: current_stage = intro_stage; break;
		case SDLK_2: current_stage = editor_stage; break;
		case SDLK_3: current_stage = play_stage; break;
		case SDLK_6:	//render the world with an FBO
			active_fbo = !active_fbo; 
			break;
		case SDLK_9:	//save scene to a text file
			gamemap->saveMap(); 
			break;
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
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{
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

