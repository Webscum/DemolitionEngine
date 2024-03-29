#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include "headers/CVector.h"
#include "headers/Demolition_Engine.h"

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


	Demolish(atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
	
	bool exiting = false;	
	float UpdateFreq  =  1000.0 / framerate;
	u_int64_t begin = SDL_GetTicks64();
	
	demolition_button objectCreateButton = {
		{{100, 20}, makeObject},
		{defaultTexture, DEMOLITION_DEFAULT_TEXTURE}
	};

	queryButtonTex(&objectCreateButton);
	
	while(!exiting){
		begin = SDL_GetTicks64();
		bool attributes = false;
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				exiting = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				printf("%d\n", vectorTotal(&objectSpace));
				for(int clickIter = 0; clickIter < vectorTotal(&objectSpace); clickIter++){
					renderSurface* rendSurfAttr = (renderSurface*) getObjectAttributeFromObjectSpace(clickIter, RENDER_SURFACE_ATTRIBUTE)->attribute;
					int numberGot = clicked(&rendSurfAttr->area, &event.button, engineWindow);
					printf("%d\n", numberGot);	
				}
				printf("%d\n", clicked(&objectCreateButton.c, &event.button, engineWindow));
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


