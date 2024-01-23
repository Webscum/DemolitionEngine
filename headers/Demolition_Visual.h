#ifndef DEMOLITION_ENGINE_VISUAL_H
#define DEMOLITION_ENGINE_VISUAL_H
#include <SDL.h>
#include <SDL_image.h>
// Create palette to use in coloring the UI

SDL_Color colors[] = {{0, 255, 246, 255}, {0, 231, 255, 255}, {0, 158, 255, 255}, {0, 20, 255, 255}};
SDL_Color BackgroundColor = {0,200,30,255}; 

typedef struct objectAttribute;

void initTex(SDL_Texture* texAddr, SDL_Rect* dest,char* fileLocation){
	SDL_Surface* surface;
	surface = IMG_Load(fileLocation);
	texAddr = SDL_CreateTextureFromSurface(engineRenderer, surface);
	SDL_FreeSurface(surface);
	SDL_QueryTexture(texAddr, NULL, NULL, &dest->w, &dest->h);
}

void queryButtonTex(demolition_button* butt){
	SDL_QueryTexture(butt->t.tex, NULL, NULL, &butt->c.dimensions.w, &butt->c.dimensions.h);
	butt->c.dimensions.w /= 2;
	butt->c.dimensions.h /= 2;
}

void RenderButton(demolition_button* item){
	SDL_RenderCopy(engineRenderer, item->t.tex, NULL, &item->c.dimensions);
}

void renderObject(spaceObject* obj){
	objectAttribute* rendSurface = getObjectAttribute(obj, "RenderSurface");
	objectAttribute* objTexture = getObjectAttribute(obj, "Texture");
	
	if (rendSurface){ 
		SDL_Rect* texDest = &(((renderSurface*)rendSurface->attribute)->area.dimensions);
		SDL_Texture* tex = ((texAttr*)objTexture->attribute)->tex;
		SDL_RenderCopy(engineRenderer, (objTexture ? tex : defaultTexture), NULL, texDest);
	}
}

#endif