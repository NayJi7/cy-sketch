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
 * @brief Main function that initializes SDL, creates a window and renderer, and draws shapes.
 * 
 * @return int Returns 0 on successful execution.
 */
int main(){

    SDL_Window *window = NULL; // Declare a pointer for the SDL window.
    SDL_Renderer *renderer = NULL; // Declare a pointer for the SDL renderer.
    SDL_Event event; // Declare an SDL_Event structure to handle user inputs (e.g., keyboard, mouse).

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) { 
        // Initialize SDL with video and events subsystems. Returns 0 if successful.
        
        printf("SDL Error: %s\n", SDL_GetError()); 
        // Print an error message if initialization fails.
        
        return -1; // Exit the program with an error code.
    }

    // Create the window and renderer
    if (SDL_CreateWindowAndRenderer(800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer) != 0) {
        // Create a resizable 800x600 window and an associated renderer.
       
        printf("SDL_CreateWindowAndRenderer Error: %s\n", SDL_GetError()); 
        // Print an error message if the window and renderer creation fails.
        
        SDL_Quit(); // Clean up SDL subsystems before exiting.
        return -1; // Exit the program with an error code.
    }

    SDL_SetWindowTitle(window, "Animated Shapes"); 
    // Set the title of the SDL window to "Animated Shapes".

    // Background color for the cursor
    SDL_Color cursorColor = {0, 0, 0, 0}; 
    // Define the cursor's color as black with full transparency (alpha = 0).
    
    Cursor cursor = createCursor(400, 300, cursorColor, 5, true); 
    // Create a custom cursor positioned at (400, 300), with a thickness of 5, and make it visible.

    // Create a main texture for drawing
    SDL_Texture* mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 800, 600);
    // Create a texture of 800x600 pixels using the RGBA8888 pixel format, and allow rendering to this texture.

    // Set the main texture as the render target and fill it with a light gray background
    SDL_SetRenderTarget(renderer, mainTexture); 
    // Set the main texture as the current render target.
    
    SDL_SetRenderDrawColor(renderer, 229, 229, 229, 255); 
    // Set the draw color to light gray (R:229, G:229, B:229, A:255).
    
    SDL_RenderClear(renderer); 
    // Clear the texture with the current draw color (fills it with light gray).


    //circles ------
    //if(drawShape(renderer, mainTexture, "circle", "animated", "filled", yellow, 200, 170, 60) == -1) return 0;
    //if(drawShape(renderer, mainTexture, "circle", "instant", "empty", yellow, 400, 170, 60) == -1) return 0;

    //ellipses ------
    //if(drawShape(renderer, mainTexture, "ellipse", "animated", "filled", red, 400, 270, 70, 50) == -1) return 0;
    //if(drawShape(renderer, mainTexture, "ellipse", "instant", "empty", gray, 300, 370, 70, 50) == -1) return 0;

    //lines ------
    //if(drawShape(renderer, mainTexture, "line", "animated", "filled", magenta, 200, 200, 300, 200, 2) == -1) return 0;
    //if(drawShape(renderer, mainTexture, "line", "instant", "filled", magenta, 100, 100, 400, 300, 20) == -1) return 0;

    //polygones -----
    //if(drawShape(renderer, mainTexture, "polygon", "animated", "filled", dark_gray, 530, 130, 100, 3) == -1) return 0;
    //if(drawShape(renderer, mainTexture, "polygon", "instant", "empty", gray, 330, 130, 100, 12) == -1) return 0;
    
    //rectangles ------
    //if(drawShape(renderer, mainTexture, "rectangle", "animated", "filled", red, 100, 100, 200, 50) == -1) return 0;
    //if(drawShape(renderer, mainTexture, "rectangle", "instant", "empty", blue, 200, 200, 200, 50) == -1) return 0;


//A FAIRE AU PLUS VITE __________________________________________________________
    //changer la logique de l'arc pour s'inspirer de l'ellipse rx, ry (à voir à la fin)
    
    //COMMENTER LE CODE ET REVOIR DERRIERE MOI car j'ai pas assez commenté les calcules dans form.c pour les animated mais je me suis pas repeté pour certain commentaire si tu remarques
    // (j'ai commenté le main.c, formEvents.c, form.c, form.h-structure, cursorEvent.h-structure)

    //REGLER LES COULEURS POUR LES ANIMATED FORMES

    //regler les deselections avec souris avec l'arc
        //arcs ------
        //if(drawShape(renderer, mainTexture, "arc", "animated", "filled", red, 200, 400, 100, 90, 270) == -1) return 0;
        //if(drawShape(renderer, mainTexture, "arc", "instant", "empty", red, 400, 200, 100, 90, 180) == -1) return 0;

    //REVOIR CETTE FONCTION (rotation, dezoom au max)
        //roundedrectangles ------
        if(drawShape(renderer, mainTexture, "roundedRectangle", "instant", "empty", red, 100, 200, 200, 150, 10) == -1) return 0;
        if(drawShape(renderer, mainTexture, "roundedRectangle", "instant", "filled", red, 300, 100, 200, 150, 10) == -1) return 0;
//____________________________________________________________________     

    SDL_SetRenderTarget(renderer, NULL); 
    // Reset the render target to the default (the window).
    
    SDL_RenderCopy(renderer, mainTexture, NULL, NULL); 
    // Copy the content of the main texture to the renderer.
    
    SDL_RenderPresent(renderer); 
    // Display the current content of the renderer on the window.

    mainLoop(renderer, event, cursor); 
    // Start the main event loop, passing the renderer, event handler, and cursor.

    // Clean up resources before exiting
    SDL_DestroyTexture(mainTexture); // Destroy the main texture.
    SDL_DestroyRenderer(renderer); // Destroy the renderer.
    SDL_DestroyWindow(window); // Destroy the SDL window.
    SDL_Quit(); // Quit SDL and clean up any initialized subsystems.

    return 0;

}
