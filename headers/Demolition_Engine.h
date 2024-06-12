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
char DEMOLITION_DEFAULT_TEXTURE[] = "Resources/DefaultTexture.png";
char DEMOLITION_MISSING_TEXTURE[] = "Resources/MissingTexture.png";

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

// Different renderers can be used for different projects and the user can define their own
typedef enum demolition_renderer{
	STANDARD_2D,
	STANDARD_3D,
	VULKAN,
	CUSTOM,
} demolition_renderer;

// Deprecated, don't use
typedef enum demolition_object_attribute_type{
	SURFACE_ATTRIBUTE,
	TEXTURE_ATTRIBUTE,
	ANIMATION_ATTRIBUTE,
} demolition_objAttrType;

// Makes the object clickable and gives it functions
typedef struct{
	SDL_Rect dimensions;
	void* (*onMouse1) (void*);
} clickable;

// Space object and Attributes defenition here
typedef struct spaceObjectVar{
	uint16_t objIdent; // Display value in hexadecimal beacause easier to identify
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
	clickable area; //Clickable is used for rendering to the clickable area
	uint8_t renderFlag; // renderFlag for user customization
}renderSurface;

typedef struct{
	vector frames;
	uint16_t duration;
	bool manual;
	uint8_t currentFrame;
} animation;

typedef struct{
	vector animations;
	uint8_t animationCount;
	uint8_t lastSwap;
	uint8_t selectedAnimation;
	uint8_t animationFlag;
} animationAttribute;

typedef struct{
	demolition_objAttrType attributeType;
	uint8_t typeID;
	void* attribute;
	uint16_t bitFlag; 	// BitFlag for customization
	//Dumb to have an attribute have an attribute inside it
	//But it helps with modularity and reusablity so it is good design imo
}objectAttribute;

typedef struct object_Attribute_Type_ID{
	uint8_t ID;
	void* (*addFunc) (spaceObject* sObj, uint8_t type);
	void (*freeFunc) (spaceObject* spaceObj);
} objAttrTypeID;

objAttrTypeID* objectAttributeTypes[128];


spaceObject* getObjectFromObjectSpace(uint16_t index){
	return (spaceObject*) vectorGet(&objectSpace, index);
}

int deleteFromObjSpace(uint16_t index){
	for(int objIndx = index; objIndx < vectorTotal(&objectSpace); objIndx++){
		((spaceObject*) vectorGet(&objectSpace, objIndx))->objIdent--;
	}
	return vectorDelete(&objectSpace, index);
}

void* defaultClick(){
	printf("defaultClick!");
	return NULL;
}

