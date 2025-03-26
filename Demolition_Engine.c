//#include <cstdint>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_timer.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "headers/CVector.h"
#include "headers/Demolition_Physics.h"
#include "headers/Demolition_Engine.h"
#include "headers/Demolition_UI.h"
#include "headers/Demolition_Visual.h"

const char DEMOLITION_DEFAULT_TEXTURE[] = "Resources/DefaultTexture.png";
const char DEMOLITION_MISSING_TEXTURE[] = "Resources/MissingTexture.png";

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

objAttrTypeID* objectAttributeTypes[128];

int main(int argc, char* argv[]){

	char* inputFile = argc > 4 ? argv[1] : NULL;
	if(!(inputFile)){
		printf("Not enough arguments!\n");
		return 1;
	}
	
	FILE* file = fopen(inputFile, "r+");
	
	if (!file){
		perror ("Error opening file\n");
		return 1; 
	}


	demolish(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
	initPhysics();

	bool exiting = false;	
	float UpdateFreq  =  1000.0 / framerate;
	uint64_t begin = SDL_GetTicks64();

	setup(begin);

	demolition_button objectCreateButton = {
		{{300, 20, 100, 100}, makeObject},
		{defaultTexture, DEMOLITION_DEFAULT_TEXTURE}
	};

	spaceObject* animationTest = (spaceObject*) makeObject(&objectSpace);

	animationAttribute* animAttr = (animationAttribute*) addObjectAttribute(animationTest, ANIMATION_INDEX);

	uint16_t corners[2] = {0,0};
	uint16_t frameSize[2] = {1, 1};

	createAnimations(animationTest, "Resources/SpriteSheet.png", corners, frameSize, 3, (uint16_t[][2]) {{6, 100}, {7, 100}, {3, 100}});

	RB_2D* rigidbodyAttr = (RB_2D*) addObjectAttribute(animationTest, RIGIDBODY_INDEX);
	rigidbodyAttr->forceVector = (gVec2D){.1, .1};

	//queryButtonTex(&objectCreateButton);
	while(!exiting){
		
		begin = SDL_GetTicks64();
		bool attributes = false;
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:{
					exiting = 1;
					break;
				}
				case SDL_MOUSEBUTTONDOWN:{
					void* funcParams[3] =  {(void*) &objectSpace, NULL, NULL};
					printf("%d\n", clicked(&objectCreateButton.c, funcParams, &event.button, engineWindow));
					checkClicks(&event.button);
					printf("\n");
					break;
				}
			}



		}

		physicsTick();

		logicUpdate(begin);

		SDL_RenderClear(engineRenderer);
		RenderScene();
		//SDL_Rect rect = (SDL_Rect){32, 32, 600, 600};
		//renderSingleTexture( getTextureAttribute(animationTest)->tex, &rect);

		//move(animationTest, (uint16_t[2]) {500, 500});
		RenderButton(&objectCreateButton);
		//RenderDemolitionUI();
		SDL_RenderPresent(engineRenderer);
		//SDL_Delay(UpdateFreq);
		//trackfps(begin);
	}

	quit();
	stopDemolition();
}

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

vector loadedAudios;
float masterVolume;
int lastAudioChannel;
int lastConstantAudioChannel;
const int constantChannels = 127;

int  loadAudio(char* audioFile){
	Mix_Chunk* loadedWav = Mix_LoadWAV(audioFile);
	return vectorPushBack(&loadedAudios, (void *)loadedWav);
}

int loadToAudio(Mix_Chunk* wav, char* audioFile){
	wav = Mix_LoadWAV(audioFile);
	return vectorPushBack(&loadedAudios, (void *)wav);
}

Mix_Chunk* getAudio(int index){
	return (Mix_Chunk*) vectorGet(&loadedAudios, index);
}

