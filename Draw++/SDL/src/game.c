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
    
    // Vérifier si toutes les formes ont été attrapées
    if (shapeCount == 0) {
        game->isPlaying = false;
        game->won = true;
        #if DEBUG
            printf("You Win! Score: %d\n", game->score);
        #endif
        return;
    }
    
    if (game->timeLeft <= 0) {
        game->isPlaying = false;
        game->won = false;
        #if DEBUG
            printf("Game Over! Final Score: %d\n", game->score);
        #endif
        return;
    }

    // Obtenir les dimensions de la fenêtre
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(SDL_GetRenderer(SDL_GetWindowFromID(1)), &windowWidth, &windowHeight);
    
    // Faire fuir les formes du curseur
    for (int i = 0; i < shapeCount; i++) {
        // Calculer la direction de fuite
        int shapeX = 0, shapeY = 0;
        int shapeSize = 50; // Taille par défaut pour la limite
        
        // Obtenir le centre et la taille de la forme
        switch (shapes[i].type) {
            case SHAPE_CIRCLE:
                shapeX = shapes[i].data.circle.x;
                shapeY = shapes[i].data.circle.y;
                shapeSize = shapes[i].data.circle.radius * 2;
                break;
            case SHAPE_RECTANGLE:
                shapeX = shapes[i].data.rectangle.x + shapes[i].data.rectangle.width/2;
                shapeY = shapes[i].data.rectangle.y + shapes[i].data.rectangle.height/2;
                break;
            case SHAPE_SQUARE:
                shapeX = shapes[i].data.square.x + shapes[i].data.square.c/2;
                shapeY = shapes[i].data.square.y + shapes[i].data.square.c/2;
                break;
            case SHAPE_ELLIPSE:
                shapeX = shapes[i].data.ellipse.x;
                shapeY = shapes[i].data.ellipse.y;
                break;
            case SHAPE_POLYGON:
            case SHAPE_TRIANGLE:
                shapeX = shapes[i].data.polygon.cx;
                shapeY = shapes[i].data.polygon.cy;
                break;
            case SHAPE_ARC:
                shapeX = shapes[i].data.arc.x;
                shapeY = shapes[i].data.arc.y;
                break;
            case SHAPE_LINE:
                shapeX = (shapes[i].data.line.x1 + shapes[i].data.line.x2)/2;
                shapeY = (shapes[i].data.line.y1 + shapes[i].data.line.y2)/2;
                break;
            case SHAPE_ROUNDED_RECTANGLE:
                shapeX = (shapes[i].data.rounded_rectangle.x1 + shapes[i].data.rounded_rectangle.x2)/2;
                shapeY = (shapes[i].data.rounded_rectangle.y1 + shapes[i].data.rounded_rectangle.y2)/2;
                break;
        }

        // Calculer le vecteur de fuite
        float dx = shapeX - cursorX;
        float dy = shapeY - cursorY;
        float dist = sqrt(dx*dx + dy*dy);

        // Si le curseur est proche, la forme fuit
        if (dist < 150) { // Distance de détection augmentée
            float speed = 2.5f; // Vitesse encore plus réduite
            int moveX = (int)((dx/dist) * speed);
            int moveY = (int)((dy/dist) * speed);
            
            // Vérifier les limites avant de bouger
            int newX = shapeX + moveX;
            int newY = shapeY + moveY;
            
            // Garder la forme dans les limites de la fenêtre
            if (newX >= shapeSize && newX <= windowWidth - shapeSize) {
                moveShape(&shapes[i], moveX, 0);
            }
            if (newY >= shapeSize && newY <= windowHeight - shapeSize) {
                moveShape(&shapes[i], 0, moveY);
            }
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
        snprintf(scoreText, sizeof(scoreText), "Score: %d   Time: %.1f", 
                 game->score, game->timeLeft);
        
        SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText, textColor);
        if (!surface) return;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture) return;

        SDL_Rect textRect = {10, 10, 0, 0};
        SDL_QueryTexture(texture, NULL, NULL, &textRect.w, &textRect.h);
        SDL_RenderCopy(renderer, texture, NULL, &textRect);
        SDL_DestroyTexture(texture);
    }

    // Message central pour la fin du jeu
    if (!game->isPlaying && game->timeLeft <= 0) {
        char centerMessage[64];
        if (game->won) {
            snprintf(centerMessage, sizeof(centerMessage), "YOU WIN! - Score: %d", game->score);
        } else {
            snprintf(centerMessage, sizeof(centerMessage), "GAME OVER - Score: %d", game->score);
        }

        SDL_Surface* surface = TTF_RenderText_Solid(font, centerMessage, textColor);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect messageRect = {0, 0, 0, 0};
                SDL_QueryTexture(texture, NULL, NULL, &messageRect.w, &messageRect.h);
                
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
    SDL_Surface* surface = TTF_RenderText_Solid(font, message, textColor);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return;

    SDL_Rect messageRect = {0, 0, 0, 0};
    SDL_QueryTexture(texture, NULL, NULL, &messageRect.w, &messageRect.h);
    
    // Positionner le message en bas
    messageRect.x = (windowWidth - messageRect.w) / 2;
    messageRect.y = windowHeight - messageRect.h - 10;
    
    SDL_RenderCopy(renderer, texture, NULL, &messageRect);
    SDL_DestroyTexture(texture);
}

void restoreShapes(GameState* game) {
    shapeCount = game->savedShapeCount;
    for (int i = 0; i < shapeCount; i++) {
        shapes[i] = game->savedShapes[i];
    }
} 