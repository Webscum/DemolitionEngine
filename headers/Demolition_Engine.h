 pla#ifndef DEMOLITION_ENGINE_H
#define DEMOLITION_ENGINE_H
#include "CVector.h"
#include <SDL.h>
#include <SDL_image.h>

int n = 100;

typedef struct spaceObject;

typedef struct{
	SDL_Texture tex;
	char* image;
} textureObjAttr;

typedef struct{
	SDL_Rect* area;
	int layer;
}renderSurface;

typedef struct{
	char* attributeType;
	void* attribute;
}objectAttribute;

typedef struct{
	char* name;
	void* renderer;
}demolitionRenderer;

struct {
	float x;
	float y;
	vector attributes;
} spaceObject;

void initSpaceObject(spaceObject* spcObj){
	vector_init(&spc.Obj->attributes)
}

void addAttribute(spaceObject* sObj, objectAttribute* attribute){
	sObj->attributes.pushBack(&sObj->attributes, attribute)
}

void Demolish(){
	
	
	printf("%d, Demolition Engine Working!\n", n);
}

#include "Demolition_Console.h"
#include "Demolition_UI.h"
#include "Demolition_Visual.h"
#endif