int playAudio(char *audioFile, demolition_aud_tp audioType){
	switch (audioType) {
		case DEMOLITION_GLOBAL_AUDIO:
		case DEMOLITION_LOCAL_AUDIO:
		{
			Mix_Chunk* loadedWav;
			int indx = loadToAudio(loadedWav, audioFile);
			playAudioFromIndex(indx, audioType);
			break;
		}
		default:
			printf("Given audio type is undefined\n");
			break;
	}
}

int playAudioConstant(char* audioFile, demolition_aud_tp audioType){
	switch (audioType) {
		case DEMOLITION_GLOBAL_AUDIO:
		case DEMOLITION_LOCAL_AUDIO:
		{
			Mix_Chunk* loadedWav;
			int indx = loadToAudio(loadedWav, audioFile);
			playAudioConstantFromIndex(indx, audioType);
			break;
		}
		default:
			printf("Given audio type is undefined\n");
			break;
	}
}

int playRepeatedAudio(char *audioFile, demolition_aud_tp audioType, int repeated){
	switch (audioType) {
		case DEMOLITION_GLOBAL_AUDIO:
		case DEMOLITION_LOCAL_AUDIO:
		{
			int indx = loadAudio(audioFile);
			playRepeatedAudioFromIndex(indx, audioType, repeated);
			return indx;
			break;
		}
		default:
			printf("Given audio type is undefined\n");
			break;
	}
}

void playAudioConstantFromIndex(int index, demolition_aud_tp audioType){
	switch (audioType) {
		case DEMOLITION_GLOBAL_AUDIO:
		case DEMOLITION_LOCAL_AUDIO:
		{
			Mix_Chunk* loadedWav = getAudio(index);
			for(;Mix_Playing(lastConstantAudioChannel); lastConstantAudioChannel++){}
			
			Mix_PlayChannel(lastConstantAudioChannel % constantChannels, loadedWav, -1);
			Mix_Volume(1, 32);
			break;
		}
		default:
			printf("Given audio type is undefined\n");
			break;
	}
}

void playAudioFromIndex(int index, demolition_aud_tp audioType){
	switch (audioType) {
		case DEMOLITION_GLOBAL_AUDIO:{
			Mix_Chunk* loadedWav = getAudio(index);
			for(;Mix_Playing(lastAudioChannel);lastAudioChannel++){}
			Mix_PlayChannel(lastAudioChannel + constantChannels, loadedWav, 0);
			Mix_Volume(1, 32);
			break;
		}
		default:
			printf("Given audio type is undefined\n");
			break;
	}
}

void playRepeatedAudioFromIndex(int index, demolition_aud_tp audioType, int repeats){
	switch (audioType) {
		case DEMOLITION_GLOBAL_AUDIO:{
			Mix_Chunk* loadedWav = getAudio(index);
			for(;Mix_Playing(lastAudioChannel);lastAudioChannel++){}
			Mix_PlayChannel(lastAudioChannel++ + constantChannels, loadedWav, repeats);
			Mix_Volume(1, 32);
			break;
		}
		default:
			printf("Given audio type is undefined\n");
			break;
	}
}

void setChannelVolume(int channel, float volume){
	if(volume >= 0.0) Mix_Volume(1, 255*volume*masterVolume);
	else printf("Given volume was negative!\n");
}

void setChannelRangeVolume(int firstChannel, int lastChannel, float volume){
	for(int i = firstChannel; i <= lastChannel; i++){
		setChannelVolume(i, volume);
	}
}

void setMasterVolume(float volume){
	if(masterVolume >= 0.0) masterVolume = volume;
	else printf("Given master volume was negative!\n");
}

void stopAudio(int channel){
	if(channel >= 0) Mix_Pause(channel);
}

void stopAllAudios() {
	Mix_Pause(-1);
}

void startAudio(int channel){
	Mix_Resume(channel);
}

void startAllAudios(){
	Mix_Resume(-1);
}

void startAudioRange(int firstChannel, int lastChannel){
	for(int i = firstChannel; i < lastChannel; i++){
		startAudio(i);
	}
}

void stopAudioRange(int firstChannel, int lastChannel){
	for(int i = firstChannel; i < lastChannel; i++){
		stopAudio(i);
	}
}

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

