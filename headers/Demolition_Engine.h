#ifndef DEMOLITION_ENGINE_H
#define DEMOLITION_ENGINE_H´
#include <SDL.h>
#include <SDL_image.h>

int n = 100;
vector objectSpace;
SDL_Window* engineWindow;
SDL_Renderer* engineRenderer;

struct spaceObject;
struct clickable;

typedef struct{
	SDL_Texture* tex;
	char* image;
} textureObjAttr;

typedef struct{
	clickable* area;
}renderSurface;

typedef struct{
	char* attributeType;
	void* attribute;
}objectAttribute;

typedef struct{
	char* name;
	void* renderer;
}demolitionRenderer;

typedef struct {
	float x;
	float y;
	float z;
	vector attributes;
} spaceObject;

// Mush code, gets the attribute of an spaceObject by name returns NULL pointer if there is no attribute of that name
objectAttribute* getObjectAttribute(spaceObject* obj, char* typeName){
	for(int i = 0; vectorTotal(&obj->attributes) > i; i++){
		objectAttribute* objAttr = (objectAttribute*) vectorGet(&obj->attributes, i);
		if(objectAttribute->type == typeName) return objAttr;
	}
	return NULL´;
}

void addObjectToSpace(spaceObject* spcObj){
	vector_init(&spcObj->attributes);
	objectSpace.pfVectorAdd(&objectSpace, (void*) spcObj);
}

void initSpaceObject(spaceObject* spcObj){
	vector_init(&spcObj->attributes);
}

void addAttribute(spaceObject* sObj, void* attribute){
	sObj->attributes.pfVectorAdd(&sObj->attributes, attribute);
}

void Demolish(){
	vector_init(&objectSpace);
	
	printf("%d, Demolition Engine Working!\n", n);
}

#include "Demolition_UI.h"
#include "Demolition_Visual.h"
#endif