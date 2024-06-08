#ifndef DEMOLITION_ENGINE_H
#define DEMOLITION_ENGINE_H
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_render.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "CVector.h"

// Definitions for the addresses of the missing and default textures
#define DEMOLITION_DEFAULT_TEXTURE "Resources/DefaultTexture.png"
#define DEMOLITION_MISSING_TEXTURE "Resources/MissingTexture.png"

int framerate;
vector objectSpace;
SDL_Window* engineWindow;
SDL_Renderer* engineRenderer;
SDL_Texture* defaultTexture;
SDL_Texture* missingTexture;

// Here so they can be called and makes the users life easier and my code slicker
// They are just the indicies that the objects attributes are at in the array containing all the object attributes
uint8_t TEXTURE_INDEX;
uint8_t SURFACE_INDEX;
uint8_t ANIMATION_INDEX;

struct spaceObjectVar;

// Made so different renderers can be used for different projects
typedef enum demolition_renderer{
	STANDARD_2D,
	STANDARD_3D,
	VULKAN,
} demolition_renderer;

typedef enum demolition_object_attribute_type{
	SURFACE_ATTRIBUTE,
	TEXTURE_ATTRIBUTE,
	ANIMATION_ATTRIBUTE,
} demolition_objAttrType;

// Moved from Demoliton_UI, defines a clickable rectangle
typedef struct{
	SDL_Rect dimensions;
	void* (*onMouse1) ();
} clickable;

// Space object and Attributes defenition begins here
typedef struct spaceObjectVar{
	uint16_t objIdent; // value in hexadecimal beacause easier to identify
	double x, y, z;
	vector attributes;
	vector children;
	struct spaceObjectVar* parent;
	uint16_t objectFlag;
} spaceObject;

typedef struct objectTextureAttribute{
	SDL_Texture* tex;
	char* textureLocation;
	uint8_t textureFlag;
} texAttr;

typedef struct{
	clickable area;
	//Clickable is used for rendering to the clickable area
	uint8_t renderFlag;
	// renderFlag for user customization
}renderSurface;

typedef struct{
	demolition_objAttrType attributeType;
	uint8_t typeID;
	void* attribute;
	uint16_t bitFlag;
	//Dumb to have an attribute have an attribute inside it
	//But it helps with modularity and reusablity so it is good design imo
	//The bitFlag is for user customization
}objectAttribute;

typedef struct object_Attribute_Type_ID{
	uint8_t ID;
	void* (*addFunc) (spaceObject* sObj, uint8_t type);
	void (*freeFunc) (objectAttribute* attrObj);
} objAttrTypeID;

objAttrTypeID* objectAttributeTypes[128];

void* defaultClick(){
	printf("defaultClick!");
	return NULL;
}

int createObjAttribute(void* (*addFunc) (spaceObject* sObj, uint8_t type), void (*freeFunc) (objectAttribute* attrObj)){
	static uint8_t attributeAmount;

	if(attributeAmount >= 255){
		printf("There are too many attributes defined!");
		return 255;
	}
	objAttrTypeID* objATID = (objAttrTypeID*) malloc(sizeof(objAttrTypeID));
	*objATID = (objAttrTypeID) {attributeAmount, addFunc, freeFunc};
	objectAttributeTypes[attributeAmount] = objATID;
	attributeAmount++;
	return attributeAmount - 1;
}

objectAttribute* getObjectAttribute(spaceObject* obj, uint8_t type){
	for(int i = 0; vectorTotal(&obj->attributes) > i; i++){
		objectAttribute* objAttr = (objectAttribute*) vectorGet(&obj->attributes, i);
		if(objAttr->typeID == type) return objAttr;
	}
	return NULL;
}

objectAttribute* getObjectAttributeFromObjectSpace(int indexOfObject, uint8_t type){
	return getObjectAttribute((spaceObject*) vectorGet(&objectSpace, indexOfObject), type);
}

// Here are the add and free functions for the general attributes,the attribute definitions are in the demolish() function

