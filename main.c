#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <stdbool.h>
#include <time.h>
#include "headers/Demolition_Engine.h"

int main(int argc, char* argv[]){
	
	char* inputFile = argc > 4 ? argv[1] : NULL;
	if(!(inputFile)){
		printf("Not enough arguments!\n");
		return 1;
	}
	Demolish(atoi(argv[2]), atoi(argv[3]));
	
	FILE* file = fopen(inputFile, "r+");
	
	
	if (!file){
		perror ("Error opening file");
		return 1;
	}
	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("error initializing SDL: %s\n", SDL_GetError());
	}
	//SDL_SetRenderDrawColor(rend, 255, 255, 255, 0);
	
	bool exiting = false;	
	float UpdateFreq  =  1000 / (atoi(argv[4]));
	time_t begin;
	
	
	
	demolition_button objectCreateButton = {
		{{100, 20}, makeObject},
		{defaultTexture, DEMOLITION_DEFAULT_TEXTURE}
	};
	
	queryButtonTex(&objectCreateButton);
	
	while(!exiting){
		time(&begin);
		bool attributes = false;
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				exiting = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				printf("%d\n", clicked(&objectCreateButton.c, &event.button, engineWindow));
				spaceObject* res = (spaceObject*) objectCreateButton.c.onMouse1();
				addAttribute(res, "RenderSurface");
				break;
			}
		}
		SDL_RenderClear(engineRenderer);
		
		// Handle Rendering Here
		for(int rendIter = 0; vectorTotal(&objectSpace) > rendIter; rendIter++){
			
		}
		
		RenderButton(&objectCreateButton);
		RenderDemolitionUI();
		SDL_RenderPresent(engineRenderer);
		SDL_Delay(UpdateFreq);
		trackfps(begin);
		
	}
	
	
	//SDL_DestroyRenderer(rend);
	SDL_DestroyRenderer(engineRenderer);
	SDL_DestroyWindow(engineWindow);
	
	SDL_Quit();
	
}

