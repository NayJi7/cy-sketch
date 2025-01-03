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
            }
        }
        // Toggle selection for the clicked shape.
        shapes[index].selected = !shapes[index].selected;
    } else {
        // Deselect all shapes if no shape is clicked.
        for (int i = 0; i < shapeCount; i++) {
            shapes[i].selected = false;
        }
    }
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
