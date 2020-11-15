#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "math.h"



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

	CreateBoundary();
	spring = CreateRectangle(429, 710, 36, 15, b2_dynamicBody);
	springPivot = CreateRectangle(429, 723, 36, 10, b2_staticBody);
	CreatePrismaticJoint(spring, springPivot);

	// Colliders Creation
	int lMiddle[48] = {
	121, 370,
	86, 388,
	72, 363,
	61, 329,
	57, 297,
	57, 214,
	64, 170,
	76, 135,
	92, 107,
	121, 79,
	121, 140,
	109, 160,
	102, 185,
	104, 199,
	116, 214,
	186, 247,
	185, 258,
	102, 249,
	100, 286,
	185, 286,
	184, 293,
	121, 326,
	115, 344,
	120, 359
	};
	lMiddleCol = CreateChain(0,0,lMiddle,48);
	int rMiddle[44] = {
	285, 366,
	322, 386,
	340, 353,
	350, 300,
	350, 213,
	342, 163,
	326, 120,
	306, 95,
	286, 81,
	286, 139,
	297, 158,
	305, 187,
	300, 206,
	281, 220,
	224, 248,
	224, 256,
	308, 248,
	308, 289,
	223, 289,
	278, 318,
	291, 334,
	294, 344
	};
	rMiddleCol = CreateChain(0, 0, rMiddle, 44);

	int lTop[16] = {
	166, 68,
	169, 61,
	178, 61,
	181, 68,
	181, 103,
	177, 107,
	169, 107,
	166, 103
	};
	lTopCol = CreateChain(0, 0, lTop, 16,2);
	int rTop[16] = {
	227, 66,
	230, 62,
	239, 62,
	242, 66,
	242, 104,
	239, 107,
	230, 107,
	227, 103
	};
	rTopCol = CreateChain(0, 0, rTop, 16,2);

	int lTriang[16] = {
	98, 543,
	94, 538,
	91, 543,
	91, 583,
	96, 587,
	117, 601,
	128, 607,
	127, 600
	};
	lTriangCol = CreateChain(0, 0, lTriang, 16.2);
	int rTriang[16] = {
	317, 582,
	317, 546,
	314, 539,
	309, 544,
	280, 602,
	286, 604,
	296, 597,
	308, 588
	};
	rTriangCol = CreateChain(0, 0, rTriang, 16,2);

	int lBot[20] = {
	133, 648,
	76, 609,
	64, 597,
	60, 590,
	60, 536,
	56, 531,
	53, 535,
	53, 618,
	60, 629,
	124, 670
	};
	lBotCol = CreateChain(0, 0, lBot, 20);
	int rBot[20] = {
	273, 647,
	333, 608,
	344, 597,
	347, 591,
	347, 534,
	352, 530,
	357, 534,
	357, 618,
	354, 624,
	282, 672
	};
	rBotCol = CreateChain(0, 0, rBot, 20);

	// Kickers Creation
	int lKicker[14] = {
	3, 2,
	21, 2,
	56, 6,
	63, 10,
	57, 14,
	21, 20,
	3, 20
	};
	int rKicker[16] = {
	55, 3,
	41, 3,
	20, 6,
	5, 8,
	0, 11,
	6, 13,
	40, 20,
	56, 20
	};
	lFlipper = CreatFlippers(146, 663, lKicker, 14);
	rFlipper = CreatFlippers(265, 655, rKicker, 16);
	lJoint = CreateStaticCircle(140, 663, 3);
	rJoint = CreateStaticCircle(266, 664, 3);

	// Left Kicker Joint
	b2RevoluteJointDef revDef1;
	revDef1.bodyA = lFlipper->body;
	revDef1.bodyB = lJoint->body;
	revDef1.collideConnected = false;
	revDef1.upperAngle = 25 * DEGTORAD;
	revDef1.lowerAngle = -25 * DEGTORAD;
	revDef1.enableLimit = true;
	revDef1.localAnchorA.Set(PIXEL_TO_METERS(10), PIXEL_TO_METERS(8));
	lFix = (b2RevoluteJoint*)pkmWorld->CreateJoint(&revDef1);

	// Right Kicker Joint
	b2RevoluteJointDef revDef2;
	revDef2.bodyA = rFlipper->body;
	revDef2.bodyB = rJoint->body;
	revDef2.collideConnected = false;
	revDef2.upperAngle = 25 * DEGTORAD;
	revDef2.lowerAngle = -25 * DEGTORAD;
	revDef2.enableLimit = true;
	revDef2.localAnchorA.Set(PIXEL_TO_METERS(49), PIXEL_TO_METERS(10));
	rFix = (b2RevoluteJoint*)pkmWorld->CreateJoint(&revDef2);
	
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

	// This will iterate all objects in the world and draw the circles
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


