#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include "../files.h/main.h"

#include "../files.h/formEvents.h"
#include "../files.h/cursorEvents.h"
#include "../files.h/animations.h"
#include "../files.h/colors.h"
#include "../files.h/game.h"

// ANSI escape codes for colors
#define RED_COLOR "-#red "

#ifndef DEBUG   // DEBUG is defined in the compilation command (makefile DEBUG=1)
#define DEBUG 1 
#endif

// Definition of the global variable
char lastKeyPressed[32] = "";

void cleanup(SDL_Texture* mainTexture, SDL_Renderer* renderer, SDL_Window* window){
    if (mainTexture) SDL_DestroyTexture(mainTexture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

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
void mainLoop(SDL_Window *window, SDL_Renderer *renderer, SDL_Event event, Cursor cursor, int bgcolorR, int bgcolorG, int bgcolorB) {
    #if DEBUG
        printf("Debug: Starting main loop\n");
    #endif

    SDL_ShowCursor(SDL_DISABLE);
    /// Ensuite initialiser SDL_ttf
    if (TTF_Init() < 0) {
        printf("%sExecutionError: Failed to initialize TTF: %s\n", RED_COLOR, TTF_GetError());
        return;
    }

    // Load the font
    const char* font_paths[] = {
        "SDL/fonts/DejaVuMathTeXGyre.ttf",
        "../SDL/fonts/DejaVuMathTeXGyre.ttf",
        "../../SDL/fonts/DejaVuMathTeXGyre.ttf"
    };
    
    TTF_Font *font = NULL;
    for (int i = 0; i < 3; i++) {
        font = TTF_OpenFont(font_paths[i], 16);
        if (font) break;
    }
    
    if (!font) {
        printf("%sExecutionError: Failed to open font: %s\n", RED_COLOR, TTF_GetError);
        TTF_Quit();
        return;
    }

    // Initialiser le jeu
    GameState gameState;
    initGame(&gameState);
    
    Uint32 lastTime = SDL_GetTicks();
    int running = 1;
    
    while (running) {
        // Calculer le deltaTime pour updateGame
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0; // Exit the loop on quit event.
                    break;

                
                case SDL_TEXTINPUT: {
                    strncpy(lastKeyPressed, event.text.text, sizeof(lastKeyPressed) - 1);
                    lastKeyPressed[sizeof(lastKeyPressed) - 1] = '\0';  // Ensure null termination
                    
                    if (DEBUG) {
                        printf("Key Pressed - %s\n", event.text.text);
                    }
                    int dx = 0, dy = 0; // Variables to track cursor movement.

                    // Check for special characters
                    if (strcmp(event.text.text, "z") == 0) {
                        // Move selected shape up in z-order
                        if (DEBUG) {
                            printf("Move selected shape up\n\n");
                        }
                        moveShapeUp();
                    }
                    else if (strcmp(event.text.text, "s") == 0) {
                        // Move selected shape down in z-order
                        if (DEBUG) {
                            printf("Move selected shape down\n\n");
                        }
                        moveShapeDown();
                    }
                    else if (strcmp(event.text.text, "a") == 0) {
                        // Toggle animation for selected shape
                        if (DEBUG) {
                            printf("Toggle animation for selected shape\n\n");
                        }
                        toggleAnimation();
                    }
                    else if (strcmp(event.text.text, "n") == 0) {
                        // Toggle animation for all shapes
                        if (DEBUG) {
                            printf("Toggle animation for all shapes\n\n");
                        }
                        for (int i = 0; i < shapeCount; i++) {
                            if(shapes[i].isAnimating) {
                                shapes[i].isAnimating = false;
                            }
                        }
                    }
                    else if (strcmp(event.text.text, "r") == 0) {
                        // Reset selected shape to initial state
                        if (DEBUG) {
                            printf("Reset selected shape to initial state\n\n");
                        }
                        for (int i = 0; i < shapeCount; i++) {
                            if (shapes[i].selected) {
                                resetShape(&shapes[i]);
                                break;
                            }
                        }
                    }
                    else if (strcmp(event.text.text, "e") == 0) {
                        if (DEBUG) {
                            printf("Toggle selection of shape under cursor\n\n");
                        }
                        int topmostShapeIndex = -1;
                        // Check if cursor is over any shape, keeping track of the last (topmost) one
                        for (int i = 0; i < shapeCount; i++) {
                            bool isInside = false;
                            switch (shapes[i].type) {
                                case SHAPE_RECTANGLE:
                                    isInside = isPointInRectangle(cursor.x, cursor.y,
                                        shapes[i].data.rectangle.x, shapes[i].data.rectangle.y,
                                        shapes[i].data.rectangle.width, shapes[i].data.rectangle.height);
                                    break;
                                case SHAPE_SQUARE:
                                    isInside = isPointInSquare(cursor.x, cursor.y,
                                        shapes[i].data.square.x, shapes[i].data.square.y,
                                        shapes[i].data.square.c);
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
                                case SHAPE_TRIANGLE:
                                    isInside = isPointInTriangle(cursor.x, cursor.y,
                                        shapes[i].data.triangle.cx, shapes[i].data.triangle.cy,
                                        shapes[i].data.triangle.radius);
                                    break;
                                case SHAPE_ARC:
                                    isInside = isPointInArc(cursor.x, cursor.y,
                                        shapes[i].data.arc.x, shapes[i].data.arc.y,
                                        shapes[i].data.arc.radius,
                                        shapes[i].data.arc.start_angle,
                                        shapes[i].data.arc.end_angle);
                                    break;
                            }
                            
                            if (isInside) {
                                topmostShapeIndex = i;  // Keep track of the last (topmost) shape found
                            }
                        }
                        
                        // If we found a shape under the cursor
                        if (topmostShapeIndex != -1) {
                            if (shapes[topmostShapeIndex].selected) {
                                // If the topmost shape is already selected, just deselect it
                                shapes[topmostShapeIndex].selected = false;
                                shapes[topmostShapeIndex].isAnimating = false;
                            } else {
                                // If the topmost shape is not selected, select it and deselect others
                                for (int i = 0; i < shapeCount; i++) {
                                    shapes[i].selected = (i == topmostShapeIndex);
                                    if (!shapes[i].selected) {
                                        shapes[i].isAnimating = false;
                                    }
                                }
                            }
                        } else {
                            // If no shape under cursor, deselect all
                            for (int i = 0; i < shapeCount; i++) {
                                shapes[i].selected = false;
                                shapes[i].isAnimating = false;
                            }
                        }
                    }
                    else if (strcmp(event.text.text, "q") == 0) {
                        // Rotate selected shapes counterclockwise
                        if (DEBUG) {
                            printf("Rotate selected shapes by -5 degrees\n\n");
                        }
                        for (int i = 0; i < shapeCount; i++) {
                            rotateShape(&shapes[i], -5);
                        }
                    }
                    else if (strcmp(event.text.text, "d") == 0) {
                        // Rotate selected shapes clockwise
                        if (DEBUG) {
                            printf("Rotate selected shapes by 5 degrees\n\n");
                        }
                        for (int i = 0; i < shapeCount; i++) {
                            rotateShape(&shapes[i], 5);
                        }
                    }
                    else if (strcmp(event.text.text, "*") == 0) {
                        // Zoom in on selected shapes
                        if (DEBUG) {
                            printf("Zoom in on selected shapes\n\n");
                        }
                        for (int i = 0; i < shapeCount; i++) {
                            if (shapes[i].selected) {
                                zoomShape(&shapes[i], 1.0);
                            }
                        }
                    }
                    else if (strcmp(event.text.text, "/") == 0) {
                        // Zoom out on selected shapes
                        if (DEBUG) {
                            printf("Zoom out on selected shapes\n\n");
                        }
                        for (int i = 0; i < shapeCount; i++) {
                            if (shapes[i].selected) {
                                zoomShape(&shapes[i], -1.0);
                            }
                        }
                    }

                    else if (strcmp(event.text.text, "+") == 0) {
                        // Cycle forward through animation modes for selected shapes
                        if (DEBUG) {
                            printf("Cycle animation mode forward for selected shapes\n\n");
                        }
                        for (int i = 0; i < shapeCount; i++) {
                            if (shapes[i].selected) {
                                if (shapes[i].animation_parser == ANIM_NONE) {
                                    shapes[i].animation_parser = ANIM_ROTATE;
                                } else if (shapes[i].animation_parser == ANIM_ROTATE) {
                                    shapes[i].animation_parser = ANIM_ZOOM;
                                } else if (shapes[i].animation_parser == ANIM_ZOOM) {
                                    shapes[i].animation_parser = ANIM_COLOR;
                                } else if (shapes[i].animation_parser == ANIM_COLOR) {
                                    shapes[i].animation_parser = ANIM_BOUNCE;
                                } else if (shapes[i].animation_parser == ANIM_BOUNCE) {
                                    shapes[i].animation_parser = ANIM_NONE;
                                } else {
                                    shapes[i].animation_parser = ANIM_NONE;
                                }
                            }
                        }
                    }
                    
                    else if (strcmp(event.text.text, "-") == 0) {
                        // Cycle through animation modes (None <- Rotate <- Zoom <- Color <- Bounce)
                        if (DEBUG) {
                            printf("Cycle animation mode backward for selected shapes\n\n");
                        }
                        for (int i = 0; i < shapeCount; i++) {
                            if (shapes[i].selected) {
                                if (shapes[i].animation_parser == ANIM_NONE) {
                                    shapes[i].animation_parser = ANIM_BOUNCE;
                                } else if (shapes[i].animation_parser == ANIM_BOUNCE) {
                                    shapes[i].animation_parser = ANIM_COLOR;
                                } else if (shapes[i].animation_parser == ANIM_COLOR) {
                                    shapes[i].animation_parser = ANIM_ZOOM;
                                } else if (shapes[i].animation_parser == ANIM_ZOOM) {
                                    shapes[i].animation_parser = ANIM_ROTATE;
                                } else if (shapes[i].animation_parser == ANIM_ROTATE) {
                                    shapes[i].animation_parser = ANIM_NONE;
                                } else {
                                    shapes[i].animation_parser = ANIM_NONE;
                                }
                            }
                        }
                    }

                    // Move the cursor and selected shapes based on key input.
                    moveCursor(&cursor, dx, dy);
                    renderCursorCoordinates(renderer, font, cursor.x, cursor.y);
                    moveSelectedShapes(shapes, shapeCount, dx, dy);
                }
                break;

                case SDL_KEYDOWN:
                    if (event.key.repeat != 0) break;
                    switch (event.key.keysym.sym) {
                        case SDLK_g:  // Activer le mode jeu
                            if (!gameState.isGameMode) {
                                initGame(&gameState);
                                gameState.isGameMode = true;  // Entrer dans le mode jeu
                                if (DEBUG) printf("Game mode activated!\n");
                            }
                            break;

                        case SDLK_KP_ENTER:
                        case SDLK_RETURN:
                            if (gameState.isGameMode) {
                                // Mode jeu : démarrer la partie
                                if (!gameState.isPlaying) {
                                    gameState.isPlaying = true;
                                    if (DEBUG) printf("Game started!\n");
                                }
                            } else {
                                // Mode normal : gérer les animations
                                if (DEBUG) {
                                    printf("Key Pressed - %s\n", SDL_GetKeyName(event.key.keysym.sym));
                                    printf("Apply animation to selected shapes\n\n");
                                }
                                for (int i = 0; i < shapeCount; i++) {
                                    if (shapes[i].selected) {
                                        if(shapes[i].isAnimating) {
                                            shapes[i].isAnimating = false;
                                        }
                                        applyAnimation(&shapes[i]);
                                    }
                                }
                            }
                            break;

                        case SDLK_SPACE:  // Quitter le mode jeu
                            if (gameState.isGameMode) {
                                gameState.isGameMode = false;
                                gameState.isPlaying = false;
                                restoreShapes(&gameState);  // Restaurer les formes
                                if (DEBUG) printf("Game mode exited!\n");
                            }
                            break;

                        case SDLK_ESCAPE:  // Quitter l'application
                            running = 0;
                            break;
                        case SDLK_RIGHT:
                            if (DEBUG) {
                                printf("Key Pressed - %s\n", SDL_GetKeyName(event.key.keysym.sym));
                                printf("Move selected shapes by 10 pixels to the right\n\n");
                            }
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].selected) {
                                    moveShape(&shapes[i], 10, 0);
                                }
                            }
                            break;
                        case SDLK_LEFT:
                            if (DEBUG) {
                                printf("Key Pressed - %s\n", SDL_GetKeyName(event.key.keysym.sym));
                                printf("Move selected shapes by 10 pixels to the left\n\n");
                            }
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].selected) {
                                    moveShape(&shapes[i], -10, 0);
                                }
                            }
                            break;  
                        case SDLK_UP:
                            if (DEBUG) {
                                printf("Key Pressed - %s\n", SDL_GetKeyName(event.key.keysym.sym));
                                printf("Move selected shapes by 10 pixels up\n\n");
                            }
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].selected) {
                                    moveShape(&shapes[i], 0, -10);
                                }
                            }
                            break;
                        case SDLK_DOWN:
                            if (DEBUG) {
                                printf("Key Pressed - %s\n", SDL_GetKeyName(event.key.keysym.sym));
                                printf("Move selected shapes by 10 pixels down\n\n");
                            }
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].selected) {
                                    moveShape(&shapes[i], 0, 10);
                                }
                            }
                            break;
                        case SDLK_BACKSPACE:
                            if (DEBUG) {
                                printf("Key Pressed - %s\n", SDL_GetKeyName(event.key.keysym.sym));
                                printf("Remove last animation from selected shapes\n\n");
                            }
                            for (int i = 0; i < shapeCount; i++) {
                                if (shapes[i].selected) {
                                    if(shapes[i].isAnimating) {
                                        shapes[i].isAnimating = false;
                                    }
                                    unapplyAnimation(&shapes[i]);
                                }
                            }
                            break;
                        case SDLK_DELETE:  // Touche SUPPR/DEL
                            if (DEBUG) {
                                printf("Key Pressed - %s\n", SDL_GetKeyName(event.key.keysym.sym));
                                printf("Delete selected shape\n\n");
                            }
                            deleteSelectedShape();  // Utiliser la fonction existante
                            break;
                    }
                    break;

                case SDL_MOUSEWHEEL:
                    if (DEBUG) {
                        printf("Mouse Wheel Scrolled\n");
                        printf("Zoom in on selected shapes\n\n");
                    }
                    for (int i = 0; i < shapeCount; i++) {
                        if (shapes[i].selected) {
                            zoomShape(&shapes[i], event.wheel.y > 0 ? 1.0 : -1.0);
                        }
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (gameState.isPlaying) {
                        // Vérifier si on a cliqué sur une forme
                        for (int i = 0; i < shapeCount; i++) {
                            if (isPointInShape(&shapes[i], event.button.x, event.button.y)) {
                                gameState.score += 100;
                                deleteShape(i);
                                break;
                            }
                        }
                    } else {
                        // Code existant pour la sélection des formes
                        if (event.button.button == SDL_BUTTON_LEFT) {
                            handleCursorSelection(event.button.x, event.button.y);
                        }
                    }
                    break;

                case SDL_MOUSEMOTION:
                    if (DEBUG) {
                        printf("Mouse Motion\n");
                        printf("Move cursor to (%d, %d)\n\n", event.motion.x, event.motion.y);
                    }
                    // Update cursor position and move selected shapes with the mouse.
                    cursor.x = event.motion.x;
                    cursor.y = event.motion.y;
                    moveShapesWithMouse(shapes, shapeCount, &event, &cursor);
                    break;
                break;
            }   
        }

        // Clear the screen and set a background color.
        SDL_SetRenderDrawColor(renderer, bgcolorR, bgcolorG, bgcolorB, 255);
        SDL_RenderClear(renderer);

        // Update animations
        for (int i = 0; i < shapeCount; i++) {
            if (shapes[i].isAnimating) {
                for (int j = 0; j < shapes[i].num_animations; j++) {
                    if (shapes[i].animations[j] == ANIM_ROTATE) {
                        animation_rotate(&shapes[i], shapes[i].animations[j]);
                    }
                    if (shapes[i].animations[j] == ANIM_ZOOM) {
                        animation_zoom(&shapes[i], shapes[i].animations[j]);
                    }
                    if (shapes[i].animations[j] == ANIM_COLOR) {
                        animation_color(&shapes[i], shapes[i].animations[j]);
                    }
                    if (shapes[i].animations[j] == ANIM_BOUNCE) {
                        int width, height;
                        SDL_GetWindowSize(window, &width, &height);
                        animation_bounce(&shapes[i], shapes[i].animations[j], width, height);
                    }
                }
            }
        }

        // Render all shapes in z-order
        renderAllShapes(renderer);

        // Render the custom cursor
        renderCursor(renderer, &cursor);

        if (gameState.isGameMode) {
            // Mode jeu (actif ou en attente)
            renderGameUI(renderer, font, &gameState);
        } else {
            // Mode normal
            renderCursorCoordinates(renderer, font, cursor.x, cursor.y);
            for (int i = 0; i < shapeCount; i++) {
                if (shapes[i].selected) {
                    renderShapeInfo(renderer, font, &shapes[i]);
                    break;
                }
            }
            renderLastKeyPressed(renderer, font);
        }

        // Present the updated frame
        SDL_RenderPresent(renderer);

        // Update game state
        if (gameState.isPlaying) {
            updateGame(&gameState, deltaTime, cursor.x, cursor.y);
        }
    }
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_ShowCursor(SDL_ENABLE); // Restore the default system cursor.
}

