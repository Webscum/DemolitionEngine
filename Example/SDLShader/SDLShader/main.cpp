#include "SDL.h"
#include <iostream>
#include "SDL_gpu.h"
#include "gl_core_3_0.h"
#include "Shader.h"


int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	
	float resolution[2] = { 1920.f, 1080.f };

	GPU_Target *window = GPU_InitRenderer(GPU_RENDERER_OPENGL_3, resolution[0], resolution[1], GPU_DEFAULT_INIT_FLAGS);

	if (window == NULL || ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		std::cout << "error initialization OpenGL\n";
	}

	GPU_Image *field = GPU_LoadImage("field.png");

	// Initialize the shader the shader 
	Shader shad("rain", "v1.vert", "rain.frag");
	shad.addVariable("tex0");
	shad.addVariable("tex1");
	shad.addVariable("globalTime");
	shad.addVariable("resolution");
	shad.addImg("channel0.psd");

	SDL_Event event;

	bool done = 0;

	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				done = 1;
			else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_ESCAPE)
					done = 1;
			}
		}

		GPU_Clear(window);

		shad.activate();

		GLfloat time = (GLfloat)SDL_GetTicks();
		GPU_SetUniformf(shad.getVar("globalTime"), time);
		shad.setImgShader();
		GPU_SetUniformfv(shad.getVar("resolution"), 2, 1, resolution);
		GPU_Blit(field, NULL, window, field->w / 2.f, field->h / 2.f);

		GPU_DeactivateShaderProgram();

		GPU_Flip(window);
	}

	shad.freeImg();
	GPU_FreeImage(field);

	GPU_Quit();

	return 0;
}