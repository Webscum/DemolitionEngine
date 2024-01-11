#ifndef DEMOLITION_ENGINE_UI_H
#define DEMOLITION_ENGINE_UI_H
#include <SDL.h>
#include <stdbool.h>


typedef enum demolition_screen_section_enum{
	DEMOLITION_SCENE,
	DEMOLITION_LOG,
	DEMOLITION_INTERFACE
}demolition_screen_section_enum;

typedef struct demoliton_screen_section{
	demolition_screen_section_enum* type;
	SDL_Rect size;
	int cornerX, cornerY;
}demolition_screen_section;

typedef struct demolition_button{
	clickable c;
	texAttr t;
}demolition_button;

// check if the given x and y are in the rect
bool isInRect(SDL_Rect* rect, int x, int y){
	return ((rect->y + rect->h) > y) && ((rect->x + rect->w) > x) && (rect->y < y) && (rect->x < x);
}

// check if the rectangle is being clicked and return 0-2 based on what button is being clicked
int clicked(clickable* obj, SDL_MouseButtonEvent* but, SDL_Window* win){
	but->windowID = SDL_GetWindowID(win);
	if(isInRect(&obj->dimensions, but->x, but->y)){
		switch(but->button){
			case SDL_BUTTON_LEFT:
				return 1;
				break;
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