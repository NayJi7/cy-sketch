#include "../files.h/cursorEvents.h"
#include "../files.h/formEvents.h"
#include "../files.h/main.h"

/**
 * @brief Main loop of the application to handle user interactions and render shapes.
 * 
 * This function continuously polls events, processes user inputs, and updates the rendering 
 * of shapes and the cursor. It supports moving, selecting, zooming, rotating, and deleting shapes.
 * The loop runs until the user exits the application.
 * 
 * @param renderer Pointer to the SDL_Renderer used for rendering shapes and the cursor.
 * @param event SDL_Event structure to capture and handle user inputs.
 * @param cursor Custom cursor structure used to represent and track the cursor's position.
 */
void mainLoop(SDL_Renderer *renderer, SDL_Event event, Cursor cursor) {
    SDL_ShowCursor(SDL_DISABLE); // Hide the default system cursor.

    int running = 1; // Main loop control flag.
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0; // Exit the loop on quit event.
                    break;

                case SDL_KEYDOWN: {
                    int dx = 0, dy = 0; // Variables to track cursor movement.

                    // Process key presses for cursor movement and actions.
                    switch (event.key.keysym.sym) {
                        case SDLK_UP:
                            dy = -10; // Move cursor up.
                            break;
                        case SDLK_DOWN:
                            dy = 10; // Move cursor down.
                            break;
                        case SDLK_LEFT:
                            dx = -10; // Move cursor left.
                            break;
                        case SDLK_RIGHT:
                            dx = 10; // Move cursor right.
                            break;
                        case SDLK_ESCAPE:
                            // Deselect all shapes.
                            for (int i = 0; i < shapeCount; i++) {
                                shapes[i].selected = false;
                            }
                            break;

                        case SDLK_PLUS:
                        case SDLK_KP_PLUS:
                            // Zoom in on selected shapes.
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].selected) {
                                    zoomShape(&shapes[i], 1.0);
                                }
                            }
                            break;

                        case SDLK_MINUS:
                        case SDLK_KP_MINUS:
                            // Zoom out on selected shapes.
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].selected) {
                                    zoomShape(&shapes[i], -1.0);
                                }
                            }
                            break;

                        case SDLK_q:
                            // Rotate selected shapes counterclockwise.
                            for (int i = 0; i < shapeCount; i++) {
                                rotateShape(&shapes[i], -5);
                            }
                            break;

                        case SDLK_d:
                            // Rotate selected shapes clockwise.
                            for (int i = 0; i < shapeCount; i++) {
                                rotateShape(&shapes[i], 5);
                            }
                            break;

                        case SDLK_RETURN:
                            // Select shapes at cursor position.
                            for (int i = 0; i < shapeCount; i++) {
                                bool isInside = false;
                                switch (shapes[i].type) {
                                    case SHAPE_RECTANGLE:
                                        isInside = isPointInRectangle(cursor.x, cursor.y,
                                            shapes[i].data.rectangle.x, shapes[i].data.rectangle.y,
                                            shapes[i].data.rectangle.width, shapes[i].data.rectangle.height);
                                        break;
                                    case SHAPE_CIRCLE:
                                        isInside = isPointInCircle(cursor.x, cursor.y,
                                            shapes[i].data.circle.x, shapes[i].data.circle.y,
                                            shapes[i].data.circle.radius);
                                        break;
                                    case SHAPE_ELLIPSE:
                                        isInside = isPointInEllipse(cursor.x, cursor.y,
                                            shapes[i].data.ellipse.x, shapes[i].data.ellipse.y,
                                            shapes[i].data.ellipse.rx, shapes[i].data.ellipse.ry);
                                        break;
                                    case SHAPE_LINE:
                                        isInside = isPointInLine(cursor.x, cursor.y,
                                            shapes[i].data.line.x1, shapes[i].data.line.y1,
                                            shapes[i].data.line.x2, shapes[i].data.line.y2,
                                            shapes[i].data.line.thickness);
                                        break;
                                    case SHAPE_ROUNDED_RECTANGLE:
                                        isInside = isPointInRoundedRectangle(cursor.x, cursor.y,
                                            shapes[i].data.rounded_rectangle.x1, shapes[i].data.rounded_rectangle.y1,
                                            shapes[i].data.rounded_rectangle.x2, shapes[i].data.rounded_rectangle.y2,
                                            shapes[i].data.rounded_rectangle.radius);
                                        break;
                                    case SHAPE_POLYGON:
                                        isInside = isPointInPolygon(cursor.x, cursor.y,
                                            shapes[i].data.polygon.cx, shapes[i].data.polygon.cy,
                                            shapes[i].data.polygon.radius, shapes[i].data.polygon.sides);
                                        break;
                                    case SHAPE_ARC:
                                        isInside = isPointInArc(cursor.x, cursor.y,
                                            shapes[i].data.arc.x, shapes[i].data.arc.y,
                                            shapes[i].data.arc.radius,
                                            shapes[i].data.arc.start_angle,
                                            shapes[i].data.arc.end_angle);
                                        break;
                                }
                                shapes[i].selected = isInside;
                            }
                            break;

                        case SDLK_BACKSPACE:
                            // Delete selected shape.
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

                    // Move the cursor and selected shapes based on key input.
                    moveCursor(&cursor, dx, dy);
                    moveSelectedShapes(shapes, shapeCount, dx, dy);
                }
                break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        // Handle shape selection using the cursor.
                        handleCursorSelection(event.button.x, event.button.y);
                    }
                    break;

                case SDL_MOUSEMOTION:
                    // Update cursor position and move selected shapes with the mouse.
                    cursor.x = event.motion.x;
                    cursor.y = event.motion.y;
                    moveShapesWithMouse(shapes, shapeCount, &event, &cursor);
                    break;
            }
        }

        // Clear the screen and set a background color.
        SDL_SetRenderDrawColor(renderer, 229, 229, 229, 255);
        SDL_RenderClear(renderer);

        // Render all shapes.
        for (int i = 0; i < shapeCount; i++) {
            renderShape(renderer, &shapes[i]);
        }

        // Render the custom cursor.
        renderCursor(renderer, &cursor);

        // Present the updated frame.
        SDL_RenderPresent(renderer);
    }

    SDL_ShowCursor(SDL_ENABLE); // Restore the default system cursor.
}

