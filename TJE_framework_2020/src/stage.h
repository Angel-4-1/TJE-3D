#ifndef STAGE_H
#define STAGE_H

#include "framework.h"
#include "entity.h"
#include "fbo.h"
#include "skybox.h"
#include "audio.h"

enum sType { STAGE, INTRO_STAGE, TUTORIAL_STAGE, EDITOR_STAGE, PLAY_STAGE, PAUSE_STAGE, WIN_STAGE, GAMEOVER_STAGE, EXIT_STAGE };

class Stage
{
public:
	bool change;	// if true --> we want to change to another stage
	sType change_to;	// stage to which we want to change
	sType stage_type;

	Stage();

	virtual void render(void);
	virtual void update(double seconds_elapsed);
	void renderGUI(float x, float y, float sizex, float sizey, Vector4 range, Texture* texture, Shader* shader);
	bool renderButton(float x, float y, float sizex, float sizey, Vector4 range, Texture* texture, Shader* shader, Vector4 color_hover, Vector4 color_unclicked, bool active = true);

	sType whoAmI();

	//events
	virtual void onKeyDown(SDL_KeyboardEvent event);
	virtual void onKeyUp(SDL_KeyboardEvent event);
};

enum eOptionIntro { NONE_INTRO, START_INTRO, EDITOR_INTRO, EXIT_INTRO, TITLE_INTRO };

class IntroStage : public Stage
{
public:
	static const int NUM_OPTIONS = 5;
	eOptionIntro selected = NONE_INTRO;

	Vector4 menu_atlas[NUM_OPTIONS] = {
		{0, 0, 0, 0},					// none
		{0, 0, 0.125 * 5, 0.125},		// start
		{0, 0.125, 0.125 * 5, 0.125},	// editor
		{0, 0.125 * 2, 0.125 * 5, 0.125},	// exit
		{0, 0.125 * 4, 0.125 * 5, 0.125 * 3},	// title
	};

	Texture* texture_atlas;
	Shader* shader;

	IntroStage();

	void render(void);
	void update(double seconds_elapsed);
	void onKeyDown(SDL_KeyboardEvent event);
};

enum eOptionTutorial { NONE_TUTORIAL, EASY_TUTORIAL, MEDIUM_TUTORIAL, HARD_TUTORIAL, CONTINUE_TUTORIAL };

class TutorialStage : public Stage
{
public:
	static const int NUM_OPTIONS = 5;
	eOptionTutorial selected = NONE_TUTORIAL;

	Vector4 menu_atlas[NUM_OPTIONS] = {
		{0, 0, 0, 0},					// none
		{0, 0, 0.125 * 4, 0.125},		// easy
		{0, 0.125, 0.125 * 4, 0.125},	// medium
		{0, 0.125 * 2, 0.125 * 4, 0.125},	// hard
		{0, 0.125 * 3, 0.125 * 5, 0.125},	// continue
	};

	Texture* texture_atlas;
	Shader* shader;

	TutorialStage();

	void render(void);
};

enum eOptionEditor { NONE_EDITOR, PLANE_EDITOR, RETURN_EDITOR, SAVE_EDITOR };

class EditorStage : public Stage
{
public:
	Mesh* plane;
	bool isPlaneDrawing;
	sType previous_stage_type;

	static const int NUM_OPTIONS = 4;
	eOptionEditor selected_opt = NONE_EDITOR;

	Vector4 menu_atlas[NUM_OPTIONS] = {
		{0, 0, 0, 0},					// none
		{0, 0, 0.125 * 5, 0.125},		// editor
		{0, 0.125, 0.125 * 5, 0.125},	// return
		{0, 0.125 * 2, 0.125 * 5, 0.125},	// save
	};

	Texture* texture_atlas;
	Shader* shader;

	EditorStage(Mesh* _plane);

	EntityMesh* selected;

	//for selecting a tree
	bool isTree;
	int tree_index_selected;	//index position of the vector containing the trees of the scene
	
	//to rotate entites
	float angle = 90;

	//type of prototype of the entity
	int object;

	void render(void);
	void update(double seconds_elapsed);
	void onKeyDown(SDL_KeyboardEvent event);
	void addObjectInFront();
	void changeScaleSelected(bool value);
	void changePositionSelected(char var, float value);
	void changePropSelected(int* prop);
	void changeRotationSelected(char var);
	void updateValuesSelected(Vector3 pos, float scale, float angle);
	void selectEntity();
};

enum eOptionPlay { NONE_PLAY, PAUSE_PLAY, FACE_PLAY, EMPTYBAR_PLAY, BAR1_PLAY, BAR2_PLAY, BAR3_PLAY, BAR4_PLAY, BAR5_PLAY, BAR6_PLAY, SHOTGUN_PLAY, REVOLVER_PLAY, MICROGUN_PLAY, GAMEOVER_PLAY };

class PlayStage : public Stage
{
public:
	static const int NUM_OPTIONS = 15;
	static const int NUM_OPTIONS_CONTROLS = 6;

	eOptionPlay selected = NONE_PLAY;

