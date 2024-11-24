#include "../files.h/main.h"

/**
 * @brief Cleans up and releases SDL resources as specified.
 * 
 * @param renderer The SDL renderer to be destroyed if `freeRenderer` is true.
 * @param window The SDL window to be destroyed if `freeWindow` is true.
 * @param freeRenderer A boolean indicating whether to free the renderer.
 * @param freeWindow A boolean indicating whether to free the window.
 * @param exitCode The exit code to terminate the program with if not -1.
 * 
 * @details 
 * This function is used to clean up SDL resources like the renderer and the window. 
 * If `freeRenderer` is true and `renderer` is valid, the renderer is destroyed.
 * Similarly, if `freeWindow` is true and `window` is valid, the window is destroyed.
 * After cleanup, SDL is properly quit, and if `exitCode` is not -1, the program 
 * exits with the specified exit code.
 */
extern void cleanResources(SDL_Renderer *renderer, SDL_Window *window, bool freeRenderer, bool freeWindow, int exitCode) 
{
    
    if(freeRenderer && renderer) SDL_DestroyRenderer(renderer);
    if(freeWindow && window) SDL_DestroyWindow(window);
    
    SDL_Quit();

    if(exitCode != -1) exit(exitCode);

}


/**
 * @brief Sets the background color of the SDL window.
 * 
 * @param renderer The SDL renderer used to set the color.
 * @param color The SDL_Color struct representing the color (RGBA) to be set.
 * 
 * @return int Returns 0 on success, -1 on failure.
 * 
 * @details 
 * This function sets the rendering color of the given SDL renderer using the 
 * RGBA values in `color`. It then clears the window to fill it with this color.
 * If either the color setting or the clearing fails, the function returns -1. 
 * Otherwise, it returns 0.
 */
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


/**
 * @brief Main function that initializes SDL, creates a window and renderer, and draws shapes.
 * 
 * @return int Returns 0 on successful execution.
 */
int main() {
    
    // Initialize an SDL window pointer
    SDL_Window *window = NULL;
    // Initialize an SDL renderer for drawing
    SDL_Renderer *renderer = NULL;
    // Define a white color for the window background
    SDL_Color white = {255, 255, 255, 255};
    // Initialize an SDL event to handle user input
    SDL_Event event;

    // Initialize SDL video and events subsystems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        printf("Error SDL: %s\n", SDL_GetError());
        cleanResources(NULL, NULL, false, false, -2);
    }

    // Create an SDL window and renderer with a resizable window of 800x600 pixels
    if (SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer) != 0) {
        printf("Error SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
        cleanResources(renderer, window, true, true, -2);
    }
    SDL_SetWindowTitle(window, "Hello SDL");

    // Set the background color of the window to white
    if (setWindowColor(renderer, white) != 0) {
        printf("Error setWindowColor: %s\n", SDL_GetError());
        cleanResources(renderer, window, true, true, -2);
    }
    SDL_RenderPresent(renderer);

    // Draw various shapes on the renderer
    drawCircle(renderer, 400, 270, 60, 0xFF00FF00, "filled");
    drawEllipse(renderer, 500, 370, 70, 50, 0xFFFF0000, "empty");
    drawRectangle(renderer, 200, 170, 80, 60, 0xFF000000, "empty");
    drawRoundedRectangle(renderer, 100, 100, 300, 200, 20, 0xFFFF8080, "empty");
    drawCustomPolygon(renderer, 630, 130, 100, 12, 0xFF808080, "filled");
    drawLine(renderer, 210, 210, 340, 340, 0xFFFF8800, 5, "filled");

    // Event handling loop
    int running = 1;
    while (running) {
        // Process events in the SDL event queue
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_MOUSEMOTION:
                    // Log mouse movement
                    printf("The mouse moved to (%d, %d)\n", event.motion.x, event.motion.y);
                    break;
                case SDL_MOUSEWHEEL:
                    // Log scroll wheel movement
                    if (event.wheel.y > 0) {
                        printf("Thumbwheel up\n");
                    } else if (event.wheel.y < 0) {
                        printf("Thumbwheel down\n");
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    // Log left mouse button press
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        printf("Left mouse button pressed at (%d, %d)\n", event.button.x, event.button.y);
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    // Log left mouse button release
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        printf("Left mouse button released at (%d, %d)\n", event.button.x, event.button.y);
                    }
                    break;
                case SDL_WINDOWEVENT:
                    // Handle window-specific events
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            printf("Window resized to (%d, %d)\n", event.window.data1, event.window.data2);
                            break;
                        case SDL_WINDOWEVENT_CLOSE:
                            printf("The window has been closed\n");
                            break;
                    }
                    break;
                case SDL_QUIT:
                    // Exit the main loop if quit event is received
                    running = 0;
                    break;
            }
        }
    }

    // Clean up resources and exit
    cleanResources(renderer, window, true, true, 0);
    return 0;
}