int createObjAttribute(void* (*addFunc) (spaceObject* sObj, uint8_t type), void (*freeFunc) (spaceObject* attrObj)){
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

objectAttribute* getObjectAttributeFromObjectSpace(uint16_t indexOfObject, uint8_t type){
	return getObjectAttribute(getObjectFromObjectSpace(indexOfObject), type);
}

// Here are the add and free functions for the general attributes,the attribute definitions are in the demolish() function

void* addTexture(spaceObject* sObj, uint8_t type){
	objectAttribute* attr; 
	texAttr* tAttr;

	attr = (objectAttribute*) malloc(sizeof(objectAttribute));
	tAttr = (texAttr*) malloc(sizeof(texAttr));

	tAttr->tex = defaultTexture;
	tAttr->textureLocation = DEMOLITION_DEFAULT_TEXTURE;
	attr->typeID = type;
	attr->attribute = (void*) tAttr;
	
	vectorPushBack(&sObj->attributes, (void*) attr);

	printf("Texture Added!\n");

	return (void*) tAttr;
}

void freeTexture(spaceObject* sObj){
	objectAttribute* objAttr = getObjectAttribute(sObj, TEXTURE_INDEX);
	texAttr* realAttribute = (texAttr*) objAttr->attribute;

	if (strcmp(realAttribute->textureLocation, DEMOLITION_DEFAULT_TEXTURE) && strcmp(realAttribute->textureLocation, DEMOLITION_MISSING_TEXTURE) && !getObjectAttribute(sObj, ANIMATION_INDEX)){
		SDL_DestroyTexture(realAttribute->tex);
	}
	free(realAttribute);
	free(objAttr);
	
}

void* addObjectAttribute(spaceObject *sObj, uint8_t type);
void* clickDemolish(void*);

void* addSurface(spaceObject* sObj, uint8_t type){

	objectAttribute* attr;
	renderSurface* rSurf;

	attr = (objectAttribute*) malloc(sizeof(objectAttribute));
	rSurf = (renderSurface*) malloc(sizeof(renderSurface));


	rSurf->area.dimensions = (SDL_Rect){(int)sObj->x, (int)sObj->y+(100*vectorTotal(&objectSpace)), 100, 100};
	rSurf->area.onMouse1 = clickDemolish;
	attr->typeID = type;
	attr->attribute = (void*) rSurf;
	if(!getObjectAttribute(sObj, TEXTURE_INDEX)) addObjectAttribute(sObj, TEXTURE_INDEX);
	//printf("Hello debugger 0!\n");
	vectorPushBack(&sObj->attributes, attr);

	printf("Render Surface Added!\n");

	return (void*) rSurf;
}

void freeSurface(spaceObject* sObj){
	//renderSurface* realAttribute = (renderSurface*)objAttr->attribute;
	objectAttribute* objAttr= getObjectAttribute(sObj, SURFACE_INDEX);
	free(objAttr->attribute);
	free(objAttr);
}

void* addAnimation(spaceObject* sObj, uint8_t type){
	objectAttribute* attr;
	animationAttribute* anim;
	attr = (objectAttribute*) malloc(sizeof(objectAttribute));
	anim = (animationAttribute*) malloc(sizeof(animationAttribute));

	attr->typeID = type;
	attr->attribute = anim;
	//printf("Hello debugger 0!\n");
	if(!getObjectAttribute(sObj, SURFACE_INDEX)) addObjectAttribute(sObj, SURFACE_INDEX);

	vector_init(&anim->animations);

	vectorPushBack(&sObj->attributes, attr);

	printf("Animation added!\n");

	return (void*) anim;
}

void freeAnimation(spaceObject* sObj){
	objectAttribute* objAttr = getObjectAttribute(sObj, ANIMATION_INDEX);
	animationAttribute* animAttr = (animationAttribute*) objAttr->attribute;
	for(int animationIndex = 0; animationIndex < vectorTotal(&animAttr->animations); animationIndex++){
		animation* anim = (animation*) vectorGet(&animAttr->animations, animationIndex);
		for(int frameIndex = 0; frameIndex < vectorTotal(&anim->frames); frameIndex++){
			SDL_DestroyTexture((SDL_Texture*) vectorGet(&anim->frames, frameIndex));
		}
		vectorFree(&anim->frames);
	}
	vectorFree(&animAttr->animations);
	free(animAttr);
	free(objAttr);
}

void freeObjectAttribute(spaceObject* sObj, uint8_t type){
	printf("Free Attribute %d! \n", type);
	objectAttributeTypes[type]->freeFunc(sObj);
}

void* addObjectAttribute(spaceObject* sObj, uint8_t type){
	printf("Add attribute!\n");
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
	
	if(deleteFromObjSpace(sObj->objIdent) == UNDEFINE){
		printf("Deletion of object unsuccesful!\n");
		return;
	}


	freeChildren(sObj);
	for(int i = 0; vectorTotal(&sObj->attributes) > i; i++){
		objectAttribute* objAttr = (objectAttribute*) vectorGet(&sObj->attributes, i);
		freeObjectAttribute(sObj, objAttr->typeID);
	}
	free(sObj);
}

renderSurface* getSurface(spaceObject* sObj){
	objectAttribute* objAttr = getObjectAttribute(sObj, SURFACE_INDEX);
	if(objAttr){
		return (renderSurface*) objAttr->attribute;
	}
	else{
		return NULL;
	}
}

SDL_Rect* getRenderSurfaceRect(spaceObject* sObj){
	return & ((clickable*) &getSurface(sObj)->area)->dimensions;
}

animationAttribute* getAnimationAttribute(spaceObject* sObj){
	objectAttribute* objAttr = getObjectAttribute(sObj, ANIMATION_INDEX);
	if(objAttr){
		return (animationAttribute*) objAttr->attribute;
	}
	else{
		return NULL;
	}
}

texAttr* getTextureAttribute(spaceObject* sObj){
	objectAttribute* objAttr = getObjectAttribute(sObj, TEXTURE_INDEX);
	if(objAttr){
		return (texAttr*) objAttr->attribute;
	}
	else{
		return NULL;
	}
}

vector* getAnimationsVector(spaceObject* sObj){
	return &(getAnimationAttribute(sObj))->animations;
}

vector* getFramesVector(spaceObject* sObj, int index){
	return &((animation*) vectorGet(getAnimationsVector(sObj), index))->frames;
}

void createAnimations(spaceObject* sObj, char* imageLocation, uint16_t cornerClip[2], uint16_t frameSize[2], uint8_t animationArray[][2] /*example {{3, .2},{2, .3},{5. 0}} there are 10 frames in total and this is how you get different animations from the same sprite sheet*/){
	SDL_Surface* srcSurface = IMG_Load(imageLocation);
	SDL_Rect clip = {cornerClip[0], cornerClip[1], frameSize[0], frameSize[1]};
	SDL_Surface* destSurface;
	SDL_Rect* destRect = getRenderSurfaceRect(sObj);

	uint8_t animationAmount = sizeof(*animationArray) / sizeof(animationArray[0]);
	uint8_t framesPerRow = (srcSurface->w - cornerClip[0] * 2) / frameSize[0];
	uint8_t row = 0;
	uint8_t column = 0;

	animationAttribute* animAttr = getAnimationAttribute(sObj);
	animAttr->animationCount = animationAmount;
	getTextureAttribute(sObj)->textureLocation = imageLocation;




	for (int animIndex= 0; animationAmount > animIndex; animIndex++) {
		animation* anim;
		anim = (animation*) malloc(sizeof(animation));
		vector_init(&anim->frames);
		vectorPushBack(getAnimationsVector(sObj), (void*) anim);
		for (int frameIndex= 0; frameIndex < animationArray[animIndex][0]; frameIndex++) {
			printf("frame: %d\n", frameIndex); // Starts to trip out somewhere after here!, fix tomorrow!
			SDL_BlitSurface(srcSurface, &clip, destSurface, destRect);
			vectorPushBack(getFramesVector(sObj, animIndex), (void*) SDL_CreateTextureFromSurface(engineRenderer, destSurface));

			row++;
			if(row >= framesPerRow){
				column++;
				row %= framesPerRow;
			}

			clip.x = cornerClip[0] + frameSize[0]*row;
			clip.y = cornerClip[1] + frameSize[1]*column;
		}

		if(anim->duration) anim->duration = animationArray[animIndex][1];
		else anim->manual = true;
	}
	animAttr->lastSwap = SDL_GetTicks64();
}

void animateObject(animationAttribute* animAttr, texAttr* texture, int time){
	animation* anim = (animation*) vectorGet(&animAttr->animations, animAttr->selectedAnimation);
	if(anim->manual || time - animAttr->lastSwap >= anim->duration){
		texture->tex = (SDL_Texture*) vectorGet(&anim->frames, ++anim->currentFrame);
		animAttr->lastSwap = SDL_GetTicks64();
	}
}

void setAnimation(spaceObject* sObj, uint8_t selected){
	animationAttribute* animAttr = getAnimationAttribute(sObj);
	if(selected < 0 || selected > animAttr->animationCount){
		printf("non-valid animation index given!");
	}
	else{
		animAttr->selectedAnimation = selected;
	}
}

void* clickDemolish(void* spcObj){
	printf("Click demolish!\n");
	demolishObject((spaceObject*) spcObj);
	return NULL;
}

// to set attributes of objects
void setAttribute(spaceObject* sObj, uint8_t type, void* newAttr){
	objectAttribute* objAttr = getObjectAttribute(sObj, type);
	freeObjectAttribute(sObj, type);
	objAttr->attribute = newAttr;
}

void* makeObject(void* space){
	spaceObject* spcObj;
	spcObj = (spaceObject*) malloc(sizeof(spaceObject));
	spcObj->objIdent = vectorTotal(&objectSpace);
	vector_init(&spcObj->attributes); 
	addObjectAttribute(spcObj, SURFACE_INDEX);
	vectorPushBack(((vector*) space), (void*) spcObj);
	printf("createObject!\n");
	return vectorGet(&objectSpace, vectorTotal(&objectSpace) - 1);
}

void move(spaceObject* sObj, uint16_t newPos[2] ){
	SDL_Rect* objectRect = getRenderSurfaceRect(sObj);
	objectRect->x = newPos[0];
	objectRect->y = newPos[1];
}

void demolish(int winW, int winH, int fps){
	// Setting the window and renderer
	engineWindow = SDL_CreateWindow("Demolition Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winW, winH, 0);
	
	u_int32_t render_flags = SDL_RENDERER_ACCELERATED;
	engineRenderer = SDL_CreateRenderer(engineWindow, -1, render_flags);
	
	vector_init(&objectSpace);

	TEXTURE_INDEX = createObjAttribute(addTexture, freeTexture);
	SURFACE_INDEX = createObjAttribute(addSurface, freeSurface);
	ANIMATION_INDEX = createObjAttribute(addAnimation, freeAnimation);

	//Setting Default Texture Value, could have used initTex but Demolition_Visual isn't compiled yet
	SDL_Surface* surface;
	surface = IMG_Load("Resources/DefaultTexture.png");
	defaultTexture = SDL_CreateTextureFromSurface(engineRenderer, surface);
	SDL_FreeSurface(surface);

	surface = IMG_Load("Resources/MissingTexture.png");
	missingTexture = SDL_CreateTextureFromSurface(engineRenderer, surface);
	SDL_FreeSurface(surface);

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