//Creation Functions
PhysBody* ModulePhysics::CreateBoundary()
{
	// 1a Boundary
	b2BodyDef boundBodyDef;
	boundBodyDef.type = b2_staticBody;
	boundBodyDef.position.Set(0, 0);

	b2Body* boundBody = pkmWorld->CreateBody(&boundBodyDef);

	b2ChainShape boundShape;

	b2Vec2* boundVec = new b2Vec2[120 / 2];
	// Boundary Vertex array 
	int pinballBound[120] = 
	{
	265, 717,
	353, 657,
	357, 660,
	373, 650,
	388, 636,
	388, 537,
	380, 518,
	366, 507,
	349, 504,
	349, 419,
	361, 404,
	373, 384,
	383, 354,
	389, 330,
	391, 308,
	394, 283,
	394, 224,
	392, 197,
	387, 166,
	379, 138,
	370, 113,
	385, 133,
	393, 150,
	403, 178,
	409, 206,
	409, 720,
	449, 720,
	449, 187,
	442, 163,
	424, 121,
	403, 91,
	380, 67,
	346, 45,
	307, 26,
	259, 15,
	184, 15,
	138, 26,
	104, 45,
	73, 70,
	43, 106,
	26, 147,
	18, 190,
	14, 221,
	14, 283,
	19, 327,
	27, 363,
	40, 393,
	61, 423,
	61, 502,
	40, 509,
	28, 520,
	20, 537,
	20, 633,
	33, 647,
	53, 664,
	56, 655,
	143, 716,
	143, 769,
	266, 769,
	266, 733
	};


	for (uint i = 0; i < 120 / 2; ++i)
	{
		boundVec[i].x = PIXEL_TO_METERS(pinballBound[i * 2 + 0]);
		boundVec[i].y = PIXEL_TO_METERS(pinballBound[i * 2 + 1]);
	}

	boundShape.CreateLoop(boundVec, 120 / 2);

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

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, b2BodyType type)
{
	b2BodyDef body;
	body.type = type;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = pkmWorld->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody* ModulePhysics::CreatFlippers(int x, int y, int* points, int size)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = pkmWorld->CreateBody(&body);
	b2PolygonShape box;

	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	box.Set(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->height = pbody->width = 0;

	return pbody;
}

PhysBody* ModulePhysics::CreateStaticCircle(int x, int y, int radius)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = pkmWorld->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

void ModulePhysics::CreatePrismaticJoint(PhysBody* dynamicBody, PhysBody* staticBody)
{

	b2PrismaticJointDef prismaticJoint;
	prismaticJoint.collideConnected = true;
	prismaticJoint.bodyA = spring->body;
	prismaticJoint.bodyB = springPivot->body;

	prismaticJoint.localAnchorA.Set(0, 0);
	prismaticJoint.localAnchorB.Set(0, -1);
	prismaticJoint.localAxisA.Set(0, -1);
	prismaticJoint.enableLimit = true;
	prismaticJoint.lowerTranslation = -0.02;
	prismaticJoint.upperTranslation = 1.0;
	(b2PrismaticJoint*)pkmWorld->CreateJoint(&prismaticJoint);
}

PhysBody* ModulePhysics::CreatePlayer(int x, int y, int radius)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = pkmWorld->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 0.2;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, float _restitution)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = pkmWorld->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.restitution = _restitution;
	fixture.density = 1.0f;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;
	return pbody;
}


// PhysBody useful functions
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