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

    // Dessiner les formes animées
    /*if(drawAnimatedCircle(renderer, window, 400, 270, 60, 0xFF00FF00, "fille") == -1) return 0;

    if(drawAnimatedRectangle(renderer, window, 200, 170, 80, 60, 0xFF000000, "filled") == -1) return 0;
    //drawAnimatedRectangle(renderer, 200, 170, 80, 60, 0xFF00FF00, "filled");
    if(drawAnimatedEllipse(renderer, window, 500, 370, 70, 50, 0xFF00FF00, "filled") == -1) return 0;

    if(drawAnimatedCustomPolygon(renderer, window, 630, 130, 100, 12, 0xFF808080, "empty") == -1) return 0;

    if(drawAnimatedLine(renderer, window, 210, 210, 340, 340, 0xFFFF8800, 5, "filled") == -1) return 0;*/

    
    if(drawShape(renderer, window, "instant", "circle", 400, 270, 60, 0xFF00FF00, "filled") == -1) return 0;
    if(drawShape(renderer, window, "instant", "rectangle", 200, 200, 100, 50, 0x00FF00FF, "empty") == -1) return 0;
    if(drawShape(renderer, window, "instant", "ellipse", 500, 370, 70, 50, 0xFF00FF00, "filled") == -1) return 0;
    if(drawShape(renderer, window, "instant", "polygon", 630, 130, 100, 12, 0xFF808080, "filled") == -1) return 0;
    if(drawShape(renderer, window, "instant", "line", 210, 210, 340, 340, 0xFFFF8800, 20, "filled") == -1) return 0;
    

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