	Vector4 menu_atlas[NUM_OPTIONS] = {
		{0, 0, 0, 0},					// none
		{0, 0, 0.125, 0.125},		// pause
		{0, 0.125 * 2, 0.125 * 2, 0.125 * 2},	// face
		{0.125 * 3, 0.125, 0.125 * 3, 0.1},	// empty bar
		{0.125 * 3, 0.125 * 2, 0.125 * 3, 0.1},	// bar 1
		{0.125 * 3, 0.125 * 3, 0.125 * 3, 0.1},	// bar 2
		{0.125 * 3, 0.125 * 4, 0.125 * 3, 0.1},	// bar 3
		{0.125 * 3, 0.125 * 5, 0.125 * 3, 0.1},	// bar 4
		{0.125 * 3, 0.125 * 6, 0.125 * 3, 0.1},	// bar 5
		{0.125 * 3, 0.125 * 7, 0.125 * 3, 0.1},	// bar 6
		{0, 0.125 * 5, 0.125, 0.125},	// shotgun icon
		{0.125, 0.125 * 5, 0.125, 0.125},	// revolver icon
		{0, 0.125 * 6, 0.125, 0.125},	// microgun icon
	};
	Texture* texture_atlas;

	Player* player;
	Character* character;	//friend to find
	Camera* camera;
	bool* free_cam;
	bool active_fbo;
	float* time;
	FBO* fbo;
	SkyBox* sky;
	Mesh* plane;
	Shader* fxshader;	//fbo shader
	bool has_shot;

	HCHANNEL channel_ambient;
	float volume_ambient;
	Audio* audio_ambient;

	PlayStage(Player* _player, Camera* _camera, bool* _free_cam, SkyBox* _sky, Mesh* _plane, Character* _character);

	void render(void);
	void renderWithFBO(void);
	void renderWorld(void);

	void renderGUIHealth(float window_centerx, float aspect);
	void renderGUIWeapon(float window_centerx, float aspect);
	void saveCharacter();
	
	void update(double seconds_elapsed);
	void onKeyDown(SDL_KeyboardEvent event);
	void onKeyUp(SDL_KeyboardEvent event);

	void resetGame();
};

enum eOptionPause { NONE_PAUSE, EDITOR_PAUSE, CONTROLS_PAUSE, RETURN_PAUSE, EXIT_PAUSE, TITLE_PAUSE, MUSIC_PAUSE, MUSIC_HIGH_PAUSE, MUSIC_LOW_PAUSE, MUSIC_BAR_PAUSE };
enum eOptionControls { NONE_CONTROLS, MOVE_CONTROLS, SHOOT_CONTROLS, RUN_CONTROLS, TITLE_CONTROLS, RETURN_CONTROLS, CHANGE_CONTROLS };

class PauseStage : public Stage
{
public:
	static const int NUM_OPTIONS = 10;
	static const int NUM_OPTIONS_CONTROLS = 7;
	
	eOptionPause selected = NONE_PAUSE;

	Vector4 menu_atlas[NUM_OPTIONS] = {
		{0, 0, 0, 0},					// none
		{0, 0, 0.125 * 5, 0.125},		// editor
		{0, 0.125, 0.125 * 5, 0.125},	// controls
		{0, 0.125 * 2, 0.125 * 5, 0.125},	// return
		{0, 0.125 * 3, 0.125 * 5, 0.125},	// exit
		{0, 0.125 * 4, 0.125 * 6, 0.125 * 2},	// title
		{0, 0.125 * 6, 0.125, 0.125},	// music
		{0.125, 0.125 * 6, 0.125, 0.125},	// music high
		{0.125 * 2, 0.125 * 6, 0.125, 0.125},	// music low
		{0.125 * 3, 0.125 * 7, 0.125, 0.125},	// music bar
	};

	Vector4 controls_atlas[NUM_OPTIONS_CONTROLS] = {
		{0, 0, 0, 0},					// none
		{0, 0, 0.125 * 8, 0.125 * 2},		// move
		{0, 0.125 * 2, 0.125 * 7, 0.125},	// shoot
		{0, 0.125 * 3, 0.125 * 6, 0.125},	// run
		{0, 0.125 * 4, 0.125 * 6, 0.125 * 2},	// title
		{0, 0.125 * 6, 0.125 * 5, 0.125},	// return
		{0, 0.125 * 7, 0.125 * 6, 0.125},	// change gun
	};

	PlayStage* play_stage;	//to be able to render the world on the backgroud
	Texture* texture_atlas;
	Texture* texture_atlas_controls;
	Shader* shader;
	Shader* fxshader;	//fbo shader
	Shader* ps_fxshader;	//fbo shader of play stage
	int volume_bars;
	
	PauseStage(PlayStage* _playstage);

	void render(void);
	void renderControls(float width, float height);
	void renderVolume(float width, float height);
	void changeState(sType objective);
	void update(double seconds_elapsed);
	void onKeyDown(SDL_KeyboardEvent event);
};

enum eOptionGameOver { NONE_GAMEOVER, TRYAGAIN_GAMEOVER, EXIT_GAMEOVER, TEXT_GAMEOVER };

class FinalStage : public Stage
{
public:
	bool isWin;
	float factor;
	PlayStage* play_stage;
	Shader* fxshader;	//fbo shader
	Shader* ps_fxshader;	//fbo shader of play stage

	static const int NUM_OPTIONS = 5;

	Texture* texture_atlas_gameover;
	eOptionGameOver selected = NONE_GAMEOVER;

	Vector4 menu_atlas[NUM_OPTIONS] = {
		{0, 0, 0, 0},					// none
		{0, 0, 0.125 * 3, 0.125 * 2},		// try again
		{0, 0.125 * 2, 0.125 * 3, 0.125},		// exit
		{0, 0.125 * 4, 1, 0.125},		// text
	};

	FinalStage(PlayStage* _playstage);

	// Depending on the value of isWin, render will call win or gameover
	void render(void);
	void renderWin(void);
	void renderGameOver(void);

	void onKeyDown(SDL_KeyboardEvent event);

	void changeWin(bool state);

	void update(double seconds_elapsed);
};

#endif