#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModulePlayer.h"


ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
	if (ballCreation == true)
	{
		player = (App->physics->CreatePlayer(429, 615, 15));
		ballCreation = false;
	}
	player->body->SetBullet(true);

	masterBallTex = App->textures->Load("Assets/masterBallTex.png");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	int playerPosX, playerPosY;
	player->GetPosition(playerPosX, playerPosY);
	App->renderer->Blit(masterBallTex, playerPosX, playerPosY, NULL, 1.0f, player->GetRotation());
	
	return UPDATE_CONTINUE;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}


