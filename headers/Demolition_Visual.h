#ifndef DEMOLITION_ENGINE_H
#include "Demolition_Engine.h"
#endif

#ifndef DEMOLITION_ENGINE_VISUAL_H
#define DEMOLITION_ENGINE_VISUAL_H
#include <SDL_image.h>
// Create palette to use in coloring the UI

SDL_Color colors[] = {{0, 255, 246, 255}, {0, 231, 255, 255}, {0, 158, 255, 255}, {0, 20, 255, 255}};
SDL_Color BackgroundColor = {0,200,30,255}; 

void initTex(SDL_Texture* texAddr, SDL_Rect* dest,char* fileLocation){
	SDL_Surface* surface;
	surface = IMG_Load(fileLocation);
	texAddr = SDL_CreateTextureFromSurface(engineRenderer, surface);
	SDL_FreeSurface(surface);
	SDL_QueryTexture(texAddr, NULL, NULL, &dest->w, &dest->h);
}

void setTexutre(spaceObject* sObj, SDL_Texture* givenTexture, char* givenTextureLocation){
	texAttr* objAttr = (texAttr*) getObjectAttribute(sObj, TEXTURE_INDEX)->attribute;
	objAttr->tex = givenTexture;
	objAttr->textureLocation = givenTextureLocation;
}

void RenderObject(spaceObject* obj){
	objectAttribute* rendSurface = getObjectAttribute(obj, SURFACE_INDEX);
	objectAttribute* objTexture = getObjectAttribute(obj, TEXTURE_INDEX);
	
	if (rendSurface){ 
		SDL_Rect* texDest = &(((renderSurface*)rendSurface->attribute)->area.dimensions);
		SDL_Texture* tex = ((texAttr*)objTexture->attribute)->tex;
		SDL_RenderCopy(engineRenderer, (objTexture ? tex : defaultTexture), NULL, texDest);
	}
	return;
}

void RenderScene(){
	for(int rendIter = 0; rendIter < vectorTotal(&objectSpace); rendIter++){
		RenderObject((spaceObject*) vectorGet(&objectSpace, rendIter));
	}
	return;
}

void trackfps(int begin) {
	static int counter;

	counter++;
	if(counter >= framerate){
		int n = (SDL_GetTicks64() - begin);
		float t = n / 1000.0;

		printf("%.2f\n", 1/t);
		counter = 0;
	}
	return;
}

#endif

#ifdef DEMOLITION_ENGINE_UI_H
void queryButtonTex(demolition_button* butt){
	SDL_QueryTexture(butt->t.tex, NULL, NULL, &butt->c.dimensions.w, &butt->c.dimensions.h);
	butt->c.dimensions.w /= 2;
	butt->c.dimensions.h /= 2;
	return;
}

void RenderButton(demolition_button* item){
	SDL_RenderCopy(engineRenderer, item->t.tex, NULL, &item->c.dimensions);
	return;
}

// Render the UI
void RenderDemolitionUI(){
	SDL_SetRenderDrawColor(engineRenderer, colors[0].r, colors[0].g, colors[0].b, colors[0].a);
	SDL_RenderFillRect(engineRenderer, &demolitionInterface.size);
    SDL_RenderDrawRect(engineRenderer, &demolitionInterface.size);
	SDL_SetRenderDrawColor(engineRenderer, colors[1].r, colors[1].g, colors[1].b, colors[1].a);
	SDL_RenderFillRect(engineRenderer, &demolitionLog.size);
    SDL_RenderDrawRect(engineRenderer, &demolitionLog.size);
	SDL_SetRenderDrawColor(engineRenderer, BackgroundColor.r, BackgroundColor.g, BackgroundColor.b, BackgroundColor.a);
}

#endif