/////////////////////////////////////////////////////////////////////////////////////////////////////
// This is a generated file. It is useless to edit it as it will be regenerated at each compilation//
/////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////
// Includes and Defines //
//////////////////////////

#include "./SDL/files.h/main.h"
#include "./SDL/files.h/colors.h"
#include "./SDL/files.h/cursorEvents.h"
#include "./SDL/files.h/form.h"

// ANSI escape codes for colors
#define RED_COLOR "\033[1;31m"
#define RESET_COLOR "\033[0m"

#define TRUE 1
#define FALSE 0

#define bgcolorR 255
#define bgcolorG 255
#define bgcolorB 255
#define cursorcolorR 0
#define cursorcolorG 0
#define cursorcolorB 0
#define cursorcolorA 255
#define cursorSize 5
#define windowW 800
#define windowH 600
#define windowTitle "Draw ++"

///////////////////////////
// Function's prototypes //
///////////////////////////


/////////////////////////////
// Function's declarations //
/////////////////////////////

///////////////////
// Main function //
///////////////////

int main() {

    /////////////////////////
    // Configuration Start //
    /////////////////////////

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Event event;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        printf("%sExecutionError: Failed to initialize SDL: %s%s\n",RED_COLOR, SDL_GetError(), RESET_COLOR);
        return -1;
    }
    if (SDL_CreateWindowAndRenderer(windowW, windowH, SDL_WINDOW_RESIZABLE, &window, &renderer) != 0) {
        printf("%sExecutionError: Failed to create window and renderer: %s%s\n", RED_COLOR, SDL_GetError(), RESET_COLOR);
        SDL_Quit();
        return -1;
    }
    SDL_SetWindowTitle(window, windowTitle);
    SDL_Color cursorColor = {cursorcolorR, cursorcolorG, cursorcolorB, cursorcolorA};
    Cursor cursor = createCursor(windowW/2, windowH/2, cursorColor, cursorSize, true);
    SDL_Texture* mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, windowW, windowH);
    if (!mainTexture) {
        printf("%sExecutionError: Failed to create main texture: %s%s\n", RED_COLOR, SDL_GetError(), RESET_COLOR);
        cleanup(mainTexture, renderer, window);
        return -1;
    }
    if (SDL_SetRenderTarget(renderer, mainTexture) != 0) {
        printf("%sExecutionError: Failed to set render target: %s%s\n", RED_COLOR, SDL_GetError(), RESET_COLOR);
        cleanup(mainTexture, renderer, window);
        return -1;
    }
    SDL_SetRenderDrawColor(renderer, bgcolorR, bgcolorG, bgcolorB, 255);
    if (SDL_RenderClear(renderer) != 0) {
        printf("%sExecutionError: Failed to clear renderer: %s%s\n", RED_COLOR, SDL_GetError(), RESET_COLOR);
        cleanup(mainTexture, renderer, window);
        return -1;
    }

    /////////////////////////////
    // User Instructions Start //
    /////////////////////////////

	if(drawShape(renderer, mainTexture, "circle", "instant", "filled", green, 400, 170, 60) == -1) {
		cleanup(mainTexture, renderer, window);
		printf("%sExecutionError: Failed to draw circle: %s%s\n", RED_COLOR, SDL_GetError(), RESET_COLOR);
		return -1;
	};

	if(drawShape(renderer, mainTexture, "square", "instant", "empty", blue, 400, 170, 60) == -1) {
		cleanup(mainTexture, renderer, window);
		printf("%sExecutionError: Failed to draw square: %s%s\n", RED_COLOR, SDL_GetError(), RESET_COLOR);
		return -1;
	};

	if(drawShape(renderer, mainTexture, "rectangle", "instant", "filled", yellow, 400, 170, 60, 30) == -1) {
		cleanup(mainTexture, renderer, window);
		printf("%sExecutionError: Failed to draw rectangle: %s%s\n", RED_COLOR, SDL_GetError(), RESET_COLOR);
		return -1;
	};

	if(drawShape(renderer, mainTexture, "triangle", "instant", "empty", red, 400, 170, 60) == -1) {
		cleanup(mainTexture, renderer, window);
		printf("%sExecutionError: Failed to draw triangle: %s%s\n", RED_COLOR, SDL_GetError(), RESET_COLOR);
		return -1;
	};


    ///////////////////////////
    // User Instructions End //
    ///////////////////////////

    // Reset render target and display content
    if (SDL_SetRenderTarget(renderer, NULL) != 0) {
        printf("%sExecutionError: Failed to reset render target: %s%s\n", RED_COLOR, SDL_GetError(), RESET_COLOR);
        cleanup(mainTexture, renderer, window);
        return -1;
    }
    if (SDL_RenderCopy(renderer, mainTexture, NULL, NULL) != 0) {
        printf("%sExecutionError: Failed to copy texture to renderer: %s%s\n", RED_COLOR, SDL_GetError(), RESET_COLOR);
        cleanup(mainTexture, renderer, window);
        return -1;
    }
    SDL_RenderPresent(renderer);
    mainLoop(window, renderer, event, cursor, bgcolorR, bgcolorG, bgcolorB);
    cleanup(mainTexture, renderer, window);

	return 0;
}
