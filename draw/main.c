#include "../files.h/main.h"

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
    if(drawShape(renderer, mainTexture, "animated", "rectangle", 200, 200, 100, 50, 0xFF00FF00, "filled") == -1) return 0;
    //if(drawShape(renderer, mainTexture, "animated", "roundedRectangle", 400, 200, 200, 150, 20, 0xFF00FF00, "filled") == -1) return 0;
    //if(drawShape(renderer, mainTexture, "instant", "ellipse", 500, 370, 70, 50, 0xFF00FF00, "filled") == -1) return 0;
    //if(drawShape(renderer, mainTexture, "animated", "arc", 400, 300, 100, 0, 180, 0xFF00FF00, "filled") == -1) return 0;
    //if(drawShape(renderer, mainTexture, "instant", "polygon", 630, 130, 100, 12, 0xFF808080, "filled") == -1) return 0;
    //if(drawShape(renderer, mainTexture, "animated", "line", 100, 100, 400, 300, 2, 0xFF00FF00, "filled") == -1) return 0;

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
                for (int i = 0; i < shapeCount; i++) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        shapes[i].selected = false;
                    }
                    else if(shapes[i].selected)
                    {
                        switch (event.key.keysym.sym) {
                            Shape *shape = &shapes[i];
                            case SDLK_UP:
                                if (shape->type == SHAPE_CIRCLE) shape->data.circle.y -= 10;
                                if (shape->type == SHAPE_RECTANGLE) shape->data.rectangle.y -= 10;
                                break;
                            case SDLK_DOWN:
                                if (shape->type == SHAPE_CIRCLE) shape->data.circle.y += 10;
                                if (shape->type == SHAPE_RECTANGLE) shape->data.rectangle.y += 10;
                                break;
                            case SDLK_LEFT:
                                if (shape->type == SHAPE_CIRCLE) shape->data.circle.x -= 10;
                                if (shape->type == SHAPE_RECTANGLE) shape->data.rectangle.x -= 10;
                                break;
                            case SDLK_RIGHT:
                                if (shape->type == SHAPE_CIRCLE) shape->data.circle.x += 10;
                                if (shape->type == SHAPE_RECTANGLE) shape->data.rectangle.x += 10;
                                break;
                            case SDLK_PLUS: // Zoomer
                            case SDLK_KP_PLUS: // Touche "+" du pavé numérique
                                if (shape->type == SHAPE_CIRCLE) shape->data.circle.radius += 5;
                                if (shape->type == SHAPE_RECTANGLE) {
                                    shape->data.rectangle.width += 10;
                                    shape->data.rectangle.height += 10;
                                }
                                break;
                            case SDLK_MINUS: // Dézoomer
                            case SDLK_KP_MINUS:// Touche "-" du pavé numérique 
                                if (shape->type == SHAPE_CIRCLE && shape->data.circle.radius > 5) {
                                    shape->data.circle.radius -= 5;
                                }
                                if (shape->type == SHAPE_RECTANGLE &&
                                    shape->data.rectangle.width > 10 &&
                                    shape->data.rectangle.height > 10) {
                                    shape->data.rectangle.width -= 10;
                                    shape->data.rectangle.height -= 10;
                                }
                                break;
                            updateShapeOnTexture(renderer, mainTexture, shape);
                        }  
                    }
                    else
                    {
                        switch (event.key.keysym.sym) {
                            // Déplacer le curseur
                            case SDLK_UP: moveCursor(&cursor, 0, -10); break;
                            case SDLK_DOWN: moveCursor(&cursor, 0, 10); break;
                            case SDLK_LEFT: moveCursor(&cursor, -10, 0); break;
                            case SDLK_RIGHT: moveCursor(&cursor, 10, 0); break;
                            // Sélectionner ou désélectionner une forme
                            case SDLK_RETURN:
                                handleCursorSelection(cursor.x, cursor.y);
                                break;

                            // Supprimer une forme sélectionnée
                            case SDLK_DELETE:
                                int selectedIndex = findShapeAtCursor(cursor.x, cursor.y);
                                if (selectedIndex != -1) {
                                    deleteShape(selectedIndex);
                                }
                            break;
                        }
                    }
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDLK_RETURN) {
                    int index = findShapeAtCursor(event.button.x, event.button.y);

                    // Si une forme est sélectionnée et le clic est à l'extérieur, désélectionner
                    for (int i = 0; i < shapeCount; i++) {
                        if (shapes[i].selected) {
                            Shape *shape = &shapes[i];
                            bool isOutside = false;

                            switch (shape->type) {
                                case SHAPE_CIRCLE:
                                    isOutside = !isPointInCircle(event.button.x, event.button.y, 
                                                                shape->data.circle.x, 
                                                                shape->data.circle.y, 
                                                                shape->data.circle.radius);
                                    break;

                                case SHAPE_RECTANGLE:
                                    isOutside = !isPointInRectangle(event.button.x, event.button.y, 
                                                                    shape->data.rectangle.x, 
                                                                    shape->data.rectangle.y, 
                                                                    shape->data.rectangle.width, 
                                                                    shape->data.rectangle.height);
                                    break;
                            }

                            if (isOutside) {
                                shape->selected = false; // Désélectionner
                            }
                        }
                    }

                    // Sélectionner une nouvelle forme si cliqué dessus
                    if (index != -1) {
                        shapes[index].selected = !shapes[index].selected; // Alterner la sélection
                    }
                }
            }   

            // **Effacer l'écran (sans effacer les formes existantes)**
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, mainTexture, NULL, NULL); // Afficher les formes déjà dessinées


            // Redessiner toutes les formes
            for (int i = 0; i < shapeCount; i++) {
                Shape *shape = &shapes[i];

                // Dessiner un rectangle jaune autour des formes sélectionnées
                if (shape->selected) {
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Couleur jaune
                    SDL_Rect selectionRect;
                    switch (shape->type) {
                        case SHAPE_CIRCLE:
                            selectionRect.x = shape->data.circle.x - shape->data.circle.radius - 5;
                            selectionRect.y = shape->data.circle.y - shape->data.circle.radius - 5;
                            selectionRect.w = (shape->data.circle.radius * 2) + 10;
                            selectionRect.h = (shape->data.circle.radius * 2) + 10;
                            break;
                        case SHAPE_RECTANGLE:
                            selectionRect.x = shape->data.rectangle.x - 5;
                            selectionRect.y = shape->data.rectangle.y - 5;
                            selectionRect.w = shape->data.rectangle.width + 10;
                            selectionRect.h = shape->data.rectangle.height + 10;
                            break;
                    }
                    SDL_RenderDrawRect(renderer, &selectionRect);
                }
            }
            // Dessiner le curseur
            renderCursor(renderer, &cursor);

            // Mettre à jour l'écran
            SDL_RenderPresent(renderer);
        }
    }
 
    // Libérer les ressources
    SDL_DestroyTexture(mainTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