/**
 * @brief Handles SDL events and checks for quit signals.
 * 
 * This function processes SDL events in a loop. If an SDL_QUIT event 
 * is detected (e.g., when the user closes the window), it cleans up 
 * the SDL renderer and texture and exits the application.
 * 
 * @param renderer SDL renderer to destroy if a quit event is detected.
 * @param texture SDL texture to destroy if a quit event is detected.
 * 
 * @return -1 if a quit event occurs, 0 otherwise.
 */
int handleEvents(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_Event event;

    // Process all pending events.
    while (SDL_PollEvent(&event)) {
        // Check if the event is a quit signal.
        if (event.type == SDL_QUIT) {
            // Destroy the renderer and texture to free resources.
            SDL_DestroyRenderer(renderer);
            SDL_DestroyTexture(texture);

            // Quit SDL to clean up the environment.
            SDL_Quit();

            // Return -1 to indicate a quit event was handled.
            printf("%sExecutionError: Application terminated by user. Cleaning up resources...%s\n", 
                   RED_COLOR);
            return -1;
        }
    }

    // Return 0 if no quit event was detected.
    return 0;
}

/**
 * @brief Initializes a new cursor with specified properties.
 * 
 * This function creates and configures a `Cursor` instance using the 
 * provided parameters for its position, color, thickness, and visibility.
 * 
 * @param x Initial x-coordinate of the cursor's position.
 * @param y Initial y-coordinate of the cursor's position.
 * @param color SDL_Color defining the cursor's color (RGBA).
 * @param thickness Integer specifying the thickness of the cursor outline.
 * @param visible Boolean indicating whether the cursor is visible.
 * 
 * @return Cursor A new cursor instance with the given properties.
 */
