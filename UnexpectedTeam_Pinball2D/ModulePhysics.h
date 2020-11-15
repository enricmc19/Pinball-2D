#pragma once
#include "Module.h"
#include "Globals.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

class b2World;
class b2Body;

class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}

	void GetPosition(int& x, int& y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int width, height;
	b2Body* body;
	Module* listener;
};

class ModulePhysics : public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	// Joints
	PhysBody *spring, *springPivot;
	PhysBody *ball;


private:	
	b2World* pkmWorld;
	bool debug;

	// Boundary Creation
	PhysBody* CreateBoundary();

	// Spring Creation
	PhysBody* CreateRectangle(int x, int y, int width, int height, b2BodyType type);
	PhysBody* CreatFlippers(int x, int y, int* points, int size);
	PhysBody* CreateStaticCircle(int x, int y, int radius);
	void CreatePrismaticJoint(PhysBody* dynamicBody, PhysBody* staticBody);
	PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, float _restitution = 0);
	

	public:
	// Ball Creation
	PhysBody* CreatePlayer(int x, int y, int radius);

	// Map Colliders Definitions
	PhysBody *lTopCol, *rTopCol;
	PhysBody *lMiddleCol,*rMiddleCol;
	PhysBody *lTriangCol, *rTriangCol;
	PhysBody *lBotCol, *rBotCol;

	// Kickers Definition
	PhysBody *lFlipper;
	PhysBody *rFlipper;
	PhysBody *lJoint;
	PhysBody *rJoint;
	b2RevoluteJoint *lFix;
	b2RevoluteJoint *rFix;
};