/**
 * @brief Handles SDL events, including quitting the program.
 * 
 * @param renderer SDL renderer instance used for drawing.
 * @param texture SDL texture associated with the renderer.
 * 
 * @return int Returns -1 if a quit event is detected; otherwise 0.
 */
int handleEvents(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyTexture(texture);
            SDL_Quit();
            return -1;
        }
    }
    return 0;
}

/**
 * @brief Initializes a new cursor with specified properties.
 * 
 * @param x Initial x-coordinate of the cursor.
 * @param y Initial y-coordinate of the cursor.
 * @param color Color of the cursor in SDL_Color format.
 * @param thickness Thickness of the cursor outline.
 * @param visible Visibility state of the cursor (true or false).
 * 
 * @return Cursor The newly created cursor instance.
 */
Cursor createCursor(int x, int y, SDL_Color color, int thickness, bool visible) {
    Cursor cursor;
    cursor.x = x;
    cursor.y = y;
    cursor.color = color;
    cursor.thickness = thickness;
    cursor.visible = visible;
    return cursor;
}

/**
 * @brief Renders the cursor on the screen.
 * 
 * @param renderer SDL renderer instance used for drawing.
 * @param cursor Pointer to the cursor instance to render.
 */
void renderCursor(SDL_Renderer *renderer, const Cursor *cursor) {
    if (!cursor->visible) return; // Ne rien dessiner si le curseur n'est pas visible

    // Définir la couleur de la flèche
    SDL_SetRenderDrawColor(renderer, cursor->color.r, cursor->color.g, cursor->color.b, cursor->color.a);

    // Dessiner le triangle représentant la pointe du curseur
    SDL_Point triangle[4] = {
        {cursor->x, cursor->y},                                // Pointe
        {cursor->x - cursor->thickness, cursor->y + 2 * cursor->thickness}, // Coin gauche
        {cursor->x + cursor->thickness, cursor->y + 2 * cursor->thickness}, // Coin droit
        {cursor->x, cursor->y}                                 // Retour à la pointe
    };
    SDL_RenderDrawLines(renderer, triangle, 4);

    // Dessiner un point pour la base du curseur
    SDL_Rect tail = {
        cursor->x - cursor->thickness / 2,
        cursor->y + 3 * cursor->thickness,
        cursor->thickness,
        cursor->thickness
    };
    SDL_RenderFillRect(renderer, &tail);
}

