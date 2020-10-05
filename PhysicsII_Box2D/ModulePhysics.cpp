#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "math.h"

// TODO 1: Include Box 2 header and library


#define METERS_TO_PIXELS(X) (X*0.1)
#define PIXEL_TO_METERS(X) (X*10)

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;
}
// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{

	LOG("Creating Physics 2D environment");

	// TODO 2: Create a private variable for the world
	// - You need to send it a default gravity
	// - You need init the world in the constructor
	// - Remember to destroy the world after using it
	b2Vec2 gravity(0.0f, -10.0f);
	world = new b2World(gravity);


	// TODO 4: Create a a big static circle as "ground"
	b2BodyDef circleBodyDef;
	circleBodyDef.position.Set(0.0f, -10.0f);
	b2Body *groundCircle = world->CreateBody(&circleBodyDef);

	b2CircleShape circle;
	circle.m_p.Set(2.0f, 3.0f);
	circle.m_radius = 0.5f;

	groundCircle->CreateFixture(&circle, 0.0f);

	return true;
}

update_status ModulePhysics::PreUpdate()
{
	// TODO 3: Update the simulation ("step" the world)
	float timeStep = 1.0f / 60.0f;
	int32 velocityIterations = 8;
	int32 positionIterations = 2;

	world->Step(timeStep, velocityIterations, positionIterations);		
	


	return UPDATE_CONTINUE;
}
 
update_status ModulePhysics::PostUpdate()
{
	// TODO 5: On space bar press, create a circle on mouse position
	// - You need to transform the position / radius

	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// You will have to add more cases to draw boxes, edges, and polygons ...
			}
		}
	}

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;
	return true;
}
