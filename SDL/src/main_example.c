#include "../files.h/main.h"
#include "../files.h/colors.h"
#include "../files.h/cursorEvents.h"
#include "../files.h/form.h"

// ANSI escape codes for colors
#define RED_COLOR "\033[1;31m"
#define RESET_COLOR "\033[0m"

/**
 * @brief Main function that initializes SDL, creates a window and renderer, and draws shapes.
 * 
 * @return int Returns 0 on successful execution.
 */
int main(){

    SDL_Window *window = NULL; // Declare a pointer for the SDL window.
    SDL_Renderer *renderer = NULL; // Declare a pointer for the SDL renderer.
    SDL_Event event; // Declare an SDL_Event structure to handle user inputs (e.g., keyboard, mouse).

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) { 
        printf("%sExecutionError: Failed to initialize SDL: %s%s\n", 
               RED_COLOR, SDL_GetError(), RESET_COLOR);
        return -1;
    }

    // Create the window and renderer
    if (SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer) != 0) {
        printf("%sExecutionError: Failed to create window and renderer: %s%s\n", 
               RED_COLOR, SDL_GetError(), RESET_COLOR);
        SDL_Quit();
        return -1;
    }

    SDL_SetWindowTitle(window, "Animated Shapes");

    // Background color for the cursor
    SDL_Color cursorColor = {0, 0, 0, 0}; 
    Cursor cursor = createCursor(400, 300, cursorColor, 5, true);

    // Create a main texture for drawing
    SDL_Texture* mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
                                                SDL_TEXTUREACCESS_TARGET, 800, 600);
    if (!mainTexture) {
        printf("%sExecutionError: Failed to create main texture: %s%s\n", 
               RED_COLOR, SDL_GetError(), RESET_COLOR);
        cleanup(mainTexture, renderer, window);
        return -1;
    }

    // Set the main texture as the render target and fill it with a light gray background
    if (SDL_SetRenderTarget(renderer, mainTexture) != 0) {
        printf("%sExecutionError: Failed to set render target: %s%s\n", 
               RED_COLOR, SDL_GetError(), RESET_COLOR);
        cleanup(mainTexture, renderer, window);
        return -1;
    }
    
    SDL_SetRenderDrawColor(renderer, 229, 229, 229, 255);
    
    if (SDL_RenderClear(renderer) != 0) {
        printf("%sExecutionError: Failed to clear renderer: %s%s\n", 
               RED_COLOR, SDL_GetError(), RESET_COLOR);
        cleanup(mainTexture, renderer, window);
        return -1;
    }

    // Draw shapes
    if(drawShape(renderer, mainTexture, "circle", "instant", "filled", "color", green, 400, 170, 60) == -1) {
        cleanup(mainTexture, renderer, window);
        return -1;
    }

    if(drawShape(renderer, mainTexture, "ellipse", "instant", "filled", "zoom", brown, 300, 370, 70, 50) == -1) {
        cleanup(mainTexture, renderer, window);
        return -1;
    }

    if(drawShape(renderer, mainTexture, "line", "instant", "filled", "rotate", pink, 100, 100, 400, 300, 2) == -1) {
        cleanup(mainTexture, renderer, window);
        return -1;
    }
    
    if(drawShape(renderer, mainTexture, "polygon", "instant", "filled", "zoom", black, 330, 130, 100, 4) == -1) {
        cleanup(mainTexture, renderer, window);
        return -1;
    }

    if(drawShape(renderer, mainTexture, "rectangle", "instant", "filled", "zoom", gold, 200, 200, 200, 50) == -1) {
        cleanup(mainTexture, renderer, window);
        return -1;
    }

    if(drawShape(renderer, mainTexture, "arc", "instant", "filled", "zoom", silver, 400, 200, 100, 90, 180) == -1) {
        cleanup(mainTexture, renderer, window);
        return -1;
    }

    // Reset render target and display content
    if (SDL_SetRenderTarget(renderer, NULL) != 0) {
        printf("%sExecutionError: Failed to reset render target: %s%s\n", 
               RED_COLOR, SDL_GetError(), RESET_COLOR);
        cleanup(mainTexture, renderer, window);
        return -1;
    }
    
    if (SDL_RenderCopy(renderer, mainTexture, NULL, NULL) != 0) {
        printf("%sExecutionError: Failed to copy texture to renderer: %s%s\n", 
               RED_COLOR, SDL_GetError(), RESET_COLOR);
        cleanup(mainTexture, renderer, window);
        return -1;
    }
    
    SDL_RenderPresent(renderer);

    // Start main event loop
    mainLoop(renderer, event, cursor);

    cleanup(mainTexture, renderer, window);

    return 0;
}