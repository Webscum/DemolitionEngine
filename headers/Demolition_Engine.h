#ifndef DEMOLITION_ENGINE_H
#define DEMOLITION_ENGINE_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
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
#include <limits.h>
#include <SDL2/SDL_audio.h>

// Definitions for the addresses of the missing and default textures
char DEMOLITION_DEFAULT_TEXTURE[] = "Resources/DefaultTexture.png";
char DEMOLITION_MISSING_TEXTURE[] = "Resources/MissingTexture.png";

int framerate;
vector objectSpace;
SDL_Window* engineWindow;
SDL_Renderer* engineRenderer;
SDL_Texture* defaultTexture;
SDL_Texture* missingTexture;
SDL_AudioDeviceID engineAudio;
SDL_AudioSpec* engineAudioSpec; 

// Here so they can be called and makes the users life easier and my code slicker
// They are just the indicies that the objects attributes are at in the array containing all the object attributes
uint8_t TEXTURE_INDEX;
uint8_t SURFACE_INDEX;
uint8_t ANIMATION_INDEX;

struct spaceObjectVar;

void putb(unsigned long long n)
{
    char b[(sizeof n * CHAR_BIT) + 1];
    char *p = b + sizeof b;
    *--p = '\0';
    for (; p-- > b;  n >>= 1) {
        *p = '0' + (char)(n & 1);
    }
    puts(b);
}

int timeSince(int origin){
	return SDL_GetTicks64() - origin;
}

void switch16BitFlagBit(uint16_t* flag, uint8_t index){
	if(index < 16) *flag |= 1 << index;
}

void switch8BitFlagBit(uint8_t* flag, uint8_t index){
	if(index < 8) *flag |= 1 << index;
}

void switch16BitFlagBits(uint16_t* flag, uint8_t indiciesLength, uint8_t indicies[]){
	if (indicies){
		for(int iteration = 0; iteration < indiciesLength; iteration++){
			switch16BitFlagBit(flag, indicies[iteration]);
		}
	}
	else{
		printf("No indicies provided");
	}
}

void switch8BitFlagBits(uint8_t* flag, uint8_t indiciesLength, uint8_t indicies[]){
	if (indicies){
		for(int iteration = 0; iteration < indiciesLength; iteration++){
			switch8BitFlagBit(flag, indicies[iteration]);
		}
	}
	else{
		printf("No indicies provided");
	}
}


// Different renderers can be used for different projects and the user can define their own
typedef enum demolition_renderer{
	STANDARD_2D,
	STANDARD_3D,
	VULKAN,
	CUSTOM,
} demolition_renderer;
demolition_renderer selectedRenderer;


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
	void* (*onMouse2) (void*);
} clickable;

SDL_Rect getClickableRect(clickable* c){
	return c->dimensions;
}

void setClickableRect(clickable* c, SDL_Rect r){
	c->dimensions = r;
}

void  addToClickableRect(clickable* c, SDL_Rect r){
	SDL_Rect cr = c->dimensions;
	cr.x += r.x;
	cr.y += r.y;
	cr.w += r.w;
	cr.h += r.h;
}

typedef struct geometryVector2D{
	double x, y;
} gVec2D;

typedef struct geometryVector3D{
	double x, y, z;
} gVec3D;

// Space object and Attributes defenition here
typedef struct spaceObjectVar{
	uint16_t objIdent; // Display value in hexadecimal beacause easier to identify
	gVec3D coordinates;
	vector attributes;
	vector children;
	struct spaceObjectVar* parent;
	uint16_t objectFlag;
} spaceObject;

// BEGIN HERE!
gVec3D* getObjectCoordinates(spaceObject* sObj){
	return  &sObj->coordinates;
}

void setObjectCoordinates(spaceObject* sObj, double x, double y, double z){
	gVec3D* objectCoordinates = &sObj->coordinates;
	objectCoordinates->x = x;
	objectCoordinates->y = y;
	objectCoordinates->z = z;
}

