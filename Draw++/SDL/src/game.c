#include "../files.h/game.h"
#include <math.h>

void initGame(GameState* game) {
    game->score = 0;
    game->timeLeft = 30.0f;  // 30 secondes de jeu
    game->isPlaying = false;
    game->isGameMode = false;  // Démarrer en mode normal
    game->won = false;
    
    // Sauvegarder les formes existantes
    game->savedShapeCount = shapeCount;
    for (int i = 0; i < shapeCount; i++) {
        game->savedShapes[i] = shapes[i];
    }
    
    game->hasShapes = (shapeCount > 0);
}

void updateGame(GameState* game, float deltaTime, int cursorX, int cursorY) {
    if (!game->isPlaying) return;
    
    game->timeLeft -= deltaTime;

    // Obtenir les dimensions de la fenêtre
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(SDL_GetRenderer(SDL_GetWindowFromID(1)), &windowWidth, &windowHeight);
    
    // Vérifier les collisions et faire fuir les formes du curseur
    for (int i = shapeCount - 1; i >= 0; i--) {
        // Calculer la direction de fuite
        int shapeX = 0, shapeY = 0;
        int shapeSize = 50; // Taille par défaut pour la limite
        bool isCaught = false;
        
        // Obtenir le centre et la taille de la forme
        switch (shapes[i].type) {
            case SHAPE_CIRCLE:
                shapeX = shapes[i].data.circle.x;
                shapeY = shapes[i].data.circle.y;
                shapeSize = shapes[i].data.circle.radius * 2;
                // Check if cursor is inside circle
                {
                    float dx = cursorX - shapeX;
                    float dy = cursorY - shapeY;
                    float distSq = dx*dx + dy*dy;
                    isCaught = distSq <= shapes[i].data.circle.radius * shapes[i].data.circle.radius;
                }
                break;
            case SHAPE_RECTANGLE:
                shapeX = shapes[i].data.rectangle.x + shapes[i].data.rectangle.width/2;
                shapeY = shapes[i].data.rectangle.y + shapes[i].data.rectangle.height/2;
                // Check if cursor is inside rectangle
                isCaught = cursorX >= shapes[i].data.rectangle.x &&
                          cursorX <= shapes[i].data.rectangle.x + shapes[i].data.rectangle.width &&
                          cursorY >= shapes[i].data.rectangle.y &&
                          cursorY <= shapes[i].data.rectangle.y + shapes[i].data.rectangle.height;
                break;
            case SHAPE_SQUARE:
                shapeX = shapes[i].data.square.x + shapes[i].data.square.c/2;
                shapeY = shapes[i].data.square.y + shapes[i].data.square.c/2;
                // Check if cursor is inside square
                isCaught = cursorX >= shapes[i].data.square.x &&
                          cursorX <= shapes[i].data.square.x + shapes[i].data.square.c &&
                          cursorY >= shapes[i].data.square.y &&
                          cursorY <= shapes[i].data.square.y + shapes[i].data.square.c;
                break;
            case SHAPE_ELLIPSE:
                shapeX = shapes[i].data.ellipse.x;
                shapeY = shapes[i].data.ellipse.y;
                // Check if cursor is inside ellipse
                {
                    float dx = (cursorX - shapeX) / (float)shapes[i].data.ellipse.rx;
                    float dy = (cursorY - shapeY) / (float)shapes[i].data.ellipse.ry;
                    isCaught = (dx*dx + dy*dy) <= 1.0f;
                }
                break;
            case SHAPE_TRIANGLE:
                shapeX = shapes[i].data.triangle.cx;
                shapeY = shapes[i].data.triangle.cy;
                // For triangles, use the center point and radius
                {
                    float dx = cursorX - shapeX;
                    float dy = cursorY - shapeY;
                    float distSq = dx*dx + dy*dy;
                    isCaught = distSq <= shapes[i].data.triangle.radius * shapes[i].data.triangle.radius;
                }
                break;
            case SHAPE_POLYGON:
                shapeX = shapes[i].data.polygon.cx;
                shapeY = shapes[i].data.polygon.cy;
                // For polygons, use the center point and radius
                {
                    float dx = cursorX - shapeX;
                    float dy = cursorY - shapeY;
                    float distSq = dx*dx + dy*dy;
                    isCaught = distSq <= shapes[i].data.polygon.radius * shapes[i].data.polygon.radius;
                }
                break;
            default:
                shapeX = cursorX;
                shapeY = cursorY;
                break;
        }

        // Calculer le vecteur de fuite
        float dx = shapeX - cursorX;
        float dy = shapeY - cursorY;
        float dist = sqrt(dx*dx + dy*dy);

        // Si le curseur est proche, la forme fuit
        if (dist < 300) { // Increased detection distance from 200 to 300
            float speed = 8.0f; // Increased base speed from 5.0f to 8.0f
            
            // The closer the cursor, the faster the shape moves away
            float speedMultiplier = 1.0f + (300.0f - dist) / 80.0f; // More aggressive speed increase
            speed *= speedMultiplier;
            
            float moveX = (dx/dist) * speed;
            float moveY = (dy/dist) * speed;
            
            // Vérifier les limites avant de bouger
            int newX = shapeX + (int)moveX;
            int newY = shapeY + (int)moveY;
            
            // Calculate shape-specific margins based on shape type
            int marginX = 0, marginY = 0;
            switch (shapes[i].type) {
                case SHAPE_CIRCLE:
                    marginX = marginY = shapes[i].data.circle.radius;
                    break;
                case SHAPE_RECTANGLE:
                    marginX = shapes[i].data.rectangle.width / 2;
                    marginY = shapes[i].data.rectangle.height / 2;
                    break;
                case SHAPE_SQUARE:
                    marginX = marginY = shapes[i].data.square.c / 2;
                    break;
                case SHAPE_ELLIPSE:
                    marginX = shapes[i].data.ellipse.rx;
                    marginY = shapes[i].data.ellipse.ry;
                    break;
                case SHAPE_TRIANGLE:
                    marginX = marginY = shapes[i].data.triangle.radius;
                    break;
                case SHAPE_POLYGON:
                    marginX = marginY = shapes[i].data.polygon.radius;
                    break;
                default:
                    marginX = marginY = 25; // Default margin for other shapes
                    break;
            }
            
            // Add a small buffer to the margins
            marginX += 5;
            marginY += 5;
            
            // Garder la forme dans les limites de la fenêtre avec un rebond
            bool bounceX = newX < marginX || newX > windowWidth - marginX;
            bool bounceY = newY < marginY || newY > windowHeight - marginY;
            
            if (bounceX) {
                moveX = -moveX;
                // Push shape away from the boundary
                if (newX < marginX) newX = marginX;
                if (newX > windowWidth - marginX) newX = windowWidth - marginX;
            }
            if (bounceY) {
                moveY = -moveY;
                // Push shape away from the boundary
                if (newY < marginY) newY = marginY;
                if (newY > windowHeight - marginY) newY = windowHeight - marginY;
            }
            
            // Calculate final movement after boundary checks
            moveX = bounceX ? newX - shapeX : (int)moveX;
            moveY = bounceY ? newY - shapeY : (int)moveY;
            
            // Apply both movements at once
            moveShape(&shapes[i], (int)moveX, (int)moveY);
        }

        // Only catch if extremely close (making it much harder)
        if (isCaught && dist < 5) { // Reduced catch distance from 10 to 5
            game->score += 1;  // Changed from 10 to 1
            // Remove the shape by shifting all shapes after it
            for (int j = i; j < shapeCount - 1; j++) {
                shapes[j] = shapes[j + 1];
            }
            shapeCount--;
            continue;  // Skip the movement part for this shape
        }
    }
}

