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
#include <SDL2/SDL_mixer.h>


// Definitions for the addresses of the missing and default textures
extern const char DEMOLITION_DEFAULT_TEXTURE[];
extern const char DEMOLITION_MISSING_TEXTURE[];

extern int framerate;
extern vector objectSpace;
extern SDL_Window* engineWindow;
extern SDL_Renderer* engineRenderer;
extern SDL_Texture* defaultTexture;
extern SDL_Texture* missingTexture;
extern SDL_AudioDeviceID engineAudio;
extern SDL_AudioSpec* engineAudioSpec; 

// Here so they can be called and makes the users life easier and my code slicker
// They are just the indicies that the objects attributes are at in the array containing all the object attributes
extern uint8_t TEXTURE_INDEX;
extern uint8_t SURFACE_INDEX;
extern uint8_t ANIMATION_INDEX;

extern uint64_t runtime;

extern uint16_t logicFrequency;
extern uint16_t renderFrequency;

extern int main(int argc, char** argv);

extern void* getArg(int argIndx);

extern void setup(uint64_t);
extern void logicUpdate(uint64_t);
extern void renderUpdate(uint64_t);
extern void quit();

extern void putb(unsigned long long n);

extern int timeSince(int origin);

extern void switch16BitFlagBit(uint16_t* flag, uint8_t index);

extern void switch8BitFlagBit(uint8_t* flag, uint8_t index);

extern void switch16BitFlagBits(uint16_t* flag, uint8_t indiciesLength, uint8_t indicies[]);

extern void switch8BitFlagBits(uint8_t* flag, uint8_t indiciesLength, uint8_t indicies[]);


// Different renderers can be used for different projects and the user can define their own
typedef enum demolition_renderer{
	STANDARD_2D,
	STANDARD_3D,
	VULKAN,
	CUSTOM,
} demolition_renderer;
extern demolition_renderer selectedRenderer;


// Deprecated, don't use
typedef enum demolition_object_attribute_type{
	SURFACE_ATTRIBUTE,
	TEXTURE_ATTRIBUTE,
	ANIMATION_ATTRIBUTE,
} demolition_objAttrType;

typedef enum demolition_audio_type{
	DEMOLITION_LOCAL_AUDIO,
	DEMOLITION_GLOBAL_AUDIO
} demolition_aud_tp;

extern vector loadedAudios;
extern float masterVolume;
extern int lastAudioChannel;
extern const int constantChannels;

extern int playAudio(char* audioFile, demolition_aud_tp audioType);
extern int playAudioConstant(char* audioFile, demolition_aud_tp audioType);
extern void playAudioConstantFromIndex(int index, demolition_aud_tp audioType);
extern void playAudioFromIndex(int index, demolition_aud_tp audioType);
extern int loadAudio(char* audioFile);
extern void stopConstantAudio(int channel);
extern void setChannelVolume(int channel, float volume);
extern void setChannelRangeVolume(int firstChannel, int lastChannel, float volume);
extern void setMasterVolume(float volume);
extern void playRepeatedAudioFromIndex(int index, demolition_aud_tp audioType, int repeats);
extern int playRepeatedAudio(char* audioFile, demolition_aud_tp audioType, int repeats);
extern void startAllAudios();
extern void stopAllAudios();
extern void startAudio(int);
extern void stopAudio(int);

// Makes the object clickable and gives it functions
typedef struct{
	SDL_Rect dimensions;
	bool clicked;
	void* (*onMouse1) (void*);
	void* (*onMouse2) (void*);
} clickable;

extern SDL_Rect getClickableRect(clickable* c);

extern void setClickableRect(clickable* c, SDL_Rect r);

extern void  addToClickableRect(clickable* c, SDL_Rect r);

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
extern gVec3D* getObjectCoordinates(spaceObject* sObj);

extern void setObjectCoordinates(spaceObject* sObj, double x, double y, double z);

extern void addToObjectCoordinates(spaceObject* sObj, double x, double y, double z);

extern void setObjectCoordinatesWithVector(spaceObject* sObj, gVec3D newCoords);

extern void addToObjectCoordinatesWithVector(spaceObject* sObj, gVec3D newCoords);

extern void swapObjectCoordinates(spaceObject* firstObj, spaceObject* secondObj);

extern void setObjectFlag(spaceObject* sObj, uint16_t newFlag);

extern void switchObjectFlagBit(spaceObject* sObj, uint8_t index);

