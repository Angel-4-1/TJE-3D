#ifndef STAGE_H
#define STAGE_H

#include "framework.h"
#include "entity.h"
#include "fbo.h"
#include "skybox.h"
#include "audio.h"

enum sType { NONE_STAGE, STAGE, LOADING_STAGE, INTRO_STAGE, TUTORIAL_STAGE, EDITOR_STAGE, PLAY_STAGE, PAUSE_STAGE, WIN_STAGE, GAMEOVER_STAGE, EXIT_STAGE };

class Stage
{
public:
	bool change;	// if true --> we want to change to another stage
	sType change_to;	// stage to which we want to change
	sType stage_type;
	Audio* click;

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

class LoadingStage : public Stage
{
public:
	int loaded;
	float total;
	bool hasFinished;

	LoadingStage();

	Player* player;
	Character* ch;
	Mesh* plane;
	Texture* plane_text;
	SkyBox* sky;
	FBO* fbo;

	void render(void);
	void update(double seconds_elapsed);

	void preloadMap();
	void preloadScene();
	void preloadItems();
	void preloadAnimations();

	void renderGUIPixels(float initx, float inity, float finalx, float finaly, Vector4 range, Texture* texture, Shader* shader);
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
		{0, 0.125 * 4, 0.125 * 5, 0.125},	// continue
	};

	int phase;

	Texture* texture_atlas;
	Shader* shader;

	//we need the player position for the enemies
	Vector3* player_position;

	TutorialStage(Vector3* _player_position);

	void render(void);
	void renderDifficulties(void);
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
	
	//selecting a mountain
	bool isMountain;
	int mountain_index_selected;

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

enum eOptionPlay { NONE_PLAY, PAUSE_PLAY, FACE_PLAY, EMPTYBAR_PLAY, BAR1_PLAY, BAR2_PLAY, BAR3_PLAY, BAR4_PLAY, BAR5_PLAY, BAR6_PLAY, SHOTGUN_PLAY, REVOLVER_PLAY, MICROGUN_PLAY, GAMEOVER_PLAY, GAMEWIN_PLAY };
enum ePhrases { NONE_PHRASE, FIND_PHRASE, GOBACK_PHRASE, SHOOT_PHRASE };

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

	//phrases that will appear during the game (hints)
	static const int NUM_OPTIONS_PHRASES = 15;
	float duration;
	Vector4 menu_atlas_phrases[NUM_OPTIONS] = {
		{0, 0, 0, 0},					// none
		{0, 0, 1, 0.125},			// find
		{0, 0.125, 1, 0.125},	// go back
		{0, 0.125 * 2, 1, 0.125},	// how to shoot
	};
	Texture* texture_atlas_phrases;
	ePhrases phrase_selected;
	float time_started_message;
	bool showMessage;

	//character possible positions
	static const int NUM_POSITIONS = 3;

	Vector3 positions_candidates[NUM_POSITIONS] = {
		{-372, 0, -436}, //top left camp
		{51, 0, -360},   //top middle camp
		{435, 0, -52},   //right middle camp
	};

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
	//first time play stage is called, the music will need to start
	bool hasStarted;

	HCHANNEL channel_ambient;
	float volume_ambient;
	Audio* audio_ambient;

	PlayStage(Player* _player, Camera* _camera, bool* _free_cam, SkyBox* _sky, Mesh* _plane, Character* _character);

	void render(void);
	void renderWithFBO(void);
	void renderWorld(void);

	void renderGUIHealth(float window_centerx, float aspect);
	void renderGUIWeapon(float window_centerx, float aspect);
	void renderMiniMap(float x, float y, float sizex, float sizey, Texture* texture);
	void saveCharacter();
	
	void renderGUIPhrase(float window_centerx, float aspect, int phrase);

	Vector3 generateRandomPositionCharacter();
	void isWin();

	void update(double seconds_elapsed);
	void onKeyDown(SDL_KeyboardEvent event);
	void onKeyUp(SDL_KeyboardEvent event);

	bool changeVolume(float volume);
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
enum eOptionGameWin { NONE_GAMEWIN, BACK_GAMEWIN, EXIT_GAMEWIN, TEXT_GAMEWIN };

class FinalStage : public Stage
{
public:
	bool isWin;
	float factor;
	PlayStage* play_stage;
	Shader* fxshader;	//fbo shader
	Shader* ps_fxshader;	//fbo shader of play stage

	static const int NUM_OPTIONS = 5;

	//Game over screen
	Texture* texture_atlas_gameover;
	eOptionGameOver selectedG = NONE_GAMEOVER;

	Vector4 menu_atlas[NUM_OPTIONS] = {
		{0, 0, 0, 0},						// none
		{0, 0, 0.125 * 3, 0.125 * 2},		// try again
		{0, 0.125 * 2, 0.125 * 3, 0.125},	// exit
		{0, 0.125 * 4, 1, 0.125},			// text
	};

	//Game Win screen
	Texture* texture_atlas_gamewin;
	eOptionGameWin selectedW = NONE_GAMEWIN;

	Vector4 menu_atlas_win[NUM_OPTIONS] = {
		{0, 0, 0, 0},						// none
		{0, 0, 0.125 * 3, 0.125 * 2},		// back to menu
		{0, 0.125 * 2, 0.125 * 3, 0.125},	// exit
		{0, 0.125 * 4, 1, 0.125 * 3},			// text
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