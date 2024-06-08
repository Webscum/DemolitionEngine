#ifndef DEMOLITION_ENGINE_H
#include "Demolition_Engine.h"
#endif


#ifndef DEMOLITION_ENGINE_UI_H
#define DEMOLITION_ENGINE_UI_H
#include <SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

typedef enum demolition_screen_section_enum{
	DEMOLITION_SCENE,
	DEMOLITION_LOG,
	DEMOLITION_INTERFACE
}demolition_screen_section_type;

typedef struct demoliton_screen_section{
	demolition_screen_section_type type;
	SDL_Rect size;
	uint8_t screenFlag;
}demolition_screen_section;

typedef struct demolition_button{
	clickable c;
	texAttr t;
}demolition_button;

demolition_screen_section demolitionLog, demolitionInterface;

// check if the given x and y are in the rect
bool isInRect(SDL_Rect* rect, int x, int y){
	return ((rect->y + rect->h) > y) && ((rect->x + rect->w) > x) && (rect->y < y) && (rect->x < x);
}

// check if the rectangle is being clicked and return 0-2 based on what button is being clicked
int clicked(clickable* obj, SDL_MouseButtonEvent* but, SDL_Window* win){
	but->windowID = SDL_GetWindowID(win);
	if(isInRect(&obj->dimensions, but->x, but->y)){
		switch(but->button){
			case SDL_BUTTON_LEFT:{
				void* ret = obj->onMouse1();
				printf("%p\n", ret);
				return 1;
				break;
			}
			case SDL_BUTTON_RIGHT:
				return 2;
				break;
			default:
				return 0;
				break;
		}
	}
	return 0;
}

#endif

#ifdef DEMOLITION_ENGINE_VISUAL_H
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