Cursor createCursor(int x, int y, SDL_Color color, int thickness, bool visible) {
    // Initialize a cursor instance with provided attributes.
    Cursor cursor;
    cursor.x = x;                   // Set the x-coordinate.
    cursor.y = y;                   // Set the y-coordinate.
    cursor.color = color;           // Set the color using SDL_Color.
    cursor.thickness = thickness;   // Set the thickness of the cursor outline.
    cursor.visible = visible;       // Set the visibility status of the cursor.
    return cursor;                  // Return the configured cursor.
}

/**
 * @brief Renders the cursor on the screen.
 * 
 * This function draws the cursor on the SDL renderer as an arrow 
 * with a triangular tip and a small square at the base. The cursor 
 * is rendered only if it is marked as visible.
 * 
 * @param renderer SDL renderer instance used for drawing.
 * @param cursor Pointer to the `Cursor` instance to render.
 */
void renderCursor(SDL_Renderer *renderer, const Cursor *cursor) {
    // Check if the cursor is visible; skip rendering if not.
    if (!cursor->visible) return;

    // Set the drawing color based on the cursor's RGBA color.
    SDL_SetRenderDrawColor(renderer, cursor->color.r, cursor->color.g, cursor->color.b, cursor->color.a);

    // Define the triangular tip of the cursor (an arrowhead shape).
    SDL_Point triangle[4] = {
        {cursor->x, cursor->y},                                // Tip of the arrow.
        {cursor->x - cursor->thickness, cursor->y + 2 * cursor->thickness}, // Bottom-left corner.
        {cursor->x + cursor->thickness, cursor->y + 2 * cursor->thickness}, // Bottom-right corner.
        {cursor->x, cursor->y}                                 // Close the triangle.
    };

    // Draw the triangular arrowhead using lines.
    SDL_RenderDrawLines(renderer, triangle, 4);

    // Define a small rectangular "tail" at the base of the cursor.
    SDL_Rect tail = {
        cursor->x - cursor->thickness / 2,    // Center horizontally around `cursor->x`.
        cursor->y + 3 * cursor->thickness,    // Place below the arrowhead.
        cursor->thickness,                    // Width of the rectangle.
        cursor->thickness                     // Height of the rectangle.
    };

    // Fill the rectangle to represent the cursor's tail.
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
 * @return The index of the shape under the cursor, or -1 if no shape is found.
 */
int findShapeAtCursor(int x, int y) {
    // Iterate through all the shapes in the global shapes array.
    for (int i = 0; i < shapeCount; i++) {
        Shape *shape = &shapes[i]; // Get a reference to the current shape.

        // Validate the shape's type form (it must be "filled" or "empty").
        if ((strcmp(shape->typeForm, "filled") != 0) && (strcmp(shape->typeForm, "empty") != 0)) {
            continue; // Skip invalid shapes.
        }

        // Determine the type of the shape and check if the cursor is within it.
        switch (shape->type) 
        {
            // Check if the cursor is inside the circle.
            case SHAPE_CIRCLE:
                if (isPointInCircle(x, y, shape->data.circle.x, shape->data.circle.y, shape->data.circle.radius)) 
                return i; // Return the index of the shape.
                break;
            case SHAPE_RECTANGLE:
                if (isPointInRectangle(x, y, shape->data.rectangle.x, shape->data.rectangle.y, shape->data.rectangle.width, shape->data.rectangle.height)) return i;
                break;
            case SHAPE_SQUARE:
                if (isPointInSquare(x, y, shape->data.square.x, shape->data.square.y, shape->data.square.c)) return i;
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
            case SHAPE_TRIANGLE:
                if(isPointInTriangle(x, y, shape->data.triangle.cx, shape->data.triangle.cy, shape->data.triangle.radius)) return i;
                break;
            case SHAPE_LINE:
                if(isPointInLine(x, y, shape->data.line.x1, shape->data.line.y1, shape->data.line.x2, shape->data.line.y2, shape->data.line.thickness)) return i;        
                break;
        }
    }
    return -1; // No shape found at the cursor's position.
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
 */
void handleCursorSelection(int cursorX, int cursorY) {
    int index = findShapeAtCursor(cursorX, cursorY);
    if (index != -1) {
        // Deselect all other shapes first.
        for (int i = 0; i < shapeCount; i++) {
            if (i != index) {
                shapes[i].selected = false; // Ensure only one shape is selected.
            }
        }
        // Toggle selection for the clicked shape.
        shapes[index].selected = !shapes[index].selected;
        shapes[index].isAnimating = false;
    } else {
        // Deselect all shapes if no shape is clicked.
        for (int i = 0; i < shapeCount; i++) {
            shapes[i].selected = false;
        }
    }
}

/**
 * @brief Displays the current cursor coordinates in the top-left corner of the window.
 * 
 * @param renderer The SDL renderer used for drawing
 * @param font The font used for text rendering
 * @param x Current X coordinate of the cursor
 * @param y Current Y coordinate of the cursor
 */
void renderCursorCoordinates(SDL_Renderer *renderer, TTF_Font *font, int x, int y) {
    if (!font) return;

    // Create text string with cursor coordinates
    char text[32];
    snprintf(text, sizeof(text), "x: %d, y: %d", x, y);

    // Create text surface with black color
    SDL_Color textColor = {0, 0, 0, 255}; // Black color (R=0, G=0, B=0, A=255)
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, textColor);
    if (!surface) return;

    // Convert surface to texture for rendering
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return;

    // Position the text in the top-left corner with margins
    SDL_Rect textRect;
    SDL_QueryTexture(texture, NULL, NULL, &textRect.w, &textRect.h);
    textRect.x = 10;  // 10 pixels margin from left
    textRect.y = 10;  // 10 pixels margin from top

    // Render the text and cleanup
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_DestroyTexture(texture);
}

char* getAnimationName(AnimationType animation){
    switch(animation){
        case ANIM_ROTATE:
            return "Rotate";
        case ANIM_ZOOM:
            return "Zoom";
        case ANIM_COLOR:
            return "Color";
        case ANIM_BOUNCE:
            return "Bounce";
        default:
            return "None";
    }
}

/**
 * @brief Displays detailed information about the selected shape in the bottom-right corner.
 * Updates in real-time when shape properties change (rotation, position, size).
 * 
 * @param renderer The SDL renderer used for drawing
 * @param font The font used for text rendering
 * @param shape Pointer to the currently selected shape
 */
void renderShapeInfo(SDL_Renderer *renderer, TTF_Font *font, Shape *shape) {
    if (!font || !shape->selected) return;

    // Create text string with shape information
    char text[128];
    char text2[128];
    // Determine if shape is empty or filled
    const char *formType = (strcmp(shape->typeForm, "empty") == 0) ? "(empty)" : "(filled)";
    
    char animation_chose[20];
    strcpy(animation_chose, getAnimationName(shape->animation_parser));
    
    // Format text based on shape type
    switch (shape->type) {
        case SHAPE_CIRCLE: 
            snprintf(text, sizeof(text), "Form: Circle %s\nRotation: %.1f deg\nRadius: %d\nPosition: (%d,%d) \nAnimation Picking: %s", 
                    formType, shape->rotation, 
                    shape->data.circle.radius,
                    shape->data.circle.x, shape->data.circle.y,
                    animation_chose);
            snprintf(text2, sizeof(text2), "Animation List: %s, %s, %s", getAnimationName(shape->animations[0]), getAnimationName(shape->animations[1]), getAnimationName(shape->animations[2]));
            break;
        case SHAPE_RECTANGLE: 
            snprintf(text, sizeof(text), "Form: Rectangle %s\nRotation: %.1f deg\nSize: %dx%d\nPosition: (%d,%d) \nAnimation Picking: %s", 
                    formType, shape->rotation, 
                    shape->data.rectangle.width, shape->data.rectangle.height,
                    shape->data.rectangle.x, shape->data.rectangle.y,
                    animation_chose);
            snprintf(text2, sizeof(text2), "Animation List: %s, %s, %s", getAnimationName(shape->animations[0]), getAnimationName(shape->animations[1]), getAnimationName(shape->animations[2]));
            break;
        case SHAPE_SQUARE:
            snprintf(text, sizeof(text), "Form: Square %s\nRotation: %.1f deg\nSize: %dx%d\nPosition: (%d,%d) \nAnimation Picking: %s", 
                    formType, shape->rotation, 
                    shape->data.square.c, shape->data.square.c,
                    shape->data.square.x, shape->data.square.y,
                    animation_chose);
            snprintf(text2, sizeof(text2), "Animation List: %s, %s, %s", getAnimationName(shape->animations[0]), getAnimationName(shape->animations[1]), getAnimationName(shape->animations[2]));
            break;
        case SHAPE_ELLIPSE: 
            snprintf(text, sizeof(text), "Form: Ellipse %s\nRotation: %.1f deg\nRadius: %dx%d\nPosition: (%d,%d) \nAnimation Picking: %s", 
                    formType, shape->rotation, 
                    shape->data.ellipse.rx, shape->data.ellipse.ry,
                    shape->data.ellipse.x, shape->data.ellipse.y,
                    animation_chose);
            snprintf(text2, sizeof(text2), "Animation List: %s, %s, %s", getAnimationName(shape->animations[0]), getAnimationName(shape->animations[1]), getAnimationName(shape->animations[2]));
            break;
        case SHAPE_LINE: 
            snprintf(text, sizeof(text), "Form: Line \nRotation: %.1f deg\nLength: %d\nThickness: %d\nStart: (%d,%d)\nEnd: (%d,%d) \nAnimation Picking: %s", 
                    shape->rotation,
                    (int)sqrt(pow(shape->data.line.x2 - shape->data.line.x1, 2) + 
                             pow(shape->data.line.y2 - shape->data.line.y1, 2)),
                    shape->data.line.thickness,
                    shape->data.line.x1, shape->data.line.y1,
                    shape->data.line.x2, shape->data.line.y2,
                    animation_chose);
            snprintf(text2, sizeof(text2), "Animation List: %s, %s, %s", getAnimationName(shape->animations[0]), getAnimationName(shape->animations[1]), getAnimationName(shape->animations[2]));
            break;  
        case SHAPE_ROUNDED_RECTANGLE: 
            snprintf(text, sizeof(text), "Form: Rounded Rect %s\nRotation: %.1f deg\nSize: %dx%d\nRadius: %d\nPosition: (%d,%d) \nAnimation Picking: %s", 
                    formType, shape->rotation, 
                    shape->data.rounded_rectangle.x2 - shape->data.rounded_rectangle.x1,
                    shape->data.rounded_rectangle.y2 - shape->data.rounded_rectangle.y1,
                    shape->data.rounded_rectangle.radius,
                    shape->data.rounded_rectangle.x1, shape->data.rounded_rectangle.y1,
                    animation_chose);
            snprintf(text2, sizeof(text2), "Animation List: %s, %s, %s", getAnimationName(shape->animations[0]), getAnimationName(shape->animations[1]), getAnimationName(shape->animations[2]));
            break;
        case SHAPE_POLYGON: 
            snprintf(text, sizeof(text), "Form: Polygon %s\nRotation: %.1f deg\nRadius: %d\nSides: %d\nPosition: (%d,%d) \nAnimation Picking: %s", 
                    formType, shape->rotation, 
                    shape->data.polygon.radius,
                    shape->data.polygon.sides,
                    shape->data.polygon.cx, shape->data.polygon.cy,
                    animation_chose);
            snprintf(text2, sizeof(text2), "Animation List: %s, %s, %s", getAnimationName(shape->animations[0]), getAnimationName(shape->animations[1]), getAnimationName(shape->animations[2]));
            break;
        case SHAPE_TRIANGLE:
            snprintf(text, sizeof(text), "Form: Triangle %s\nRotation: %.1f deg\nRadius: %d\nPosition: (%d,%d) \nAnimation Picking: %s", 
                    formType, shape->rotation, 
                    shape->data.triangle.radius,
                    shape->data.triangle.cx, shape->data.triangle.cy,
                    animation_chose);
            snprintf(text2, sizeof(text2), "Animation List: %s, %s, %s", getAnimationName(shape->animations[0]), getAnimationName(shape->animations[1]), getAnimationName(shape->animations[2]));
            break;
        case SHAPE_ARC: 
            snprintf(text, sizeof(text), "Form: Arc %s\nRotation: %.1f deg\nRadius: %d\nAngles: %d deg to %d deg\nPosition: (%d,%d) \nAnimation Picking: %s", 
                    formType, shape->rotation, 
                    shape->data.arc.radius,
                    shape->data.arc.start_angle,
                    shape->data.arc.end_angle,
                    shape->data.arc.x, shape->data.arc.y,
                    animation_chose);
            snprintf(text2, sizeof(text2), "Animation List: %s, %s, %s", getAnimationName(shape->animations[0]), getAnimationName(shape->animations[1]), getAnimationName(shape->animations[2]));
            break;
        default: 
            snprintf(text, sizeof(text), "Form: Unknown %s\nRotation: %.1f deg \nAnimation Picking: %s", 
                    formType, shape->rotation,
                    animation_chose);
            snprintf(text2, sizeof(text2), "Animation List: %s, %s, %s", getAnimationName(shape->animations[0]), getAnimationName(shape->animations[1]), getAnimationName(shape->animations[2]));
            break;
    }

    // Create text surface with enhanced quality rendering
    SDL_Color textColor = {0, 0, 0, 255}; // Black color
    // Use blended rendering for smoother text with word wrap at 300 pixels
    SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font, text, textColor, 300);
    SDL_Surface *surface2 = TTF_RenderText_Blended_Wrapped(font, text2, textColor, 400);
    if (!surface) return;
    if (!surface2) return;
    // Convert surface to texture for rendering
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Texture *texture2 = SDL_CreateTextureFromSurface(renderer, surface2);
    SDL_FreeSurface(surface);
    SDL_FreeSurface(surface2);
    if (!texture) return;
    if (!texture2) return;

    // Get window dimensions for positioning
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    // Position the text in the bottom-right corner with margins
    SDL_Rect textRect;
    SDL_Rect textRect2;
    SDL_QueryTexture(texture, NULL, NULL, &textRect.w, &textRect.h);
    SDL_QueryTexture(texture2, NULL, NULL, &textRect2.w, &textRect2.h);
    textRect.x = windowWidth - textRect.w + 70;  // Adjust position from right edge
    textRect.y = windowHeight - textRect.h - 10; // 10 pixels margin from bottom
    textRect2.x = 10;  // Adjust position from left edge
    textRect2.y = windowHeight - textRect2.h - 10; // 10 pixels margin from bottom

    // Render the text and cleanup
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_RenderCopy(renderer, texture2, NULL, &textRect2);
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(texture2);
}

