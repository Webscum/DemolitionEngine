#ifndef DEMOLITION_ENGINE_H
#define DEMOLITION_ENGINE_H
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>
#include <stdio.h>
#include "CVector.h"

#define DEMOLITION_DEFAULT_TEXTURE "Resources/DefaultTexture.png"

int n = 100;
vector objectSpace;
SDL_Window* engineWindow;
SDL_Renderer* engineRenderer;
SDL_Texture* defaultTexture;

struct spaceObject;

// Made so different renderers can be used for different projects
typedef enum demolition_renderer{
	STANDARD_2D,
	STANDARD_3D,
	VULKAN
} demolition_renderer;

// Moved from Demoliton_UI, defines a clickable rectangle
typedef struct{
	SDL_Rect dimensions;
	void* (*onMouse1) ();
} clickable;

// Space object and Attributes defenition begins here
typedef struct {
	double x, y, z;
	vector attributes;
} spaceObject;

typedef struct{
	SDL_Texture* tex;
	// 120 just so there is enough space for the image address completely 
	char image[120];
} texAttr;

typedef struct{
	clickable area;
}renderSurface;

typedef struct{
	char* attributeType;
	void* attribute;
}objectAttribute;

// Mush code, gets the attribute of an spaceObject by name returns NULL pointer if there is no attribute of that name
objectAttribute* getObjectAttribute(spaceObject* obj, char* typeName){
	for(int i = 0; vectorTotal(&obj->attributes) > i; i++){
		objectAttribute* objAttr = (objectAttribute*) vectorGet(&obj->attributes, i);
		if(objAttr->attributeType == typeName) return objAttr;
	}
	return NULL;
}

void* makeObject(){
	printf("createObject!\n");
	spaceObject spcObj;
	vector_init(&spcObj.attributes);
	objectSpace.pfVectorAdd(&objectSpace, (void*) &spcObj);
	return vectorGet(&objectSpace, vectorTotal(&objectSpace) - 1);
}

void addAttribute(spaceObject* sObj, void* attribute){
	sObj->attributes.pfVectorAdd(&sObj->attributes, attribute);
}

void trackfps(time_t begin) {
    static int framerate;
    static double timeTracker;
	
	time_t end;
	time(&end);	
	
	timeTracker += difftime(end, begin);
	++framerate;
	
    if (timeTracker >= 1.0) {
        printf("FPS: %d\n", framerate);
        timeTracker -= 1;
        framerate = 0;
    }
}

void Demolish(int winW, int winH){
	// Setting the window and renderer
	engineWindow = SDL_CreateWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winW, winH, 0);

	Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	engineRenderer = SDL_CreateRenderer(engineWindow, 1, render_flags);
	
	VECTOR_INIT(objectSpace);

	//Setting Default Texture Value, could have used initTex but Demolition_Visual isn't compiled yet
	SDL_Surface* surface;
	surface = IMG_Load("Resources/DefaultTexture.png");
	defaultTexture = SDL_CreateTextureFromSurface(engineRenderer, surface);
	
	//Setting the window icon
	surface = IMG_Load("Resources/AppIcon.png");
	SDL_SetWindowIcon(engineWindow, surface);
	SDL_FreeSurface(surface);
	
	printf("Demolition Engine Working!\n");
}

#include "Demolition_UI.h"
#include "Demolition_Visual.h"
#endif