void renderGameUI(SDL_Renderer* renderer, TTF_Font* font, GameState* game) {
    SDL_Color textColor = {0, 0, 0, 255};
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    // Score et Timer en haut à gauche (seulement pendant le jeu)
    if (game->isPlaying) {
        char scoreText[64];
        if (game->won) {
            snprintf(scoreText, sizeof(scoreText), "You won! Score: %d", game->score);
        } else {
            snprintf(scoreText, sizeof(scoreText), "Score: %d   Time: %.1f", 
                     game->score, game->timeLeft);
        }
        
        // Use Blended rendering for better quality at larger sizes
        SDL_Surface* surface = TTF_RenderText_Blended(font, scoreText, textColor);
        if (!surface) return;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture) return;

        SDL_Rect textRect = {0, 0, 0, 0};
        SDL_QueryTexture(texture, NULL, NULL, &textRect.w, &textRect.h);
        // Make the text 2x larger when won, 1.5x otherwise
        float scale = game->won ? 2.0f : 1.5f;
        textRect.w = (int)(textRect.w * scale);
        textRect.h = (int)(textRect.h * scale);
        // Center the text horizontally and place it at the top with a small margin
        textRect.x = (windowWidth - textRect.w) / 2;
        textRect.y = 20;  // Increased margin from top
        SDL_RenderCopy(renderer, texture, NULL, &textRect);
        SDL_DestroyTexture(texture);

        // Remove the duplicate "You won!" message section since we now show it in the score text
    }

    // Message central pour la fin du jeu
    if (!game->isPlaying && game->timeLeft <= 0) {
        char centerMessage[64];
        if (game->won) {
            snprintf(centerMessage, sizeof(centerMessage), "GAME COMPLETE! - Final Score: %d", game->score);
        } else {
            snprintf(centerMessage, sizeof(centerMessage), "GAME OVER - Final Score: %d", game->score);
        }

        SDL_Surface* surface = TTF_RenderText_Blended(font, centerMessage, textColor);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect messageRect = {0, 0, 0, 0};
                SDL_QueryTexture(texture, NULL, NULL, &messageRect.w, &messageRect.h);
                // Make the text 2x larger
                messageRect.w = (int)(messageRect.w * 2.0);
                messageRect.h = (int)(messageRect.h * 2.0);
                // Centrer le message
                messageRect.x = (windowWidth - messageRect.w) / 2;
                messageRect.y = (windowHeight - messageRect.h) / 2;
                
                SDL_RenderCopy(renderer, texture, NULL, &messageRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }

    // Message en bas de l'écran (instructions)
    char message[128];
    if (!game->hasShapes) {
        snprintf(message, sizeof(message), "No shapes available! - Press SPACE to quit");
    } else if (!game->isPlaying && game->timeLeft <= 0) {
        snprintf(message, sizeof(message), "Press ENTER to restart or SPACE to quit");
    } else if (!game->isPlaying) {
        snprintf(message, sizeof(message), "Catch the shapes before they escape! - Press ENTER to start or SPACE to quit");
    } else {
        snprintf(message, sizeof(message), "Catch the shapes before they escape! - Press SPACE to quit");
    }

    // Rendu du message d'instructions
    SDL_Surface* surface = TTF_RenderText_Blended(font, message, textColor);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return;

    SDL_Rect messageRect = {0, 0, 0, 0};
    SDL_QueryTexture(texture, NULL, NULL, &messageRect.w, &messageRect.h);
    // Make the text 1.5x larger
    messageRect.w = (int)(messageRect.w * 1.5);
    messageRect.h = (int)(messageRect.h * 1.5);
    // Positionner le message en bas
    messageRect.x = (windowWidth - messageRect.w) / 2;
    messageRect.y = windowHeight - messageRect.h - 20;  // Increased margin from bottom
    
    SDL_RenderCopy(renderer, texture, NULL, &messageRect);
    SDL_DestroyTexture(texture);
}

void restoreShapes(GameState* game) {
    shapeCount = game->savedShapeCount;
    for (int i = 0; i < shapeCount; i++) {
        shapes[i] = game->savedShapes[i];
    }
} 