/**
 * @brief Renders the last pressed key in the top-right corner of the window
 * 
 * @param renderer The SDL renderer used for drawing
 * @param font The font used for text rendering
 */
void renderLastKeyPressed(SDL_Renderer *renderer, TTF_Font *font) {
    // Return if font is not loaded or no key has been pressed yet
    if (!font || lastKeyPressed[0] == '\0') return;

    // Create the display text with the last key pressed
    char keyText[64];
    snprintf(keyText, sizeof(keyText), "Key Pressed: %s", lastKeyPressed);
    
    // Set text color to black
    SDL_Color textColor = {0, 0, 0, 255}; // Black color (R=0, G=0, B=0, A=255)
    SDL_Surface* keySurface = TTF_RenderText_Solid(font, keyText, textColor);
    if (!keySurface) return;

    // Convert surface to texture for rendering
    SDL_Texture* keyTexture = SDL_CreateTextureFromSurface(renderer, keySurface);
    SDL_FreeSurface(keySurface);
    if (!keyTexture) return;

    // Position the text in the top-right corner
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);
    SDL_Rect keyRect;
    SDL_QueryTexture(keyTexture, NULL, NULL, &keyRect.w, &keyRect.h);
    keyRect.x = windowWidth - keyRect.w - 10;  // 10 pixels margin from right
    keyRect.y = 10;                            // 10 pixels margin from top

    // Render the text and cleanup
    SDL_RenderCopy(renderer, keyTexture, NULL, &keyRect);
    SDL_DestroyTexture(keyTexture);
}

/**
 * @brief Deletes the shape at the cursor's position.
 * 
 * This function removes the shape under the cursor from the global shapes array.
 * If no shape is found under the cursor, no action is taken.
 */
void handleShapeDeletion(int cursorX, int cursorY) {
    // Find the index of the shape under the cursor.
    int index = findShapeAtCursor(cursorX, cursorY);
    
    // If a shape is found, delete it.
    if (index != -1) {
        deleteShape(index); // Call the deleteShape function to remove the shape.
    }
}
