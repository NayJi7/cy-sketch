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
    SDL_Color cursorColor = {255, 255, 255, 255};
    Cursor cursor = createCursor(400, 300, cursorColor, 5, true);

    // Créer une texture principale pour dessiner
    SDL_Texture* mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 800, 600);

    // Remplir la texture de fond noir
    SDL_SetRenderTarget(renderer, mainTexture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    //rectangles ------
    //if(drawShape(renderer, mainTexture, "instant", "rectangle", 100, 100, 200, 50, 0xFF00FF00, "filled") == -1) return 0;
    //if(drawShape(renderer, mainTexture, "instant", "rectangle", 200, 200, 200, 50, 0xFF00FF00, "empty") == -1) return 0;
    //circles ------
    //if(drawShape(renderer, mainTexture, "instant", "circle", 200, 170, 60, 0xFF00FF00, "filled") == -1) return 0;
    //if(drawShape(renderer, mainTexture, "instant", "circle", 200, 170, 60, 0xFF00FF00, "empty") == -1) return 0;
    //ellipses ------
    //if(drawShape(renderer, mainTexture, "instant", "ellipse", 400, 270, 70, 50, 0xFF00FF00, "fille") == -1) return 0;
    //if(drawShape(renderer, mainTexture, "instant", "ellipse", 300, 370, 70, 50, 0xFF00FF00, "empty") == -1) return 0;
    //lines ------
    //if(drawShape(renderer, mainTexture, "instant", "line", 200, 200, 300, 200, 2, 0xFF00FF00, "filled") == -1) return 0;
    //if(drawShape(renderer, mainTexture, "instant", "line", 100, 100, 400, 300, 20, 0xFF00FF00, "empty") == -1) return 0;

    //roundedrectangles ------
    //NE MARCHE PAS POUR SELECTIONNER
        //if(drawShape(renderer, mainTexture, "instant", "roundedRectangle", 300, 100, 200, 150, 20, 0xFF00FF00, "empty") == -1) return 0;
        //if(drawShape(renderer, mainTexture, "instant", "roundedRectangle", 400, 200, 200, 150, 20, 0xFF00FF00, "filled") == -1) return 0;

    // A FAIRE : 
        //arcs ------
        //if(drawShape(renderer, mainTexture, "animated", "arc", 200, 400, 100, 0, 180, 0xFF00FF00, "filled") == -1) return 0;
        //if(drawShape(renderer, mainTexture, "animated", "arc", 400, 200, 100, 0, 180, 0xFF00FF00, "empty") == -1) return 0;
        //if(drawShape(renderer, mainTexture, "instant", "polygon", 530, 130, 100, 12, 0xFF808080, "filled") == -1) return 0;
        //if(drawShape(renderer, mainTexture, "instant", "polygon", 330, 130, 100, 12, 0xFF808080, "filled") == -1) return 0;
    

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, mainTexture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_ShowCursor(SDL_DISABLE);

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
                                    zoomShape(&shapes[i], 1.0); // Zoom avant
                                }
                            }
                            break;

                        case SDLK_MINUS: // rapeticis
                        case SDLK_KP_MINUS:
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].selected) {
                                    zoomShape(&shapes[i], -1.0); // Zoom avant
                                }
                            }
                            break;
 
                        case SDLK_q: // Rotation anti-horaire
                            for (int i = 0; i < shapeCount; i++) {
                                rotateShape(&shapes[i], -5);
                            }
                            break;

                        case SDLK_d: // Rotation horaire
                            for (int i = 0; i < shapeCount; i++) {
                                rotateShape(&shapes[i], 5);
                            }
                            break;

                        case SDLK_RETURN: // selectionne ma forme
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].type == SHAPE_RECTANGLE) {
                                    if (isPointInRectangle(cursor.x, cursor.y, shapes[i].data.rectangle.x, shapes[i].data.rectangle.y, shapes[i].data.rectangle.width, shapes[i].data.rectangle.height)) {
                                        shapes[i].selected = true;
                                    } else {
                                        shapes[i].selected = false;
                                    }
                                } 
                                else if (shapes[i].type == SHAPE_CIRCLE) {
                                    if(isPointInCircle(cursor.x, cursor.y, shapes[i].data.circle.x, shapes[i].data.circle.y, shapes[i].data.circle.radius)) {
                                        shapes[i].selected = true;
                                    } else {
                                        shapes[i].selected = false;
                                    }
                                } 
                                else if (shapes[i].type == SHAPE_ELLIPSE) {
                                    if (isPointInEllipse(cursor.x, cursor.y, 
                                        shapes[i].data.ellipse.x, 
                                        shapes[i].data.ellipse.y, 
                                        shapes[i].data.ellipse.rx, 
                                        shapes[i].data.ellipse.ry)) {
                                        shapes[i].selected = true;
                                    } else {
                                        shapes[i].selected = false;
                                    }
                                }
                                else if (shapes[i].type == SHAPE_LINE) {
                                    // Calculer si le curseur est proche de la ligne
                                    if (isPointInLine(cursor.x, cursor.y, 
                                                    shapes[i].data.line.x1, shapes[i].data.line.y1, 
                                                    shapes[i].data.line.x2, shapes[i].data.line.y2, 
                                                    shapes[i].data.line.thickness)) {
                                        shapes[i].selected = true;
                                    } else {
                                        shapes[i].selected = false;
                                    }
                                }
                                else if (shapes[i].type == SHAPE_ROUNDED_RECTANGLE) {
                                    if (isPointInRoundedRectangle(cursor.x, cursor.y, 
                                    shapes[i].data.rounded_rectangle.x1,
                                    shapes[i].data.rounded_rectangle.y1,
                                    shapes[i].data.rounded_rectangle.x2,
                                    shapes[i].data.rounded_rectangle.y2,
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

                    // Déplacement des formes avec les touches directionnelles
                    moveSelectedShapes(shapes, shapeCount, dx, dy);

                case SDL_MOUSEBUTTONDOWN: // selection forme avec souris
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        handleCursorSelection(event.button.x, event.button.y);
                    }
                    break;
        
                case SDL_MOUSEMOTION: // deplacement forme avec souris 
                    cursor.x = event.motion.x;
                    cursor.y = event.motion.y;

                    moveShapesWithMouse(shapes, shapeCount, &event, &cursor);
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
    SDL_ShowCursor(SDL_ENABLE);


    SDL_DestroyTexture(mainTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}