void* addTexture(spaceObject* sObj, uint8_t type){
	objectAttribute* attr; 
	texAttr* tAttr;

	attr = (objectAttribute*) malloc(sizeof(objectAttribute));
	tAttr = (texAttr*) malloc(sizeof(texAttr));

	tAttr->tex = defaultTexture;
	//strcpy(tAttr->textureLocation, DEMOLITION_DEFAULT_TEXTURE);
	attr->typeID = type;
	attr->attribute = (void*) tAttr;

	vectorPushBack(&sObj->attributes, attr);

	printf("Texture Added!\n");

	return (void*) tAttr;
}

void* addObjectAttribute(spaceObject *sObj, uint8_t type);

void freeTexture(objectAttribute* objAttr){
	texAttr* realAttribute = (texAttr*) objAttr->attribute;
	if (strcmp(realAttribute->textureLocation, DEMOLITION_DEFAULT_TEXTURE) && strcmp(realAttribute->textureLocation, DEMOLITION_MISSING_TEXTURE)) SDL_DestroyTexture(realAttribute->tex);
	free(realAttribute);
	free(objAttr->attribute);
}

void* addSurface(spaceObject* sObj, uint8_t type){
	static int increase;
	increase++;

	objectAttribute* attr;
	renderSurface* rSurf;

	attr = (objectAttribute*) malloc(sizeof(objectAttribute));
	rSurf = (renderSurface*) malloc(sizeof(renderSurface));

	rSurf->area.dimensions = (SDL_Rect){(int)sObj->x, (int)sObj->y+(100*increase), 100, 100};
	rSurf->area.onMouse1 = defaultClick;
	attr->typeID = type;
	attr->attribute = rSurf;
	if(!getObjectAttribute(sObj, TEXTURE_INDEX)) addObjectAttribute(sObj, TEXTURE_INDEX);
	vectorPushBack(&sObj->attributes, attr);

	printf("Render Surface Added!\n");

	return (void*) rSurf;
}

void freeSurface(objectAttribute* objAttr){
	renderSurface* realAttribute = (renderSurface*)objAttr->attribute;
	free(realAttribute);
	free(objAttr->attribute);
}

void freeObjectAttribute(objectAttribute* attr){
	objectAttributeTypes[attr->typeID]->freeFunc(attr);
}

void* addObjectAttribute(spaceObject* sObj, uint8_t type){
	printf("Add object index: %i \n", type);
	return objectAttributeTypes[type]->addFunc(sObj, type);
}

void demolishObject(spaceObject* sObj);

void freeChildren(spaceObject* sObj){
	for (int i = 0; i < vectorTotal(&sObj->children); i++){
		spaceObject* child = (spaceObject*)vectorGet(&sObj->children,i);
		demolishObject(child);
	}
}

void demolishObject(spaceObject* sObj){
	for(int i = 0; vectorTotal(&sObj->attributes) > i; i++){
		objectAttribute* objAttr = (objectAttribute*) vectorGet(&sObj->attributes, i);
		freeObjectAttribute(objAttr);
	}
	freeChildren(sObj);
	free(sObj);
}

// to set attributes of objects
void setAttribute(spaceObject* sObj, demolition_objAttrType type, void* newAttr){
	objectAttribute* objAttr = getObjectAttribute(sObj, type);
	freeObjectAttribute(objAttr);
	objAttr->attribute = newAttr;
}

void* makeObject(){
	spaceObject* spcObj;
	spcObj = (spaceObject*) malloc(sizeof(spaceObject));
	vector_init(&spcObj->attributes); 
	addObjectAttribute(spcObj, SURFACE_INDEX);
	objectSpace.pfVectorAdd(&objectSpace, (void*) spcObj);
	printf("createObject!\n");
	return vectorGet(&objectSpace, vectorTotal(&objectSpace) - 1);
}

void demolish(int winW, int winH, int fps){
	// Setting the window and renderer
	engineWindow = SDL_CreateWindow("Demolition Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winW, winH, 0);
	
	u_int32_t render_flags = SDL_RENDERER_ACCELERATED;
	engineRenderer = SDL_CreateRenderer(engineWindow, -1, render_flags);
	
	vector_init(&objectSpace);

	TEXTURE_INDEX = createObjAttribute(addTexture, freeTexture);
	SURFACE_INDEX = createObjAttribute(addSurface, freeSurface);

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

	framerate = fps;
	
	printf("Demolition Engine Working!\n");
	return;
}

void demolitionStop(){
	printf("Stop the demolition!!!\n");
	return;
}


#endif