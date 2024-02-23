#ifndef DEMOLITION_ENGINE_H
#define DEMOLITION_ENGINE_H
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include "CVector.h"

#define DEMOLITION_DEFAULT_TEXTURE "Resources/DefaultTexture.png"
#define DEMOLITION_MISSING_TEXTURE "Resources/MissingTexture.png"

int framerate;
vector objectSpace;
SDL_Window* engineWindow;
SDL_Renderer* engineRenderer;
SDL_Texture* defaultTexture;
SDL_Texture* missingTexture;

// Made so different renderers can be used for different projects
typedef enum demolition_renderer{
	STANDARD_2D,
	STANDARD_3D,
	VULKAN
} demolition_renderer;

typedef enum demolition_object_attribute_type{
	RENDER_SURFACE_ATTRIBUTE,
	TEXTURE_ATTRIBUTE,

} demolition_objAttrType;

// Moved from Demoliton_UI, defines a clickable rectangle
typedef struct{
	SDL_Rect dimensions;
	void* (*onMouse1) ();
} clickable;

// Space object and Attributes defenition begins here
typedef struct {
	double x, y, z;
	vector attributes;
	vector children;
	void* parent;
} spaceObject;

typedef struct{
	SDL_Texture* tex;
	char image[80];
	// 80 so it can fit the address completely if it happens to be long and when saving the address we cannot know how long it might be
} texAttr;

typedef struct{
	clickable area;
}renderSurface;

typedef struct{
	demolition_objAttrType attributeType;
	void* attribute;
	u_int8_t bitFlag;
	//Dumb to have an attribute have an attribute inside it
	//But it helps with modularity and reusablity so it is good design imo
	//The bitFlag is for user customization
}objectAttribute;

void* defaultClick(){
	printf("clicked button!\n");
	return NULL;
}

// Mush code, gets the attribute of an spaceObject by name returns NULL pointer if there is no attribute of that name
objectAttribute* getObjectAttribute(spaceObject* obj, demolition_objAttrType typeName){
	for(int i = 0; vectorTotal(&obj->attributes) > i; i++){
		objectAttribute* objAttr = (objectAttribute*) vectorGet(&obj->attributes, i);
		if(objAttr->attributeType == typeName) return objAttr;
	}
	return NULL;
}

objectAttribute* getObjectAttributeFromObjectSpace(int indexOfObject, demolition_objAttrType objectAttributeType){
	return getObjectAttribute((spaceObject*) vectorGet(&objectSpace, indexOfObject), objectAttributeType);
}

// Not tested, found a logic error
void addAttribute(spaceObject* sObj, demolition_objAttrType attrType){
	if(getObjectAttribute(sObj, attrType)) {
		printf("This object already has that\n");
		return;
	}
	objectAttribute* attr;
	switch(attrType){
		case RENDER_SURFACE_ATTRIBUTE:{
			renderSurface* rSurf;
			attr = (objectAttribute*) malloc((sizeof(void*) + sizeof(demolition_objAttrType))/ 8);
			rSurf = (renderSurface*) malloc(sizeof(renderSurface));
			rSurf->area.dimensions = (SDL_Rect){(int)sObj->x, (int)sObj->y, 100, 100};
			rSurf->area.onMouse1 = defaultClick;
			attr->attributeType = attrType;
			attr->attribute = rSurf;
			vectorPushBack(&sObj->attributes, attr);
			printf("Render Surface Added!\n");
			break;
		}
		default:{
			printf("something went wrong, check if the given attribute type was correct!\n");
			return;
		}
	}
	vectorPushBack(&sObj->attributes, (void*)attr);
	printf("addAttribute!\n");
	return;
}

void* makeObject(){
	spaceObject* spcObj;
	spcObj = (spaceObject*) malloc(sizeof(spaceObject) / 8);
	vector_init(&spcObj->attributes);

	addAttribute(spcObj, RENDER_SURFACE_ATTRIBUTE);
	objectSpace.pfVectorAdd(&objectSpace, (void*) spcObj);
	printf("createObject!\n");
	return vectorGet(&objectSpace, vectorTotal(&objectSpace) - 1);
}

void trackfps(u_int64_t begin) {
	static int counter;

	counter++;
	if(counter >= framerate){
		u_int64_t n = (SDL_GetTicks64() - begin);
		u_int64_t t = 1000 / n;

		printf("FPS: %lu\n", t);
		counter = 0;
	}
}

// Very messy to put headers seperately and in middle of code, should be fixed
#include "Demolition_UI.h"
#include "Demolition_Visual.h"
#include "Demolition_Physics.h"

// standard definitions and initilizations
void Demolish(int winW, int winH, int fps){
	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}

	// Setting the window and renderer
	engineWindow = SDL_CreateWindow("Demolition Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winW, winH, 0);
	
	Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	engineRenderer = SDL_CreateRenderer(engineWindow, -1, render_flags);
	
	
	vector_init(&objectSpace);

	//Setting Default Texture Value, could have used initTex but Demolition_Visual isn't compiled yet
	SDL_Surface* surface;
	surface = IMG_Load("Resources/DefaultTexture.png");
	defaultTexture = SDL_CreateTextureFromSurface(engineRenderer, surface);
	
	surface = IMG_Load("Resources/MissingTexture.png");
	missingTexture = SDL_CreateTextureFromSurface(engineRenderer, surface);
	
	//Setting the window icon
	surface = IMG_Load("Resources/AppIcon.png");
	SDL_SetWindowIcon(engineWindow, surface);
	SDL_FreeSurface(surface);
	
	srand(time(NULL));
	
	demolitionLog.type = DEMOLITION_LOG;
	demolitionLog.size = (SDL_Rect){0, winH - 200, winW - 200, 200};
					
	demolitionInterface.type = DEMOLITION_INTERFACE;
	demolitionInterface.size = (SDL_Rect){winW - 200, 0, 200, winH};

	framerate = fps;
	
	printf("Demolition Engine Working!\n");
}

#endif