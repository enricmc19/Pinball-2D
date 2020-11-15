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

	// Slowpoke PushBacks
	slowpoke.PushBack({ 0,0,50,65 });
	slowpoke.PushBack({ 53,0,50,65 });
	slowpoke.PushBack({ 106,0,55,65 });
	slowpoke.speed = 0.08;
	slowpokeRect = { 0,0,50,65 };

	// Collyster PushBacks
	collyster.PushBack({ 0,0,52,63 });
	collyster.PushBack({ 55,0,52,63 });
	collyster.PushBack({ 109,0,52,63 });
	collyster.speed = 0.07;
	collysterRect = { 0,0,52,63 };
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	// Texture Load
	pinballRec = new SDL_Rect{ 0,0,490,717 };
	pinballTex = App->textures->Load("Assets/Textures/pinballTex.png");

	slowpokeTex = App->textures->Load("Assets/Textures/slowpokeAnim.png");
	collysterTex = App->textures->Load("Assets/Textures/collysterAnim.png");

	springTex = App->textures->Load("Assets/Textures/springTex.png");
	lKickerTex = App->textures->Load("Assets/Textures/lKicker.png");
	rKickerTex = App->textures->Load("Assets/Textures/rKicker.png");

	// Audio Load
	throwFx = App->audio->LoadFx("Assets/Sound/shootFx.wav");
	flipperFx = App->audio->LoadFx("Assets/Sound/flipperFx.wav");
	App->audio->PlayMusic("Assets/Sound/city.ogg");

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
		App->physics->spring->body->ApplyForce({0,-250}, {0,0}, true);
		App->audio->PlayFx(throwFx);
	}

	// Kickers Texture Blit + Physics
	App->renderer->Blit(lKickerTex, 132, 655, NULL, 1.0f, App->physics->lFlipper->GetRotation(), 10, 8);
	App->renderer->Blit(rKickerTex, 220, 654, NULL, 1.0f, App->physics->rFlipper->GetRotation(), 49, 10);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		App->physics->rFlipper->body->ApplyForce({ -10, -80 }, { 0, 0 }, true);

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_UP)
		{
			App->physics->rFlipper->body->ApplyForce({ 6, 80 }, { 0, 0 }, true);
		}
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		App->physics->lFlipper->body->ApplyForce({ 6, 80 }, { 0, 0 }, true);

		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_UP)
		{
			App->physics->lFlipper->body->ApplyForce({ -10, -80 }, { 0, 0 }, true);
		}
	}
	if ((App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN) || (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN))
	{
		App->audio->PlayFx(flipperFx);
	}

	// ANIMATIONS
	// Slowpoke Animation
	SDL_Rect slowpokeCurrentFrame = slowpokeAnim->GetCurrentFrame();
	App->renderer->Blit(slowpokeTex, 76, 228, &slowpokeCurrentFrame);

	// Collyster Animation
	SDL_Rect collysterCurrentFrame = collysterAnim->GetCurrentFrame();
	App->renderer->Blit(collysterTex, 300, 230, &collysterCurrentFrame);

	return UPDATE_CONTINUE;
}

// Unload assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	App->textures->Unload(pinballTex);

	return true;
}

