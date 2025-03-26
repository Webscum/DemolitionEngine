#ifndef DEMOLITION_ENGINE_PHYSICS
#define DEMOLITION_ENGINE_PHYSICS

#include "CVector.h"
#include <SDL2/SDL_rect.h>
#include "Demolition_Engine.h"

extern uint8_t RIGIDBODY_INDEX;
extern uint16_t physicsFrequency;

enum colliderType{
	BOX_COLLIDER,
	CIRCLE_COLLIDER,
	POLYGON_COLLIDER,
};

typedef struct DemolitionRigidbody2DCollider{
	enum colliderType type;
	union colliderData{
		SDL_Rect box;
		float circleRadius;
		vector polygon;
	} area;
} RB_2D_COL;

typedef struct DemolitionRigidBody2D{
	float mass;
	float gravityScale;
	gVec2D forceVector;
	uint16_t physicsFlags;
	void* collider;
}RB_2D;

extern void* addRigidbody(spaceObject* sObj, uint8_t type);
extern void freeRigidbody(spaceObject* sObj);

extern void processObjectPhysics(spaceObject* sObj);
extern void physicsUpdate();

extern void physicsTick();

extern void initPhysics();

#endif