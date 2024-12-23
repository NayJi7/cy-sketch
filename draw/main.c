#include "../files.h/main.h"

/**
 * @brief Sets the background color of the SDL window.
 * 
 * @param renderer The SDL renderer used to set the color.
 * @param color The SDL_Color struct representing the color (RGBA) to be set.
 * 
 * @return int Returns 0 on success, -1 on failure.
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
    SDL_Color cursorColor = {255, 0, 0, 255};
    Cursor cursor = createCursor(400, 300, cursorColor, 5, true);

    // Créer une texture principale pour dessiner
    SDL_Texture* mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 800, 600);

    // Remplir la texture de fond noir
    SDL_SetRenderTarget(renderer, mainTexture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    
    if(drawShape(renderer, mainTexture, "animated", "rectangle", 200, 200, 200, 50, 0xFF00FF00, "filled") == -1) return 0;
    if(drawShape(renderer, mainTexture, "instant", "circle", 300, 270, 60, 0xFF00FF00, "filled") == -1) return 0;
    if(drawShape(renderer, mainTexture, "instant", "roundedRectangle", 400, 200, 200, 150, 20, 0xFF00FF00, "filled") == -1) return 0;
    if(drawShape(renderer, mainTexture, "instant", "ellipse", 500, 370, 70, 50, 0xFF00FF00, "filled") == -1) return 0;
    //if(drawShape(renderer, mainTexture, "animated", "arc", 400, 300, 100, 0, 180, 0xFF00FF00, "filled") == -1) return 0;
    //if(drawShape(renderer, mainTexture, "instant", "polygon", 630, 130, 100, 12, 0xFF808080, "filled") == -1) return 0;
    //if(drawShape(renderer, mainTexture, "animated", "line", 100, 100, 400, 300, 2, 0xFF00FF00, "filled") == -1) return 0;

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, mainTexture, NULL, NULL);
    SDL_RenderPresent(renderer);

    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;

                case SDL_KEYDOWN:
                    int dx = 0, dy = 0;

                    switch (event.key.keysym.sym) {
                        case SDLK_UP: // deplace le curseur 
                            dy = -10;
                            break;
                        case SDLK_DOWN:
                            dy = 10;
                            break;
                        case SDLK_LEFT:
                            dx = -10;
                            break;
                        case SDLK_RIGHT:
                            dx = 10;
                            break;
                        case SDLK_ESCAPE:
                            // Désélectionner toutes les formes
                            for (int i = 0; i < shapeCount; i++) {
                                shapes[i].selected = false;
                            }
                            break;

                        case SDLK_PLUS: // zoom 
                        case SDLK_KP_PLUS:
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].selected) {
                                    if (shapes[i].type == SHAPE_RECTANGLE) {
                                        shapes[i].data.rectangle.width += 10;
                                        shapes[i].data.rectangle.height += 10;
                                    } 
                                    else if (shapes[i].type == SHAPE_CIRCLE) {
                                        shapes[i].data.circle.radius += 5;
                                    }
                                    else if (shapes[i].type == SHAPE_ELLIPSE) {
                                        shapes[i].data.ellipse.rx += 5;
                                        shapes[i].data.ellipse.ry += 5;
                                    }
                                    else if (shapes[i].type == SHAPE_ROUNDED_RECTANGLE) {
                                        shapes[i].data.rounded_rectangle.x2 += 10;
                                        shapes[i].data.rounded_rectangle.y2 += 10;
                                        shapes[i].data.rounded_rectangle.radius += 2; // Augmenter le rayon
                                    }
                                }
                            }
                            break;

                        case SDLK_MINUS: // rapeticis
                        case SDLK_KP_MINUS:
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].selected) {
                                    if (shapes[i].type == SHAPE_RECTANGLE) {
                                        if (shapes[i].data.rectangle.width > 10 && shapes[i].data.rectangle.height > 10) {
                                            shapes[i].data.rectangle.width -= 10;
                                            shapes[i].data.rectangle.height -= 10;
                                        }
                                    } 
                                    else if (shapes[i].type == SHAPE_CIRCLE) {
                                        if (shapes[i].data.circle.radius > 10) {
                                            shapes[i].data.circle.radius -= 5;
                                        }
                                    }
                                    else if (shapes[i].type == SHAPE_ELLIPSE) {
                                        shapes[i].data.ellipse.rx -= 5;
                                        shapes[i].data.ellipse.ry -= 5;
                                    }
                                    else if (shapes[i].type == SHAPE_ROUNDED_RECTANGLE) {
                                        if ((shapes[i].data.rounded_rectangle.x2 - shapes[i].data.rounded_rectangle.x1) > 10 &&
                                            (shapes[i].data.rounded_rectangle.y2 - shapes[i].data.rounded_rectangle.y1) > 10) {
                                            shapes[i].data.rounded_rectangle.x2 -= 10;
                                            shapes[i].data.rounded_rectangle.y2 -= 10;
                                            if (shapes[i].data.rounded_rectangle.radius > 2) {
                                                shapes[i].data.rounded_rectangle.radius -= 2;
                                            }
                                        }
                                    }
                                }
                            }
                            break;
                        //marche pas 
                        case SDLK_a: // Rotation anti-horaire
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].selected) {
                                    shapes[i].rotation -= 10;
                                    if (shapes[i].rotation < 0) shapes[i].rotation += 360; // Gérer la rotation négative
                                }
                            }
                            break;

                        case SDLK_d: // Rotation horaire
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].selected) {
                                    shapes[i].rotation += 50;
                                    if (shapes[i].rotation >= 360) shapes[i].rotation -= 360; // Gérer les dépassements
                                }
                            }
                            break;

                        case SDLK_RETURN: // selectionne ma forme
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].type == SHAPE_RECTANGLE) {
                                    if (cursor.x >= shapes[i].data.rectangle.x &&
                                        cursor.x <= shapes[i].data.rectangle.x + shapes[i].data.rectangle.width &&
                                        cursor.y >= shapes[i].data.rectangle.y &&
                                        cursor.y <= shapes[i].data.rectangle.y + shapes[i].data.rectangle.height) {
                                        shapes[i].selected = true;
                                    } else {
                                        shapes[i].selected = false;
                                    }
                                } 
                                else if (shapes[i].type == SHAPE_CIRCLE) {
                                    int dx = cursor.x - shapes[i].data.circle.x;
                                    int dy = cursor.y - shapes[i].data.circle.y;
                                    if ((dx * dx + dy * dy) <= (shapes[i].data.circle.radius * shapes[i].data.circle.radius)) {
                                        shapes[i].selected = true;
                                    } else {
                                        shapes[i].selected = false;
                                    }
                                } 
                                else if (shapes[i].type == SHAPE_ELLIPSE) {
                                    int dx = cursor.x - shapes[i].data.ellipse.x;
                                    int dy = cursor.y - shapes[i].data.ellipse.y;
                                    // Vérifie si le point est dans l'ellipse en respectant l'équation standard
                                    if (((dx * dx) / (shapes[i].data.ellipse.rx * shapes[i].data.ellipse.rx) +
                                        (dy * dy) / (shapes[i].data.ellipse.ry * shapes[i].data.ellipse.ry)) <= 1) {
                                        shapes[i].selected = true;
                                    } else {
                                        shapes[i].selected = false;
                                    }
                                }
                                else if (shapes[i].type == SHAPE_ROUNDED_RECTANGLE) {
                                    if (isPointInRoundedRectangle(cursor.x, cursor.y,
                                                                shapes[i].data.rounded_rectangle.x1,
                                                                shapes[i].data.rounded_rectangle.y1,
                                                                shapes[i].data.rounded_rectangle.x2 - shapes[i].data.rounded_rectangle.x1,
                                                                shapes[i].data.rounded_rectangle.y2 - shapes[i].data.rounded_rectangle.y1,
                                                                shapes[i].data.rounded_rectangle.radius)) {
                                        shapes[i].selected = true;
                                    } else {
                                        shapes[i].selected = false;
                                    }
                                }
                            }
                            break;

                        case SDLK_BACKSPACE: // Supprimer la forme sélectionnée
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].selected) {
                                    deleteShape(i);
                                    break;
                                }
                            }
                            break;

                        default:
                            break;
                    }

                    moveCursor(&cursor, dx, dy);

                    for (int i = 0; i < shapeCount; i++) { // deplacement des formes 
                        if (shapes[i].selected) {
                            if (shapes[i].type == SHAPE_RECTANGLE) {
                                shapes[i].data.rectangle.x += dx;
                                shapes[i].data.rectangle.y += dy;
                            } 
                            else if (shapes[i].type == SHAPE_CIRCLE) {
                                shapes[i].data.circle.x += dx;
                                shapes[i].data.circle.y += dy;
                            }
                            else if (shapes[i].type == SHAPE_ELLIPSE) {
                                shapes[i].data.ellipse.x += dx;
                                shapes[i].data.ellipse.y += dy;
                            }
                            else if (shapes[i].type ==  SHAPE_ROUNDED_RECTANGLE) {
                                shapes[i].data.rounded_rectangle.x1 += dx;
                                shapes[i].data.rounded_rectangle.y1 += dy;
                                shapes[i].data.rounded_rectangle.x2 += dx;
                                shapes[i].data.rounded_rectangle.y2 += dy;
                            }
                            
                        }
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN: // selection forme avec souris
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        handleCursorSelection(event.button.x, event.button.y);
                    }
                    break;
        
                case SDL_MOUSEMOTION: // deplacement forme avec souris 
                    for (int i = 0; i < shapeCount; i++) {
                        if (shapes[i].selected) {
                            if (shapes[i].type == SHAPE_RECTANGLE) {
                                shapes[i].data.rectangle.x = event.motion.x - shapes[i].data.rectangle.width / 2;
                                shapes[i].data.rectangle.y = event.motion.y - shapes[i].data.rectangle.height / 2;

                                cursor.x = shapes[i].data.rectangle.x + shapes[i].data.rectangle.width / 2;
                                cursor.y = shapes[i].data.rectangle.y + shapes[i].data.rectangle.height / 2;
                            } 
                            else if (shapes[i].type == SHAPE_CIRCLE) {
                                shapes[i].data.circle.x = event.motion.x;
                                shapes[i].data.circle.y = event.motion.y;

                                cursor.x = shapes[i].data.circle.x;
                                cursor.y = shapes[i].data.circle.y;
                            }
                            else if (shapes[i].type == SHAPE_ELLIPSE) {
                                shapes[i].data.ellipse.x = event.motion.x;
                                shapes[i].data.ellipse.y = event.motion.y;

                                cursor.x = shapes[i].data.ellipse.x;
                                cursor.y = shapes[i].data.ellipse.y;
                            }
                            else if (shapes[i].type == SHAPE_ROUNDED_RECTANGLE) {
                                shapes[i].data.rounded_rectangle.x1 = event.motion.x - 50;
                                shapes[i].data.rounded_rectangle.y1 = event.motion.y - 50;
                                shapes[i].data.rounded_rectangle.x2 = event.motion.x + 50;
                                shapes[i].data.rounded_rectangle.y2 = event.motion.y + 50;

                                cursor.x = (shapes[i].data.rounded_rectangle.x1 + shapes[i].data.rounded_rectangle.x2) / 2;
                                cursor.y = (shapes[i].data.rounded_rectangle.y1 + shapes[i].data.rounded_rectangle.y2) / 2;
                            }

                        }
                    }
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < shapeCount; i++) {
            renderShape(renderer, &shapes[i]);
        }

        renderCursor(renderer, &cursor);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(mainTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}
