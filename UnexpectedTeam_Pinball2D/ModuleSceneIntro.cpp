#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
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


	pinballRec = new SDL_Rect{ 0,0,700,1024 };
	pinballTex = App->textures->Load("Assets/pinballText.png");


	return ret;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	App->renderer->Blit(pinballTex, 0, 0, pinballRec, SCREEN_SIZE);

	// Spring Texture Blit
	int sPositionX, sPositionY;
	App->physics->spring->GetPosition(sPositionX, sPositionY);
	//App->renderer->Blit(springText, sPositionX, sPositionY, NULL, 1.0f);
	App->physics->spring->body->ApplyForce({ 0,-5 }, { 0, 0 }, true);
	
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		App->physics->spring->body->ApplyForce({ 0,18 }, { 0, 0 }, true);
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
	{
		App->physics->spring->body->ApplyForce({ 0,-180 }, { 0, 0 }, true);
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

