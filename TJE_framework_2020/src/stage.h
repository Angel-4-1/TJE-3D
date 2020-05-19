#ifndef STAGE_H
#define STAGE_H

#include "framework.h"
#include "entity.h"

enum sType { STAGE, INTRO_STAGE, TUTORIAL_STAGE, EDITOR_STAGE, PLAY_STAGE, WIN_STAGE, GAMEOVER_STAGE, EXIT_STAGE };

class Stage
{
public:
	bool change;	// if true --> we want to change to another stage
	sType change_to;	// stage to which we want to change
	sType stage_type;

	Stage();

	virtual void render(void);
	virtual void update(double seconds_elapsed);

	sType whoAmI();

	//events
	virtual void onKeyDown(SDL_KeyboardEvent event);
	virtual void onKeyUp(SDL_KeyboardEvent event);
};

class IntroStage : public Stage
{
public:
	IntroStage();

	void render(void);
	void update(double seconds_elapsed);
	void onKeyDown(SDL_KeyboardEvent event);
};

class EditorStage : public Stage
{
public:
	EditorStage();

	EntityMesh* selected;
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
	void selectEntity();
};

class PlayStage : public Stage
{
public:
	Player* player;
	PlayStage(Player* _player);

	void render(void);
	void update(double seconds_elapsed);
	void onKeyDown(SDL_KeyboardEvent event);
	void onKeyUp(SDL_KeyboardEvent event);
};

class FinalStage : public Stage
{
public:
	bool isWin;

	FinalStage();

	// Depending on the value of isWin, render will call win or gameover
	void render(void);
	void renderWin(void);
	void renderGameOver(void);

	void onKeyDown(SDL_KeyboardEvent event);

	void changeWin(bool state);
};

#endif