extern void switchObjectFlagBits(spaceObject* sObj, uint8_t indiciesLength, uint8_t indicies[]);

extern uint16_t getObjectFlag(spaceObject* sObj);

typedef struct objectTextureAttribute{
	SDL_Texture* tex;
	char* textureLocation;
	uint8_t textureFlag;
} texAttr;

extern void setTextureLocation(texAttr* tAttr, char* newLocation);

extern void setTexture(texAttr* tAttr, SDL_Texture* newTex);

extern void switchTextureFlagBit(texAttr* tAttr, uint8_t index);

extern void switchTextureFlagBits(texAttr* tAttr, uint8_t indiciesLength, uint8_t indicies[]);

typedef struct{
	clickable area; //Clickable is used for rendering to the clickable area
	uint8_t renderFlag; // renderFlag for user customization
}renderSurface;

extern clickable* getRenderSurfaceClickable(renderSurface* rSurf);

extern void setRenderSurfaceAreaSize(renderSurface* rSurf, SDL_Rect newSize);

extern void setRenderFlag(renderSurface* rSurf, uint8_t newFlag);

extern void switchRenderFlag(renderSurface* rSurf, uint8_t index);

typedef struct{
	uint8_t channelCount;
	uint8_t channels[];
} audioEmitAttribute;

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

extern uint8_t getAnimationCount(animationAttribute* animAttr);

extern void setSelectedAnimation(animationAttribute* animAttr, uint8_t newAnim);


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

extern objAttrTypeID* objectAttributeTypes[128];


extern spaceObject* getObjectFromObjectSpace(uint16_t index);

extern int deleteFromObjSpace(uint16_t index);

extern void* defaultClick();

extern uint8_t createObjAttribute(void* (*addFunc) (spaceObject* sObj, uint8_t type), void (*freeFunc) (spaceObject* attrObj));

extern objectAttribute* getObjectAttribute(spaceObject* obj, uint8_t type);

extern objectAttribute* getObjectAttributeFromObjectSpace(uint16_t indexOfObject, uint8_t type);

// Here are the add and free functions for the general attributes,the attribute definitions are in the demolish() function

extern void* addTexture(spaceObject* sObj, uint8_t type);

extern void freeTexture(spaceObject* sObj);

extern void* addObjectAttribute(spaceObject *sObj, uint8_t type);

extern void* clickDemolish(void*);

extern void* clickMove(void*);

extern void* addSurface(spaceObject* sObj, uint8_t type);

extern void freeSurface(spaceObject* sObj);

extern void* addAnimation(spaceObject* sObj, uint8_t type);

extern void freeAnimation(spaceObject* sObj);

extern void freeObjectAttribute(spaceObject* sObj, uint8_t type);

extern void* addObjectAttribute(spaceObject* sObj, uint8_t type);

extern void demolishObject(spaceObject* sObj);

extern void freeChildren(spaceObject* sObj);

extern void demolishObject(spaceObject* sObj);

extern renderSurface* getSurface(spaceObject* sObj);

extern SDL_Rect* getRenderSurfaceRect(spaceObject* sObj);

extern animationAttribute* getAnimationAttribute(spaceObject* sObj);

extern texAttr* getTextureAttribute(spaceObject* sObj);

extern vector* getAnimationsVector(spaceObject* sObj);

/*example for animationArray parameter: {{3, 200},{2, 300},{5. 45}}, the first is the amount of frames and the second is the time in milliseconds a frame stays and this is how you get different animations from the same sprite sheet*/
extern void createAnimations(spaceObject* sObj, char* imageLocation, uint16_t cornerClip[2], uint16_t frameSize[2], uint8_t animationAmount,uint16_t animationArray[][2] );

extern void animateObject(animationAttribute* animAttr, texAttr* texture, int time);

extern void* clickDemolish(void* spcObj);

// to set attributes of objects
extern void setAttribute(spaceObject* sObj, uint8_t type, void* newAttr);

extern void* makeObject(void* space);

extern void moveSpaceObjectRectangleBy(spaceObject* sObj, gVec2D vec2D );

extern void moveSpaceObjectRectangleTo(spaceObject* sObj, gVec2D newPos);

extern void moveSpaceObjectBy(spaceObject* sObj, gVec2D vec2D );

extern void moveSpaceObjectTo(spaceObject* sObj, gVec2D newPos);

extern void* clickMove(void* sObj);


extern void demolish(int winW, int winH, int fps);

extern void stopDemolition();


#endif
