#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "math.h"

#include "Box2D/Box2D/Box2D.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	pkmWorld = NULL;
	debug = false;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	pkmWorld = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	CreateBoundary(pkmWorld);
	
	return true;
}

update_status ModulePhysics::PreUpdate()
{
	pkmWorld->Step(1.0f / 60.0f, 6, 2);

	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate()
{
	// On space bar press, create a circle on mouse position
	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		b2BodyDef body;
		body.type = b2_dynamicBody;
		float radius = PIXEL_TO_METERS(25);
		body.position.Set(PIXEL_TO_METERS(App->input->GetMouseX()), PIXEL_TO_METERS(App->input->GetMouseY()));

		b2Body* b = pkmWorld->CreateBody(&body);

		b2CircleShape shape;
		shape.m_radius = radius;
		b2FixtureDef fixture;
		fixture.shape = &shape;

		b->CreateFixture(&fixture);
	}

	if(App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		// TODO 1: When pressing 2, create a box on the mouse position

		// TODO 2: To have the box behave normally, set fixture's density to 1.0f
	}

	if(App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		// TODO 3: Create a chain shape using those vertices
		// remember to convert them from pixels to meters!
		/*
		int points[24] = {
			-38, 80,
			-44, -54,
			-16, -60,
			-16, -17,
			19, -19,
			19, -79,
			61, -77,
			57, 73,
			17, 78,
			20, 16,
			-25, 13,
			-9, 72
		};
		*/
	}

	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = pkmWorld->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
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
	delete pkmWorld;

	return true;
}

void ModulePhysics::CreateBoundary(b2World *pkmWorld)
{
	// 1a Boundary
	b2BodyDef boundBodyDef1;
	boundBodyDef1.type = b2_staticBody;
	boundBodyDef1.position.Set(0, 0);

	b2Body* boundBody1 = pkmWorld->CreateBody(&boundBodyDef1);

	b2ChainShape boundShape1;

	b2Vec2* boundVec1 = new b2Vec2[62 / 2];
	// Boundary Vertex array 1
	int pinballBound1[62] =
	{
	206, 1024,
	80, 940,
	62, 936,
	30, 910,
	30, 762,
	52, 727,
	86, 720,
	86, 598,
	53, 544,
	28, 463,
	22, 401,
	21, 316,
	31, 242,
	57, 160,
	106, 96,
	155, 61,
	226, 29,
	267, 20,
	375, 20,
	444, 40,
	498, 67,
	574, 136,
	603, 182,
	626, 231,
	638, 273,
	640, 1024,
	700, 1024,
	700, 0,
	0, 0,
	0, 1024,
	180, 1024
	};

	for (uint i = 0; i < 62 / 2; ++i)
	{
		boundVec1[i].x = PIXEL_TO_METERS(pinballBound1[i * 2 + 0]);
		boundVec1[i].y = PIXEL_TO_METERS(pinballBound1[i * 2 + 1]);
	}

	boundShape1.CreateLoop(boundVec1, 62 / 2);

	b2FixtureDef boundFixture1;
	boundFixture1.shape = &boundShape1;
	boundBody1->CreateFixture(&boundFixture1);

	delete boundVec1;


	// 2a Boundary
	b2BodyDef boundBodyDef2;
	boundBodyDef2.type = b2_staticBody;
	boundBodyDef2.position.Set(0, 0);

	b2Body* boundBody2 = pkmWorld->CreateBody(&boundBodyDef2);

	b2ChainShape boundShape2;

	b2Vec2* boundVec2 = new b2Vec2[52 / 2];
	// Boundary Vertex array 2
	int pinballBound2[52] = 
	{
	584, 1024,
	584, 296,
	577, 260,
	560, 212,
	532, 166,
	540, 200,
	548, 230,
	557, 271,
	561, 313,
	561, 398,
	559, 442,
	550, 495,
	532, 544,
	496, 602,
	496, 711,
	525, 720,
	546, 737,
	556, 766,
	556, 902,
	542, 920,
	524, 933,
	511, 943,
	499, 940,
	457, 970,
	383, 1024,
	566, 1024
	};

	for (uint i = 0; i < 52 / 2; ++i)
	{
		boundVec2[i].x = PIXEL_TO_METERS(pinballBound2[i * 2 + 0]);
		boundVec2[i].y = PIXEL_TO_METERS(pinballBound2[i * 2 + 1]);
	}

	boundShape2.CreateLoop(boundVec2, 52 / 2);

	b2FixtureDef boundFixture2;
	boundFixture2.shape = &boundShape2;
	boundBody2->CreateFixture(&boundFixture2);

	delete boundVec2;
}