void addToObjectCoordinates(spaceObject* sObj, double x, double y, double z){
	gVec3D* objectCoordinates = &sObj->coordinates;
	objectCoordinates->x += x;
	objectCoordinates->y += y;
	objectCoordinates->z += z;
}

void setObjectCoordinatesWithVector(spaceObject* sObj, gVec3D newCoords){
	sObj->coordinates = newCoords;
}

void addToObjectCoordinatesWithVector(spaceObject* sObj, gVec3D newCoords){
	addToObjectCoordinates(sObj, newCoords.x, newCoords.y, newCoords.z);
}

void swapObjectCoordinates(spaceObject* firstObj, spaceObject* secondObj){
	gVec3D firstCoords = firstObj->coordinates;
	firstObj->coordinates = secondObj->coordinates;
	secondObj->coordinates = firstCoords;
}

void setObjectFlag(spaceObject* sObj, uint16_t newFlag){
	sObj->objectFlag = newFlag;
}

void switchObjectFlagBit(spaceObject* sObj, uint8_t index){
	switch16BitFlagBit(&sObj->objectFlag, index);
}

void switchObjectFlagBits(spaceObject* sObj, uint8_t indiciesLength, uint8_t indicies[]){
	switch16BitFlagBits(&sObj->objectFlag, indiciesLength, indicies);
}

uint16_t getObjectFlag(spaceObject* sObj){
	return sObj->objectFlag;
}

typedef struct objectTextureAttribute{
	SDL_Texture* tex;
	char* textureLocation;
	uint8_t textureFlag;
} texAttr;

void setTextureLocation(texAttr* tAttr, char* newLocation){
	tAttr->textureLocation = newLocation;
}

void setTexture(texAttr* tAttr, SDL_Texture* newTex){
	tAttr->tex = newTex;
}

void switchTextureFlagBit(texAttr* tAttr, uint8_t index){
	switch8BitFlagBit(&tAttr->textureFlag, index);
}

void switchTextureFlagBits(texAttr* tAttr, uint8_t indiciesLength, uint8_t indicies[]){
	switch8BitFlagBits(&tAttr->textureFlag, indiciesLength, indicies);
}

typedef struct{
	clickable area; //Clickable is used for rendering to the clickable area
	uint8_t renderFlag; // renderFlag for user customization
}renderSurface;

clickable* getRenderSurfaceClickable(renderSurface* rSurf){
	return &rSurf->area;
}

void setRenderSurfaceAreaSize(renderSurface* rSurf, SDL_Rect newSize){
	setClickableRect(getRenderSurfaceClickable(rSurf), newSize); 
}

void setRenderFlag(renderSurface* rSurf, uint8_t newFlag){
	rSurf->renderFlag = newFlag;
}

void switchRenderFlag(renderSurface* rSurf, uint8_t index){
	switch8BitFlagBit(&rSurf->renderFlag, index);
}

typedef struct{
	vector frames;
	uint16_t duration;
	bool manual;
	uint8_t currentFrame;
} animation;

typedef struct{
	vector animations;
	uint8_t animationCount;
	int lastSwap;
	uint8_t selectedAnimation;
	uint8_t animationFlag;
} animationAttribute;

uint8_t getAnimationCount(animationAttribute* animAttr){
	return animAttr->animationCount;
}

void setSelectedAnimation(animationAttribute* animAttr, uint8_t newAnim){
	if(newAnim < getAnimationCount(animAttr)){
		((animation*) vectorGet(&animAttr->animations, animAttr->selectedAnimation))->currentFrame = 0;
		animAttr->selectedAnimation = newAnim;
	}
	else{
		printf("Provided animation index not available!\n");
	}
}


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
void* clickMove(void*);

