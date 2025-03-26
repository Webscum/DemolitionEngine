#include "headers/Demolition_Physics.h"
#include "headers/Demolition_Engine.h"


uint8_t RIGIDBODY_INDEX;

void processObjectPhysics(spaceObject* sObj){
	objectAttribute* objectRigidbodyAttr = getObjectAttribute(sObj, RIGIDBODY_INDEX);
	RB_2D* objectRigidbody = (RB_2D*) objectRigidbodyAttr->attribute;
	moveSpaceObjectBy(sObj, objectRigidbody->forceVector);
}

void physicsTick(){
	for(int physicsIndex = 0; physicsIndex < vectorTotal(&objectSpace); physicsIndex++){
		spaceObject* sObj = getObjectFromObjectSpace(physicsIndex);
		processObjectPhysics(sObj);
	}
}

void* addRigidbody(spaceObject *sObj, uint8_t type){
	objectAttribute* attr;
	RB_2D* objRigidbody;
	RB_2D_COL* objRigidbodyCollider;


	attr = (objectAttribute*) malloc(sizeof(objectAttribute));
	objRigidbody = (RB_2D*) malloc(sizeof(RB_2D));
	objRigidbodyCollider = (RB_2D_COL*) malloc(sizeof(RB_2D_COL));

	objRigidbodyCollider->type = BOX_COLLIDER;
	objRigidbodyCollider->area.box = (SDL_Rect){(int)sObj->coordinates.x, (int)sObj->coordinates.y, 100, 100};

	objRigidbody->collider = objRigidbodyCollider;

	attr->typeID = type;
	attr->attribute = (void*) objRigidbody;

	vectorPushBack(&sObj->attributes, attr);
	printf("Rigidbody Added!\n");
	return (void*) objRigidbody;
}

void freeRigidbody(spaceObject* sObj){
	objectAttribute* objAttr= getObjectAttribute(sObj, RIGIDBODY_INDEX);

	free(((RB_2D*)objAttr->attribute)->collider);
	free(objAttr->attribute);
	free(objAttr);
}

void initPhysics(){
	RIGIDBODY_INDEX = createObjAttribute(addRigidbody, freeRigidbody);
}