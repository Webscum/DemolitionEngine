#ifndef DEMOLITION_ENGINE_H
#define DEMOLITION_ENGINE_H
#include <SDL.h>
#include <SDL_image.h>
#include <time.h>
#include <stdio.h>
#include "CVector.h"

#define DEMOLITION_DEFAULT_TEXTURE "Resources/DefaultTexture.png"
#define DEMOLITION_MISSING_TEXTURE "Resources/MissingTexture.png"

int n = 100;
vector objectSpace;
SDL_Window* engineWindow;
SDL_Renderer* engineRenderer;
SDL_Texture* defaultTexture;
SDL_Texture* missingTexture;
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

// Not tested so when you can:
void addAttribute(spaceObject* sObj, char* name){
	
	if(!(getObjectAttribute(&sObj, name))) {
		printf("This object already has that\n");
		return;
	}
	
	objectAttribute* attribute;
	switch(name){
		case "RenderSurface":
			renderSurface* rSurf;
			attribute = (objectAttribute*) malloc((sizeof(void*) + sizeof(name))/ 8);
			rSurf = (renderSurface*) malloc(sizeof(renderSurface));
			// assign all values
			
			attribute->attributeType = name;
			vectorAdd(&sObj->attributes, attribute);
			printf("Render Surface Added!\n");
			break;
	}
	
	vectorAdd(&sObj->attributes, (void*)attribute);
}

void* makeObject(){
	printf("createObject!\n");
	spaceObject* spcObj;
	spcObj = (spaceObject*) malloc(sizeof(spaceObject) / 8);
	vector_init(&spcObj->attributes);
	
	addAttribute(spcObj, "RenderSurface")
	
	objectSpace.pfVectorAdd(&objectSpace, (void*) spcObj);
	return vectorGet(&objectSpace, vectorTotal(&objectSpace) - 1);
}

void trackfps(time_t begin) {
	static int counter;
	
	time_t end;
	time(&end);
	counter++;
	if(counter => 10){
		printf("%d\n", 1000 % (int)(difftime(end, begin) * 1000));
	}
}

// Very messy to put headers seperately and in middle of code, should be fixed
#include "Demolition_UI.h"
#include "Demolition_Visual.h"

// standard definitions and initilizations
void Demolish(int winW, int winH){
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
	
	demolitionLog.type = DEMOLITION_LOG;
	demolitionLog.size = (SDL_Rect){0, winH - 200, winW - 200, 200};
					
	demolitionInterface.type = DEMOLITION_INTERFACE;
	demolitionInterface.size = (SDL_Rect){winW - 200, 0, 200, winH};
						  
	
	printf("Demolition Engine Working!\n");
}

#endif