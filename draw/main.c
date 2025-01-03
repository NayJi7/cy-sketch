#include "../files.h/main.h"

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
    if(drawShape(renderer, mainTexture, "polygon", "animated", "filled", dark_gray, 530, 130, 100, 4) == -1) return 0;
    if(drawShape(renderer, mainTexture, "polygon", "instant", "empty", gray, 330, 130, 100, 4) == -1) return 0;
    
    //rectangles ------
    //if(drawShape(renderer, mainTexture, "rectangle", "animated", "filled", red, 100, 100, 200, 50) == -1) return 0;
    //if(drawShape(renderer, mainTexture, "rectangle", "instant", "empty", blue, 200, 200, 200, 50) == -1) return 0;

    //arcs ------
    //if(drawShape(renderer, mainTexture, "arc", "animated", "filled", red, 200, 400, 100, 90, 270) == -1) return 0;
    //if(drawShape(renderer, mainTexture, "arc", "instant", "empty", red, 400, 200, 100, 90, 180) == -1) return 0;


//A FAIRE AU PLUS VITE __________________________________________________________
    //changer la logique de l'arc pour s'inspirer de l'ellipse rx, ry (à voir à la fin)
    //REGLER LES COULEURS POUR LES ANIMATED FORMES


    
    //REVOIR DERRIERE MOI LES COMMENTAIRES car j'ai pas assez commenté les calcules dans form.c pour les animated mais je me suis pas repeté pour certain commentaire si tu remarques
    // (j'ai commenté toutes les fonctions)
    
    //REVOIR CETTE FONCTION (rotation, dezoom au max presque propre pour empty, zoom/dezoom pour filled, la version animated)
        //roundedrectangles ------
        //if(drawShape(renderer, mainTexture, "roundedRectangle", "animated", "empty", red, 100, 200, 200, 200, 10) == -1) return 0;
        //if(drawShape(renderer, mainTexture, "roundedRectangle", "instant", "filled", red, 300, 100, 200, 200, 10) == -1) return 0;
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
