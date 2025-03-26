#include "headers/Demolition_Engine.h"
#include "headers/Demolition_Physics.h"
#include <stdio.h>

uint16_t physicsFrequency = 30;
uint16_t renderFrequency = 60;
uint16_t logicFrequency = 120;


int newAudioIndex = -1;

void setup(uint64_t startTime){
	printf("Setup function!\n");
}

void logicUpdate(uint64_t time){
	//if (newAudioIndex == -1) newAudioIndex = playAudioConstant("Resources/laserShoot.wav", DEMOLITION_LOCAL_AUDIO);
	//else playAudioConstantFromIndex(newAudioIndex, DEMOLITION_LOCAL_AUDIO);
}

void quit(){
	printf("Quit function!\n");
}