void* addSurface(spaceObject* sObj, uint8_t type){

	objectAttribute* attr;
	renderSurface* rSurf;

	attr = (objectAttribute*) malloc(sizeof(objectAttribute));
	rSurf = (renderSurface*) malloc(sizeof(renderSurface));


	rSurf->area.dimensions = (SDL_Rect){(int)sObj->coordinates.x, (int)sObj->coordinates.y+(100*vectorTotal(&objectSpace)), 100, 100};
	rSurf->area.onMouse1 = clickDemolish;
	rSurf->area.onMouse2 = clickMove;
	attr->typeID = type;
	attr->attribute = (void*) rSurf;
	if(!getObjectAttribute(sObj, TEXTURE_INDEX)) addObjectAttribute(sObj, TEXTURE_INDEX);
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
	return &((clickable*) &getSurface(sObj)->area)->dimensions;
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

void createAnimations(spaceObject* sObj, char* imageLocation, uint16_t cornerClip[2], uint16_t frameSize[2], uint8_t animationAmount,uint16_t animationArray[][2] /*example {{3, .2},{2, .3},{5. 0}} there are 10 frames in total and this is how you get different animations from the same sprite sheet*/){
	SDL_Surface* srcSurface = IMG_Load(imageLocation);
	SDL_Rect clip = {cornerClip[0], cornerClip[1], frameSize[0], frameSize[1]};
	SDL_Rect* destRect = getRenderSurfaceRect(sObj);
	const SDL_Rect ogDestRect = *destRect;
	SDL_Surface* destSurface = SDL_CreateRGBSurface(0, destRect->w, destRect->h, 32, 0, 0, 0, 0);

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
			//printf("frame: %d\n", frameIndex); // Starts to trip out somewhere after here!, fix tomorrow!
			
			destSurface->w = clip.w, destSurface->h = clip.h;
			destRect->w = clip.w, destRect->h = clip.h;
			SDL_BlitSurface(srcSurface, &clip, destSurface, destRect);
			SDL_Texture* surfTex = SDL_CreateTextureFromSurface(engineRenderer, destSurface);
			SDL_QueryTexture(surfTex, NULL, NULL, &destRect->w, &destRect->h);
			destSurface->w = ogDestRect.w, destSurface->h = ogDestRect.h;
			destRect->w = ogDestRect.w, destRect->h = ogDestRect.h;
			vectorPushBack(&anim->frames, (void*) surfTex);

			column++;
			if(column >= framesPerRow){
				row++;
				column %= framesPerRow;
			}

			clip.x = cornerClip[0] + frameSize[0]*column;
			clip.y = cornerClip[1] + frameSize[1]*row;
		}

		if(animationArray[animIndex][1]){
			printf("Animation has Duration\n");
			anim->duration = animationArray[animIndex][1];
		}
		else{
			printf("Animation is Manual\n");
			anim->manual = true;
		}
	}
	SDL_FreeSurface(destSurface);
	SDL_FreeSurface(srcSurface);
	animAttr->lastSwap = SDL_GetTicks64();
}

