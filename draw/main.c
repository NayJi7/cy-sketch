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
void cleanResources(SDL_Renderer *renderer, SDL_Window *window, bool freeRenderer, bool freeWindow, int exitCode) 
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
int main(){

    // Initialisation SDL
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    //SDL_Color white = {255, 255, 255, 255};
    SDL_Event event;

    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        printf("Erreur SDL : %s\n", SDL_GetError());
        return -1;
    }

    // Créer la fenêtre et le renderer
    if (SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer) != 0) {
        printf("Erreur SDL_CreateWindowAndRenderer : %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    SDL_SetWindowTitle(window, "Hello SDL");

    
    if(drawShape(renderer, window, "animated", "circle", 400, 270, 60, 0xFF00FF00, "empty") == -1) return 0;
    if(drawShape(renderer, window, "instant", "line", 100, 100, 400, 300, 2, 0xFF00FF00, "filled") == -1) return 0;
    if(drawShape(renderer, window, "animated", "rectangle", 200, 200, 100, 50, 0xFF00FF00, "empty") == -1) return 0;
    if(drawShape(renderer, window, "animated", "roundedRectangle", 400, 200, 200, 150, 20, 0xFF00FF00, "empty") == -1) return 0;
    if(drawShape(renderer, window, "animated", "ellipse", 500, 370, 70, 50, 0xFF00FF00, "empty") == -1) return 0;
    if(drawShape(renderer, window, "instant", "arc", 400, 300, 100, 0, 180, 0xFF00FF00, "filled") == -1) return 0;
    if(drawShape(renderer, window, "instant", "polygon", 630, 130, 100, 12, 0xFF808080, "filled") == -1) return 0;
    
    

    // Boucle d'événements pour fermer la fenêtre
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;  // Fermer la fenêtre si l'utilisateur clique sur la croix
            }
        }
    }

    // Libérer les ressources et fermer SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}