#include "../files.h/main.h"

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


int main(void) {
    
    //Initialise a window
    SDL_Window *window = NULL;
    //Initialise a render to have the possibility of drawing
    SDL_Renderer *renderer = NULL;

    int statut = EXIT_FAILURE;

    SDL_Color white = {255, 255, 255, 255};
    //Initialise a Surface for the cursor
    SDL_Surface *cursorSurface = NULL;
    //Initialise a surface
    SDL_Surface *screenSurface = NULL;
    //Initialise a Surface for the cursor
    SDL_Cursor *cursor = NULL;
    //Initialise an event
    SDL_Event event;

    //Initialise the rendering(VIDEO for window) and\or EVENTS management system
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
    {
        printf("Error SDL : %s\n", SDL_GetError());
        goto Quit;
    }

 
   //CREATE window and Renderer in a same time
    if(SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer) != 0)
    {
        printf("Error SDL_CreateWindowAndRenderer : %s\n", SDL_GetError());
        goto Quit;
    }
    SDL_SetWindowTitle(window, "Hello SDL");


    //Setting a color
    if(setWindowColor(renderer, white) != 0)
    {
        printf("Error setWindowColor : %s\n", SDL_GetError());
        goto Quit;
    }
    SDL_RenderPresent(renderer);


    drawCircle(renderer, 400, 270, 60, 0xFF00FF00, "filled");
    drawEllipse(renderer, 500, 370, 70, 50, 0xFFFF0000, "empty");
    drawRectangle(renderer, 200, 170, 80, 60, 0xFF000000, "filled");
    drawCustomPolygon(renderer, 630, 130, 100, 12, 0xFF808080, "empty");
    drawLine(renderer, 210, 210, 340, 340, 0xFFFF8800, 5, "filled");

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
    }

    statut = EXIT_SUCCESS;

Quit:
    if(renderer)
        SDL_DestroyRenderer(renderer);
    if(window)
        SDL_DestroyWindow(window);
    if(cursorSurface)
        SDL_FreeSurface(cursorSurface);

    SDL_Quit();

    return statut;
}


