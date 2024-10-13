#include "CVector.h"
#ifndef DEMOLITION_ENGINE_PHYSICS
#define DEMOLITION_ENGINE_PHYSICS

#ifndef DEMOLITION_ENGINE_H
#include "Demolition_Engine.h"
#endif

uint8_t RIGIDBODY_INDEX;

typedef struct DemolitionRigidBody2D{
	float mass;
	float gravityScale;
	gVec2D forceVector;
	uint16_t physicsFlags;
}RB_2D;

void processObjectPhysics(spaceObject* sObj){
	
}

void physicsTick(){
	for(int physicsIndex = 0; physicsIndex < vectorTotal(&objectSpace); physicsIndex++){
		spaceObject* sObj = getObjectFromObjectSpace(physicsIndex);
		processObjectPhysics(sObj);
	}
}

#endif