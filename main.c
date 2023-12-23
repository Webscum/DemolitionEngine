#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include "headers/Demolition_Engine.h"

int main(int argc, char* argv[]){
	
	Demolish();
	
	char* inputFile = argc > 3 ? argv[1] : NULL;
	if(!(inputFile)){
		printf("Not enough arguments!\n");
		return 1;
	}

	FILE* file = fopen(inputFile, "r+");

	if (!file){
		perror ("Error opening file");
		return 1;
	}
	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	SDL_Window* engineWindow = SDL_CreateWindow("GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, atoi(argv[2]), atoi(argv[3]), 0);

	Uint32 render_flags = SDL_RENDERER_ACCELERATED;

	SDL_Renderer* engineRenderer = SDL_CreateRenderer(engineWindow, 1, render_flags);
	//SDL_SetRenderDrawColor(rend, 255, 255, 255, 0);
	
	bool exiting = false;	
	float UpdateFreq  = 1000 / 60;
	spaceObject Button = {{100, 100, 500, 500}};
	vector objArray;
	vector_init(&objArray);
	
	
	while(!exiting){
		bool attributes = false;
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				exiting = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				break;
			}
		}
		SDL_RenderClear(engineRenderer);
		SDL_RenderPresent(engineRenderer);
		SDL_Delay(UpdateFreq);
	}
	
	//SDL_DestroyRenderer(rend);
	SDL_DestroyRenderer(engineRenderer);
	SDL_DestroyWindow(engineWindow);
	
	SDL_Quit();

	
}
