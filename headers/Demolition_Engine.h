#ifndef DEMOLITION_ENGINE_H
#define DEMOLITION_ENGINE_H
#include "CVector.h"
#include <SDL.h>
#include <SDL_image.h>

int n = 100;

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

void initSpaceObject(spaceObject* spcObj){
	vector_init(&spcObj->attributes);
}

void addAttribute(spaceObject* sObj, void* attribute){
	sObj->attributes.pfVectorAdd(&sObj->attributes, attribute);
}

void Demolish(){
	
	
	printf("%d, Demolition Engine Working!\n", n);
}

#include "Demolition_UI.h"
#include "Demolition_Visual.h"
#endif