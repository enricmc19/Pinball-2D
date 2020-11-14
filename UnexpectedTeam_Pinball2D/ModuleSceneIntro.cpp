#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModulePhysics.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	pinballRec = nullptr;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	pinballRec = new SDL_Rect{ 0,0,490,717 };
	pinballTex = App->textures->Load("Assets/pinballTex.png");
	springTex = App->textures->Load("Assets/springTex.png");

	throwFx = App->audio->LoadFx("Assets/shootFx.ogg");

	return ret;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	App->renderer->Blit(pinballTex, 0, 0, pinballRec, SCREEN_SIZE);

	// Spring Texture Blit + Physics
	int sPosX, sPosY;
	App->physics->spring->GetPosition(sPosX, sPosY);
	App->renderer->Blit(springTex, sPosX, sPosY, NULL, 1.0f);

	App->physics->spring->body->ApplyForce({ 0,-5 }, { 0, 0 }, true);
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		App->physics->spring->body->ApplyForce({0,10}, {0,0}, true);
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
	{
		App->physics->spring->body->ApplyForce({0,-350}, {0,0}, true);
		App->audio->PlayFx(throwFx, 0);
	}

	return UPDATE_CONTINUE;
}

// Unload assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	App->textures->Unload(pinballTex);

	return true;
}