void animateObject(animationAttribute* animAttr, texAttr* texture, int time){
	animation* anim = (animation*) vectorGet(&animAttr->animations, animAttr->selectedAnimation);
	if(anim->manual || time - animAttr->lastSwap >= anim->duration){
		texture->tex = (SDL_Texture*) vectorGet(&anim->frames, ++anim->currentFrame % vectorTotal(&anim->frames));
		animAttr->lastSwap = time;
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
	renderSurface* rSurf = (renderSurface*) addObjectAttribute(spcObj, SURFACE_INDEX);
	vectorPushBack(((vector*) space), (void*) spcObj);
	setObjectCoordinates(spcObj, 0.0, 0.0, 1.0);
	printf("createObject!\n");
	return vectorGet(&objectSpace, vectorTotal(&objectSpace) - 1);
}


void moveBy(spaceObject* sObj, uint16_t vec2D[2] ){
	SDL_Rect* objectRect = getRenderSurfaceRect(sObj);
	printf("Move the object!\n");
	objectRect->x += vec2D[0];
	objectRect->y += vec2D[1];
}

void moveTo(spaceObject* sObj, uint16_t newPos[2]){
	SDL_Rect* objectRect = getRenderSurfaceRect(sObj);
	printf("Move the object!\n");
	objectRect->x = newPos[0];
	objectRect->y = newPos[1];
}

void* clickMove(void* sObj){
	moveBy((spaceObject*) sObj, (uint16_t[2]) {20,20});
	return NULL;
}



void demolish(int winW, int winH, int fps){

	SDL_Init(SDL_INIT_EVERYTHING);

	// Setting the window and renderer
	engineWindow = SDL_CreateWindow("Demolition Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winW, winH, 0);
	
	u_int32_t render_flags = SDL_RENDERER_ACCELERATED;
	engineRenderer = SDL_CreateRenderer(engineWindow, -1, render_flags);

	SDL_AudioSpec desiredAudioSpec;
	SDL_AudioSpec obtainedAudioSpec;
	SDL_memset(&obtainedAudioSpec, 0,sizeof(obtainedAudioSpec));
	SDL_memset(&desiredAudioSpec, 0, sizeof(desiredAudioSpec));
	desiredAudioSpec.freq = 46000;
	desiredAudioSpec.format = AUDIO_F32;
	desiredAudioSpec.channels = 2;
	desiredAudioSpec.samples = 4096;
	desiredAudioSpec.callback = NULL;
	engineAudio = SDL_OpenAudioDevice(NULL, 0, &desiredAudioSpec, &obtainedAudioSpec, SDL_AUDIO_ALLOW_ANY_CHANGE);
	engineAudioSpec = (SDL_AudioSpec*) malloc(sizeof(SDL_AudioSpec));
	*engineAudioSpec = obtainedAudioSpec;
	//SDL_AudioInit(engineAudio);

	Uint32 wav_length;
	Uint8 *wav_buffer;


	/* Load the WAV */
	if (SDL_LoadWAV("Resources/Startup.wav", &obtainedAudioSpec, &wav_buffer, &wav_length) == NULL) {
		fprintf(stderr, "Could not open .wav file: %s\n", SDL_GetError());
	} else {

		printf("Opened .wav file, %s\n", SDL_GetAudioDeviceName(engineAudio, false));
		printf("Queue result : %d\n", SDL_QueueAudio(engineAudio, wav_buffer, wav_length));
		printf("Second queue result: %d\n", SDL_QueueAudio(engineAudio, wav_buffer, wav_length));
		SDL_PauseAudioDevice(engineAudio, 1);
	}

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

	//Setting the window icon, and startup logo
	surface = IMG_Load("Resources/AppIcon.png");
	SDL_SetWindowIcon(engineWindow, surface);
	SDL_Texture* startUpTexture = SDL_CreateTextureFromSurface(engineRenderer, surface);
	SDL_FreeSurface(surface); 

	framerate = fps;
	
	printf("Demolition Engine Working!\n");

	int cubeSide = winH < winW ? winH / 4 * 3 : winW / 4 * 3;

	SDL_Rect demolitionStartUpRectangle = {(winW - cubeSide) / 2, (winH - cubeSide) / 2, cubeSide, cubeSide};

	int origin = SDL_GetTicks64();
	

	for(int timeSinceOrigin = timeSince(origin); timeSinceOrigin < 3000; timeSinceOrigin = timeSince(origin)){

		SDL_RenderClear(engineRenderer);

		if(timeSinceOrigin < 1000){
			SDL_SetTextureAlphaMod(startUpTexture, 255 * (timeSinceOrigin/1000.0 + 0.001));
		}
		else{
			SDL_SetTextureAlphaMod(startUpTexture, 255 * (1.0 - ((timeSinceOrigin-1000) / 2000.0)));
		}

		SDL_RenderCopy(engineRenderer, startUpTexture, NULL, &demolitionStartUpRectangle);

		SDL_RenderPresent(engineRenderer);
	}
	
	if(SDL_GetQueuedAudioSize(engineAudio)){
		SDL_ClearQueuedAudio(engineAudio);
		printf("Here!!!\n");
		SDL_FreeWAV(wav_buffer);
	}

	SDL_DestroyTexture(startUpTexture);

	return;
}

void stopDemolition(){
	SDL_DestroyRenderer(engineRenderer);
	SDL_DestroyWindow(engineWindow);
	SDL_Quit();
	printf("Stop the demolition!!!\n");
	return;
}


#endif
