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

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
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
    SDL_SetWindowTitle(window, "Animated Shapes");

        // Couleur de fond
    SDL_Color backgroundColor = {0, 0, 0, 255};

    // Définir un cercle
    Circle circle = {400, 300, 50, {0, 255, 0, 255}, false};

    // Définir un curseur
    SDL_Color cursorColor = {255, 0, 0, 255};
    Cursor cursor = createCursor(400, 300, cursorColor, 5, true);


    // Créer une texture principale pour dessiner
    SDL_Texture* mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 800, 600);

    // Remplir la texture de fond noir
    SDL_SetRenderTarget(renderer, mainTexture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    
    if(drawShape(renderer, mainTexture, "instant", "circle", 400, 270, 60, 0xFF00FF00, "filled") == -1) return 0;
    if(drawShape(renderer, mainTexture, "animated", "line", 100, 100, 400, 300, 2, 0xFF00FF00, "filled") == -1) return 0;
    if(drawShape(renderer, mainTexture, "instant", "rectangle", 200, 200, 100, 50, 0xFF00FF00, "filled") == -1) return 0;
    if(drawShape(renderer, mainTexture, "animated", "roundedRectangle", 400, 200, 200, 150, 20, 0xFF00FF00, "filled") == -1) return 0;
    if(drawShape(renderer, mainTexture, "instant", "ellipse", 500, 370, 70, 50, 0xFF00FF00, "filled") == -1) return 0;
    if(drawShape(renderer, mainTexture, "animated", "arc", 400, 300, 100, 0, 180, 0xFF00FF00, "filled") == -1) return 0;
    if(drawShape(renderer, mainTexture, "instant", "polygon", 630, 130, 100, 12, 0xFF808080, "filled") == -1) return 0;
    
    // Rétablir la cible de rendu et afficher la texture
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, mainTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
    

    // Boucle d'événements pour fermer la fenêtre
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                // Désélectionner avec la touche ÉCHAP
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    circle.selected = false;
                } else if (circle.selected) {
                    // Déplacer la forme sélectionnée
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            circle.y -= 10;
                            break;
                        case SDLK_DOWN:
                            circle.y += 10;
                            break;
                        case SDLK_LEFT:
                            circle.x -= 10;
                            break;
                        case SDLK_RIGHT:
                            circle.x += 10;
                            break;
                        case SDLK_PLUS:    // Zoomer
                        case SDLK_KP_PLUS: // Touche "+" du pavé numérique
                            circle.radius += 5; // Augmenter le rayon
                            break;
                        case SDLK_MINUS:   // Dézoomer
                        case SDLK_KP_MINUS:// Touche "-" du pavé numérique
                            if (circle.radius > 5) { // Empêcher un rayon négatif ou nul
                                circle.radius -= 5;
                            }
                            break;
                    }
                } else {
                    // Déplacer le curseur
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            moveCursor(&cursor, 0, -10);
                            break;
                        case SDLK_DOWN:
                            moveCursor(&cursor, 0, 10);
                            break;
                        case SDLK_LEFT:
                            moveCursor(&cursor, -10, 0);
                            break;
                        case SDLK_RIGHT:
                            moveCursor(&cursor, 10, 0);
                            break;
                        case SDLK_RETURN: // Sélectionner la forme
                            if (isCursorInCircle(&cursor, &circle)) {
                                circle.selected = !circle.selected; // Alterner l'état sélectionné
                            }
                            break;
                        case SDLK_DELETE:  // Supprimer la forme
                            // Réinitialiser les propriétés du cercle
                            if (circle.selected) {
                                // Supprimer complètement la forme
                                circle.x = -100;      // Déplacer le cercle hors de l'écran
                                circle.y = -100;
                                circle.radius = 0;    // Réduire le rayon à 0
                                circle.selected = false; // Désélectionner
                                // Ici vous pouvez aussi complètement réinitialiser la structure
                                circle = (Circle){-100, -100, 0, {0, 0, 0, 255}, false};
                            }
                            break;
                    }
                }
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                // Désélectionner si clic en dehors du cercle
                if (circle.selected) {
                    int dx = event.button.x - circle.x;
                    int dy = event.button.y - circle.y;
                    if ((dx * dx + dy * dy) > (circle.radius * circle.radius)) {
                        circle.selected = false; // Désélectionner
                    }
                }
            }
        }

        // Effacer l'écran
        setWindowColor(renderer, backgroundColor);

        // Dessiner le cercle uniquement si il n'est pas supprimé
        if (circle.radius > 0) {
            SDL_SetRenderDrawColor(renderer, circle.color.r, circle.color.g, circle.color.b, circle.color.a);
            for (int w = 0; w < circle.radius * 2; w++) {
                for (int h = 0; h < circle.radius * 2; h++) {
                    int dx = circle.radius - w; // Distance horizontale
                    int dy = circle.radius - h; // Distance verticale
                    if ((dx * dx + dy * dy) <= (circle.radius * circle.radius)) {
                        SDL_RenderDrawPoint(renderer, circle.x + dx, circle.y + dy);
                    }
                }
            }

            // Indiquer si le cercle est sélectionné
            if (circle.selected) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Couleur jaune pour indiquer la sélection
                SDL_Rect selectionRect = {circle.x - circle.radius - 5, circle.y - circle.radius - 5, 
                                           2 * circle.radius + 10, 2 * circle.radius + 10};
                SDL_RenderDrawRect(renderer, &selectionRect);
            }
        }

        // Dessiner le curseur
        renderCursor(renderer, &cursor);

        // Afficher le rendu
        SDL_RenderPresent(renderer);
    }


    // Libérer les ressources
    SDL_DestroyTexture(mainTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

