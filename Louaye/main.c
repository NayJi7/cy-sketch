// mingw32-make EQUIVALENT à make sur Windows

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>

#undef main  //avoid main's problems when we doing a make

int main(int argc, char *argv[]) {
    
    //Initialise a window
    SDL_Window *window = NULL;

    int statut = EXIT_FAILURE;

    //Initialise the rendering(VIDEO for window) and\or EVENTS management system
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    {
        printf("Error SDL : %s\n", SDL_GetError());
        goto Quit;
    }

// Create a window
    //=> SDL_CreateWindow(title, x, y, weight, height, (flags))
        //flags : SDL_WINDOW_RESIZABLE = change the size with your cursor
    window = SDL_CreateWindow("Hello SDL",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 400, SDL_WINDOW_RESIZABLE);
    // robustness
    if (!window) {
        printf("Could not create window : %s\n", SDL_GetError());
        goto Quit;
    }
// ----------------------------------------------------------------------


    // Waiting an event before to quit
    SDL_Event event;
    while (1) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }
    }

    statut = EXIT_SUCCESS;
    SDL_DestroyWindow(window);

Quit:
    SDL_Quit();
    return statut;
}