/**
 * @brief Moves the cursor by a specified offset.
 * 
 * @param cursor Pointer to the cursor instance.
 * @param dx Horizontal movement in pixels.
 * @param dy Vertical movement in pixels.
 */
void moveCursor(Cursor *cursor, int dx, int dy) {
    cursor->x += dx;
    cursor->y += dy;
}

/**
 * @brief Finds the shape at the given cursor coordinates.
 * 
 * @param x X-coordinate of the cursor.
 * @param y Y-coordinate of the cursor.
 * 
 * @return int Index of the shape at the cursor position, or -1 if none is found.
 */
int findShapeAtCursor(int x, int y) {
    for (int i = 0; i < shapeCount; i++) {
        Shape *shape = &shapes[i];

        // Vérifiez que le typeForm est valide pour éviter des formes incorrectes
        if ((strcmp(shape->typeForm, "filled") != 0) && (strcmp(shape->typeForm, "empty") != 0)) {
            continue; // Ignorer cette forme
        }

        switch (shape->type) 
        {
            case SHAPE_CIRCLE:
                if (isPointInCircle(x, y, shape->data.circle.x, shape->data.circle.y, shape->data.circle.radius)) return i;
                break;
            case SHAPE_RECTANGLE:
                if (isPointInRectangle(x, y, shape->data.rectangle.x, shape->data.rectangle.y, shape->data.rectangle.width, shape->data.rectangle.height)) return i;
                break;
            case SHAPE_ELLIPSE:
                if (isPointInEllipse(x, y, shape->data.ellipse.x, shape->data.ellipse.y, shape->data.ellipse.rx, shape->data.ellipse.ry)) return i;
                break;
            case SHAPE_ARC:
                if(isPointInArc(x, y, shape->data.arc.x, shape->data.arc.y, shape->data.arc.radius, shape->data.arc.start_angle, shape->data.arc.end_angle)) return i;
                break;
            case SHAPE_ROUNDED_RECTANGLE:
                if(isPointInRoundedRectangle(x, y, shape->data.rounded_rectangle.x1, shape->data.rounded_rectangle.y1, shape->data.rounded_rectangle.x2, shape->data.rounded_rectangle.y2, shape->data.rounded_rectangle.radius)) return i;
                break;
            case SHAPE_POLYGON:
                if(isPointInPolygon(x, y, shape->data.polygon.cx, shape->data.polygon.cy, shape->data.polygon.radius, shape->data.polygon.sides)) return i;
                break;
            case SHAPE_LINE:
                if(isPointInLine(x, y, shape->data.line.x1, shape->data.line.y1, shape->data.line.x2, shape->data.line.y2, shape->data.line.thickness)) return i;        
                break;
        }
    }
    return -1; // Aucune forme trouvée
}



/**
 * @brief Common parameters for functions.
 * 
 * @param cursorX X-coordinate of the cursor.
 * @param cursorY Y-coordinate of the cursor.
*/


/**
 * @brief Handles the selection or deselection of shapes based on cursor position.
 * 
 * When the cursor is positioned over a shape and the mouse is clicked:
 * - If the shape is not selected, it will be selected.
 * - If the shape is already selected, it will be deselected.
 * 
 * @param cursorX The x-coordinate of the cursor.
 * @param cursorY The y-coordinate of the cursor.
 */
void handleCursorSelection(int cursorX, int cursorY) {
    int index = findShapeAtCursor(cursorX, cursorY);
    if (index != -1) {
        // Toggle selection status of the shape
        shapes[index].selected = !shapes[index].selected;
    }
}

void handleShapeDeletion(int cursorX, int cursorY) {
    int index = findShapeAtCursor(cursorX, cursorY);
    if (index != -1) {
        deleteShape(index);
    }
}
