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
	CreateSpring(pkmWorld);
	
	return true;
}

update_status ModulePhysics::PreUpdate()
{
	pkmWorld->Step(1.0f / 60.0f, 6, 2);

	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate()
{
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

PhysBody* ModulePhysics::CreateBoundary(b2World *pkmWorld)
{
	// 1a Boundary
	b2BodyDef boundBodyDef;
	boundBodyDef.type = b2_staticBody;
	boundBodyDef.position.Set(0, 0);

	b2Body* boundBody = pkmWorld->CreateBody(&boundBodyDef);

	b2ChainShape boundShape;

	b2Vec2* boundVec = new b2Vec2[116 / 2];
	// Boundary Vertex array 1
	int pinballBound[116] =
	{
	378, 1022,
	500, 940,
	513, 942,
	536, 924,
	553, 906,
	553, 770,
	542, 741,
	520, 725,
	497, 718,
	497, 604,
	521, 566,
	543, 514,
	557, 446,
	564, 383,
	561, 310,
	553, 246,
	542, 203,
	528, 165,
	550, 187,
	563, 217,
	574, 247,
	585, 295,
	585, 1025,
	640, 1025,
	640, 277,
	627, 228,
	608, 184,
	576, 134,
	528, 86,
	453, 41,
	412, 29,
	372, 21,
	264, 21,
	204, 36,
	162, 54,
	128, 76,
	86, 119,
	50, 173,
	34, 228,
	25, 277,
	21, 320,
	21, 404,
	23, 442,
	29, 476,
	42, 522,
	62, 568,
	87, 603,
	87, 717,
	59, 725,
	37, 741,
	27, 765,
	27, 910,
	46, 926,
	72, 944,
	81, 940,
	208, 1024,
	208, 1079,
	379, 1079
	};

	for (uint i = 0; i < 116 / 2; ++i)
	{
		boundVec[i].x = PIXEL_TO_METERS(pinballBound[i * 2 + 0]);
		boundVec[i].y = PIXEL_TO_METERS(pinballBound[i * 2 + 1]);
	}

	boundShape.CreateLoop(boundVec, 116 / 2);

	b2FixtureDef boundFixture;
	boundFixture.shape = &boundShape;
	boundBody->CreateFixture(&boundFixture);

	delete boundVec;

	// Returning a PhysBody
	PhysBody* pboundBody = new PhysBody();
	pboundBody->body = boundBody;
	boundBody->SetUserData(pboundBody);
	pboundBody->width = pboundBody->height = 0;

	return pboundBody;
}

PhysBody* ModulePhysics::CreateSpring(b2World* pkmWorld)
{
	// Dynamic
	b2BodyDef springBodyDef;
	springBodyDef.type = b2_dynamicBody;
	springBodyDef.position.Set(PIXEL_TO_METERS(611), PIXEL_TO_METERS(990));

	b2Body* springBody = pkmWorld->CreateBody(&springBodyDef);

	b2CircleShape springShape;
	springShape.m_radius = PIXEL_TO_METERS(5);
	b2FixtureDef springFixture;
	springFixture.shape = &springShape;
	springFixture.density = 1.0f;

	springBody->CreateFixture(&springFixture);

	//Static Pivot
	b2BodyDef pivotBodyDef;
	pivotBodyDef.position.Set(PIXEL_TO_METERS(611), PIXEL_TO_METERS(1013));
	b2Body* pivotBody = pkmWorld->CreateBody(&pivotBodyDef);

	b2PolygonShape pivotShape;
	pivotShape.SetAsBox(PIXEL_TO_METERS(50) * 0.5f, PIXEL_TO_METERS(20) * 0.5f);
	pivotBody->CreateFixture(&pivotShape, 0.0f);

	// Prismatic Joint
	b2PrismaticJointDef jointDef;
	b2Vec2 worldAxis(1.0f, 0.0f);
	jointDef.Initialize(pivotBody, springBody, pivotBody->GetWorldCenter(), worldAxis);
	jointDef.lowerTranslation = -5.0f;
	jointDef.upperTranslation = 2.5f;
	jointDef.enableLimit = true;
	jointDef.maxMotorForce = 1.0f;
	jointDef.motorSpeed = 0.0f;
	jointDef.enableMotor = true;



	// Returning a PhysBody
	PhysBody* pSpringBody = new PhysBody();
	pSpringBody->body = springBody;
	springBody->SetUserData(pSpringBody);
	pSpringBody->width = pSpringBody->height = 0;

	return pSpringBody;
}

void PhysBody::GetPosition(int& x, int& y) const
{
	b2Vec2 position = body->GetPosition();
	x = METERS_TO_PIXELS(position.x) - (width);
	y = METERS_TO_PIXELS(position.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}
	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx * fx) + (fy * fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}
	return ret;
}