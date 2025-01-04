#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include "../files.h/main.h"
#include "../files.h/formEvents.h"
#include "../files.h/cursorEvents.h"

// ANSI escape codes for colors
#define RED_COLOR "\033[1;31m"
#define RESET_COLOR "\033[0m"

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
    SDL_ShowCursor(SDL_DISABLE);
    /// Ensuite initialiser SDL_ttf
    if (TTF_Init() < 0) {
        printf("%sExecutionError: Failed to initialize TTF: %s%s\n", RED_COLOR, TTF_GetError(), RESET_COLOR);
        return;
    }

    // Charger la police
    TTF_Font *font = TTF_OpenFont("./fonts/DejaVuMathTeXGyre.ttf", 16);
    if (!font) {
        printf("%sExecutionError: Failed to open font: %s%s\n", RED_COLOR, TTF_GetError(), RESET_COLOR);
        TTF_Quit();
        return;
    }

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
                            exit(0);
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

                        case SDLK_z:
                            // Move selected shape up in z-order
                            moveShapeUp();
                            break;

                        case SDLK_s:
                            // Move selected shape down in z-order
                            moveShapeDown();
                            break;

                        case SDLK_a:
                            // Toggle animation for selected shape
                            toggleAnimation();
                            break;

                        case SDLK_RETURN:
                        case SDLK_e: {
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
                            break;
                        }

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
                    renderCursorCoordinates(renderer, font, cursor.x, cursor.y);
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

        // Update animations
        updateAnimations(renderer);

        // Render all shapes in z-order
        renderAllShapes(renderer);

        // Render the custom cursor.
        renderCursor(renderer, &cursor);
        renderCursorCoordinates(renderer, font, cursor.x, cursor.y);
        
        // Afficher les informations de rotation pour la forme sélectionnée
        for (int i = 0; i < shapeCount; i++) {
            if (shapes[i].selected) {
                renderShapeInfo(renderer, font, &shapes[i]);
                break; // On ne montre que pour la première forme sélectionnée
            }
        }

        // Present the updated frame.
        SDL_RenderPresent(renderer);
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
                   RED_COLOR, RESET_COLOR);
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
                shapes[i].isAnimating = false; // Stop animation when deselected
            }
        }
        // Toggle selection for the clicked shape.
        shapes[index].selected = !shapes[index].selected;
        // If deselected, stop animation
        if (!shapes[index].selected) {
            shapes[index].isAnimating = false;
        }
    } else {
        // Deselect all shapes if no shape is clicked.
        for (int i = 0; i < shapeCount; i++) {
            shapes[i].selected = false;
            shapes[i].isAnimating = false; // Stop animation when deselected
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
    // Determine if shape is empty or filled
    const char *formType = (strcmp(shape->typeForm, "empty") == 0) ? "(empty)" : "(filled)";
    
    char animation[20];
    switch(shape->animation) {
        case ANIM_ROTATE:
            strcpy(animation, "Rotate");
            break;
        case ANIM_ZOOM:
            strcpy(animation, "Zoom"); 
            break;
        default:
            strcpy(animation, "None");
            break;
    }
    // Format text based on shape type
    switch (shape->type) {
        case SHAPE_CIRCLE: 
            snprintf(text, sizeof(text), "Form: Circle %s\nRotation: %.1f deg\nRadius: %d\nPosition: (%d,%d) \nAnimation: %s", 
                    formType, shape->rotation, 
                    shape->data.circle.radius,
                    shape->data.circle.x, shape->data.circle.y,
                    animation);
            break;
        case SHAPE_RECTANGLE: 
            snprintf(text, sizeof(text), "Form: Rectangle %s\nRotation: %.1f deg\nSize: %dx%d\nPosition: (%d,%d) \nAnimation: %s", 
                    formType, shape->rotation, 
                    shape->data.rectangle.width, shape->data.rectangle.height,
                    shape->data.rectangle.x, shape->data.rectangle.y,
                    animation);
            break;
        case SHAPE_ELLIPSE: 
            snprintf(text, sizeof(text), "Form: Ellipse %s\nRotation: %.1f deg\nRadius: %dx%d\nPosition: (%d,%d) \nAnimation: %s", 
                    formType, shape->rotation, 
                    shape->data.ellipse.rx, shape->data.ellipse.ry,
                    shape->data.ellipse.x, shape->data.ellipse.y,
                    animation);
            break;
        case SHAPE_LINE: 
            snprintf(text, sizeof(text), "Form: Line \nRotation: %.1f deg\nLength: %d\nThickness: %d\nStart: (%d,%d)\nEnd: (%d,%d) \nAnimation: %s", 
                    shape->rotation,
                    (int)sqrt(pow(shape->data.line.x2 - shape->data.line.x1, 2) + 
                             pow(shape->data.line.y2 - shape->data.line.y1, 2)),
                    shape->data.line.thickness,
                    shape->data.line.x1, shape->data.line.y1,
                    shape->data.line.x2, shape->data.line.y2,
                    animation);
            break;
        case SHAPE_ROUNDED_RECTANGLE: 
            snprintf(text, sizeof(text), "Form: Rounded Rect %s\nRotation: %.1f deg\nSize: %dx%d\nRadius: %d\nPosition: (%d,%d) \nAnimation: %s", 
                    formType, shape->rotation, 
                    shape->data.rounded_rectangle.x2 - shape->data.rounded_rectangle.x1,
                    shape->data.rounded_rectangle.y2 - shape->data.rounded_rectangle.y1,
                    shape->data.rounded_rectangle.radius,
                    shape->data.rounded_rectangle.x1, shape->data.rounded_rectangle.y1,
                    animation);
            break;
        case SHAPE_POLYGON: 
            snprintf(text, sizeof(text), "Form: Polygon %s\nRotation: %.1f deg\nRadius: %d\nSides: %d\nPosition: (%d,%d) \nAnimation: %s", 
                    formType, shape->rotation, 
                    shape->data.polygon.radius,
                    shape->data.polygon.sides,
                    shape->data.polygon.cx, shape->data.polygon.cy,
                    animation);
            break;
        case SHAPE_ARC: 
            snprintf(text, sizeof(text), "Form: Arc %s\nRotation: %.1f deg\nRadius: %d\nAngles: %d deg to %d deg\nPosition: (%d,%d) \nAnimation: %s", 
                    formType, shape->rotation, 
                    shape->data.arc.radius,
                    shape->data.arc.start_angle,
                    shape->data.arc.end_angle,
                    shape->data.arc.x, shape->data.arc.y,
                    animation);
            break;
        default: 
            snprintf(text, sizeof(text), "Form: Unknown %s\nRotation: %.1f deg \nAnimation: %s", 
                    formType, shape->rotation,
                    animation);
            break;
    }

    // Create text surface with enhanced quality rendering
    SDL_Color textColor = {0, 0, 0, 255}; // Black color
    // Use blended rendering for smoother text with word wrap at 300 pixels
    SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font, text, textColor, 300);
    if (!surface) return;

    // Convert surface to texture for rendering
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return;

    // Get window dimensions for positioning
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    // Position the text in the bottom-right corner with margins
    SDL_Rect textRect;
    SDL_QueryTexture(texture, NULL, NULL, &textRect.w, &textRect.h);
    textRect.x = windowWidth - textRect.w + 80;  // Adjust position from right edge
    textRect.y = windowHeight - textRect.h - 10; // 10 pixels margin from bottom

    // Render the text and cleanup
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_DestroyTexture(texture);
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
