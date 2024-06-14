#ifndef DEMOLITION_ENGINE_H
#include "Demolition_Engine.h"
#endif

#ifndef DEMOLITION_ENGINE_PHYSICS
#define DEMOLITION_ENGINE_PHYSICS

typedef struct DemolitionRigidBody2D{
	float mass;
	float gravityScale;
	float forceVector[2];
}RB_2D;

void physicsTick(){

}

#endif