uint8_t createObjAttribute(void* (*addFunc) (spaceObject* sObj, uint8_t type), void (*freeFunc) (spaceObject* attrObj)){
	static uint8_t attributeAmount;

	if(attributeAmount >= 255){
		printf("There are too many attributes defined!");
		return 255;
	}
	objAttrTypeID* objATID = (objAttrTypeID*) malloc(sizeof(objAttrTypeID));
	*objATID = (objAttrTypeID) {attributeAmount, addFunc, freeFunc};
	objectAttributeTypes[attributeAmount] = objATID;

	return attributeAmount++;
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

void* addSurface(spaceObject* sObj, uint8_t type){

	objectAttribute* attr;
	renderSurface* rSurf;

	attr = (objectAttribute*) malloc(sizeof(objectAttribute));
	rSurf = (renderSurface*) malloc(sizeof(renderSurface));

	sObj->coordinates.y += (100*vectorTotal(&objectSpace));

	rSurf->area.dimensions = (SDL_Rect){(int)sObj->coordinates.x, (int)sObj->coordinates.y, 100, 100};
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
	renderSurface* rSurf = getSurface(sObj);
	if(rSurf)
		return &((clickable*) &rSurf->area)->dimensions;
	else
		return NULL;
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

void moveSpaceObjectRectangleBy(spaceObject* sObj, gVec2D vec2D ){
	SDL_Rect* objectRect = getRenderSurfaceRect(sObj);
	if(objectRect){
		objectRect->x += vec2D.x;
		objectRect->y += vec2D.y;
	} else {
		printf("render surface rectangle didn't exist or was inaccessible");
	}
}

void moveSpaceObjectRectangleTo(spaceObject* sObj, gVec2D newPos){
	SDL_Rect* objectRect = getRenderSurfaceRect(sObj);
	if(objectRect){
		objectRect->x = newPos.x;
		objectRect->y = newPos.y;
	} else {
		printf("render surface rectangle didn't exist or was inaccessible");
	}
}

void moveSpaceObjectBy(spaceObject* sObj, gVec2D vec2D ){
	moveSpaceObjectRectangleBy(sObj, vec2D);
	addToObjectCoordinates(sObj, vec2D.x, vec2D.y, 0);
}

void moveSpaceObjectTo(spaceObject* sObj, gVec2D newPos){
	moveSpaceObjectRectangleTo(sObj, newPos);
	setObjectCoordinates(sObj, newPos.x, newPos.y, sObj->coordinates.z);
}

void* clickMove(void* sObj){
	moveSpaceObjectBy((spaceObject*) sObj, (gVec2D){20,20});
	return sObj;
}


void demolish(int winW, int winH, int fps){

	SDL_Init(SDL_INIT_EVERYTHING);

	// Setting the window and renderer
	engineWindow = SDL_CreateWindow("Demolition Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winW, winH, 0);
	
	u_int32_t render_flags = SDL_RENDERER_ACCELERATED;
	engineRenderer = SDL_CreateRenderer(engineWindow, -1, render_flags);

	// Set renderer background
	SDL_SetRenderDrawColor(engineRenderer, 32, 32, 32, 255);


	vector_init(&loadedAudios);
	Mix_Init(MIX_INIT_MP3 | MIX_INIT_WAVPACK);
	// There are 64 channels reserved per channel group and there are 8 channel groups
	// a channel is not required to be in a channel group
	Mix_OpenAudio(48000, AUDIO_F32SYS, 32*8, 2048);
	Mix_Chunk* startupSound = Mix_LoadWAV("Resources/Startup.wav");
	Mix_Chunk* laserShoot = Mix_LoadWAV("Resources/laserShoot.wav");
	Mix_PlayChannel(1, startupSound, 0);
	Mix_PlayChannel(2, laserShoot, 0);
	Mix_Volume(1, 32);

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
	surface = IMG_Load("Resources/dissolveLogo.png");
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