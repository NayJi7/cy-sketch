// mingw32-make EQUIVALENT à make sur Windows

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>

#undef main  //avoid main's problems when we doing a make

int setWindowColor(SDL_Renderer *renderer, SDL_Color color)
{
    if(SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a) < 0)
    {
        return -1;
    }
    if(SDL_RenderClear(renderer) < 0)
    {
        return -1;
    }
    return 0;
}


int main(int argc, char *argv[]) {
    
    //Initialise a window
    SDL_Window *window = NULL;
    //Initialise a render to have the possibility of drawing
    SDL_Renderer *renderer = NULL;
    int statut = EXIT_FAILURE;
    SDL_Color blue = {0, 0, 255, 255};
    //Initialise a Cursor
    SDL_Cursor *cursor = NULL;
    //Initialise an event
    SDL_Event event;


    //Initialise the rendering(VIDEO for window) and\or EVENTS management system
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    {
        printf("Error SDL : %s\n", SDL_GetError());
        goto Quit;
    }

//How to create a cursor :
    cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    if(!cursor)
    {
        printf("Error SDL_CreateSystemCursor : %s\n", SDL_GetError());
        goto Quit;
    }
    SDL_SetCursor(cursor);
    SDL_ShowCursor(SDL_ENABLE);

// Create a window
    /*
    //=> SDL_CreateWindow(title, x, y, weight, height, (flags))
        //flags : SDL_WINDOW_RESIZABLE = change the size with your cursor
    window = SDL_CreateWindow("Hello SDL",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 400, SDL_WINDOW_RESIZABLE);
    // robustness
    if (!window) {
        printf("Error SDL_CreateWindow : %s\n", SDL_GetError());
        goto Quit;
    }
// ----------------------------------------------------------------------
    */

// Create a renderer to interect with the window 
    /*
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer)
    {
        printf("Error SDL_CreateRenderer : %s\n", SDL_GetError());
        goto Quit;
    }
    */
    
   //CREATE window and Renderer in a same time
    if(SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer) != 0)
    {
        printf("Error SDL_CreateWindowAndRenderer : %s\n", SDL_GetError());
        goto Quit;
    }
    SDL_SetWindowTitle(window, "Hello SDL");

    //Setting a color
    if(setWindowColor(renderer, blue) != 0)
    {
        printf("Error setWindowColor : %s\n", SDL_GetError());
        goto Quit;
    }
    SDL_RenderPresent(renderer);

//Event Part
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
        // MOUSE/SCROLL/KEYBORD EVENTS
            switch (event.type) {
            //MOVEMENT PART
                case SDL_MOUSEMOTION:
                    printf("The mouse moved to (%d, %d)\n", event.motion.x, event.motion.y);
                    break;
                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0) {
                        printf("Thumbwheel up\n");
                    } else if (event.wheel.y < 0) {
                        printf("Thumbwheel down\n");
                    }
                    break;
            //PRESS PART        
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        printf("Left mouse button pressed to (%d, %d)\n", event.button.x, event.button.y);
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        printf("Left mouse button pressed to (%d, %d)\n", event.button.x, event.button.y);
                    }
                    break;
                //WINDOW EVENTS
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            printf("Window resized to (%d, %d)\n", event.window.data1, event.window.data2);
                            break;
                        case SDL_WINDOWEVENT_CLOSE:
                            printf("The window has been closed\n");
                            break;
                    }
                    break;
                //CLOSED PART    
                case SDL_QUIT:
                    running = 0;
                    break;
            }
        }

        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    statut = EXIT_SUCCESS;

Quit:
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);

    SDL_Quit();
    return statut;
}


