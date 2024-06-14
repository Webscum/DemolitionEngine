//#include <cstdint>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include "headers/CVector.h"
#include "headers/Demolition_Engine.h"
#include "headers/Demolition_UI.h"
#include "headers/Demolition_Visual.h"

// The extensibility of this engine should be improved!!!!!!

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
	
	bool exiting = false;	
	float UpdateFreq  =  1000.0 / framerate;
	int begin = SDL_GetTicks64();

	demolition_button objectCreateButton = {
		{{100, 20}, makeObject},
		{defaultTexture, DEMOLITION_DEFAULT_TEXTURE}
	};

	spaceObject* animationTest = (spaceObject*) makeObject(&objectSpace);

	addObjectAttribute(animationTest, ANIMATION_INDEX);

	uint16_t corners[2] = {40,40};
	uint16_t frameSize[2] = {120, 120};

	createAnimations(animationTest, "Resources/BombSprites.jpg", corners, frameSize, (uint16_t[][2]) {{7, 75}});


	queryButtonTex(&objectCreateButton);
	
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
					//printf("%d\n", vectorTotal(&objectSpace));
					checkClicks(&event.button);
					printf("\n");
					break;
				}
			}

		}
		
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

	//SDL_DestroyRenderer(rend);
	SDL_DestroyRenderer(engineRenderer);
	SDL_DestroyWindow(engineWindow);

	SDL_Quit();
	
}
