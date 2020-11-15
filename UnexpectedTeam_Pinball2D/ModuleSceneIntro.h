#pragma once
#include "Module.h"
#include "p2List.h"
#include "Globals.h"
#include "Animation.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();

private:
	// Map Textures
	SDL_Texture* pinballTex;	
	SDL_Rect* pinballRec;

	// Spring Texture
	SDL_Texture* springTex;

	// Flippers Texture
	SDL_Texture* lKickerTex;
	SDL_Texture* rKickerTex;

	// Pokemon Animations + Rects
	SDL_Texture *slowpokeTex;
	Animation slowpoke;
	Animation *slowpokeAnim = &slowpoke;
	SDL_Rect slowpokeRect;

	SDL_Texture *collysterTex;
	Animation collyster;
	Animation *collysterAnim = &collyster;
	SDL_Rect collysterRect;

	uint throwFx;
};
