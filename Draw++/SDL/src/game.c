#include "../files.h/game.h"
#include <math.h>

/**
 * @brief Get the name of the specified game type
 * @param type The game type (GAME_ESCAPE or GAME_DEFENSE)
 * @return The name of the game as a string
 */
const char* getGameName(GameType type) {
    switch (type) {
        case GAME_ESCAPE:
            return "Escape Game";
        case GAME_DEFENSE:
            return "Shape Defense";
        default:
            return "Unknown Game";
    }
}

/**
 * @brief Initialize a new game state
 * @param game Pointer to the game state to initialize
 * 
 * Sets up initial values for score, timers, and game mode.
 * Saves the current shapes and sets game-specific variables based on the game type.
 */
void initGame(GameState* game) {
    game->score = 0;
    game->isPlaying = false;
    game->isGameMode = false;
    game->won = false;
    game->winMessageTimer = 0.0f;
    game->gameJustEnded = false;
    game->currentGame = GAME_ESCAPE;  // Set default game mode
    game->timeLeft = 60.0f;  // Default time
    game->enemyCount = 0;
    game->spawnTimer = 2.0f;
    game->basesRemaining = 0;
    
    // Save existing shapes
    game->savedShapeCount = shapeCount;
    for (int i = 0; i < shapeCount; i++) {
        game->savedShapes[i] = shapes[i];
    }
    
    game->hasShapes = (shapeCount > 0);
    
    // Set game-specific variables
    if (game->currentGame == GAME_DEFENSE) {
        game->timeLeft = 60.0f;  // 1 minute for defense game
        game->enemyCount = 0;
        game->spawnTimer = 2.0f;  // Initial spawn timer
        game->basesRemaining = game->savedShapeCount;
    } else {
        game->timeLeft = game->savedShapeCount * 2.0f;  // 5 seconds per shape for escape game
    }
}

/**
 * @brief Update the game state each frame
 * @param game Pointer to the current game state
 * @param deltaTime Time elapsed since last frame
 * @param cursorX Current X position of the cursor
 * @param cursorY Current Y position of the cursor
 * @param window SDL window pointer
 * @param renderer SDL renderer pointer
 * 
 * Handles game logic updates including time management, win conditions,
 * and game-specific updates for each game mode.
 */
void updateGame(GameState* game, float deltaTime, int cursorX, int cursorY, SDL_Window* window, SDL_Renderer* renderer) {
    if (!game || !window || !renderer) return;  // Safety check
    
    if (!game->isPlaying) {
        // Update win message timer if game just ended
        if (game->gameJustEnded && game->winMessageTimer < 3.0f) {
            game->winMessageTimer += deltaTime;
            if (game->winMessageTimer >= 3.0f) {
                game->gameJustEnded = false;  // Reset the flag after message duration
            }
        }
        return;
    }
    
    // Update time
    game->timeLeft -= deltaTime;

    // Check end game conditions based on game type
    switch (game->currentGame) {
        case GAME_ESCAPE:
            if (game->timeLeft <= 0 || shapeCount == 0) {
                game->isPlaying = false;
                game->won = (shapeCount == 0);
                game->winMessageTimer = 0.0f;
                game->gameJustEnded = true;
                return;
            }
            escapeRun(game, cursorX, cursorY);
            break;
            
        case GAME_DEFENSE:
            updateDefenseGame(game, deltaTime, cursorX, cursorY, window, renderer);
            break;
            
        case GAME_COUNT:
            // Handle invalid game type
            game->isPlaying = false;
            game->won = false;
            game->winMessageTimer = 0.0f;
            game->gameJustEnded = true;
            break;
    }
}

/**
 * @brief Render the game UI elements
 * @param renderer SDL renderer pointer
 * @param font TTF font for text rendering
 * @param game Pointer to the current game state
 * 
 * Renders score, timer, game messages, and instructions based on the current game state.
 */
void renderGameUI(SDL_Renderer* renderer, TTF_Font* font, GameState* game) {
    SDL_Color textColor = {0, 0, 0, 255};
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);

    // Score and Timer in top center (only during game)
    if (game->isPlaying) {
        char scoreText[64];
        if (game->currentGame == GAME_DEFENSE) {
            snprintf(scoreText, sizeof(scoreText), "Score: %d   Time: %.1f   Bases: %d", 
                     game->score, game->timeLeft, game->basesRemaining);
        } else {
            snprintf(scoreText, sizeof(scoreText), "Score: %d   Time: %.1f", 
                     game->score, game->timeLeft);
        }
        
        // Use Blended rendering for better quality
        SDL_Surface* surface = TTF_RenderText_Blended(font, scoreText, textColor);
        if (!surface) return;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!texture) return;

        SDL_Rect textRect = {0, 0, 0, 0};
        SDL_QueryTexture(texture, NULL, NULL, &textRect.w, &textRect.h);
        textRect.w = (int)(textRect.w * 1.5f);
        textRect.h = (int)(textRect.h * 1.5f);
        textRect.x = (windowWidth - textRect.w) / 2;
        textRect.y = 20;
        SDL_RenderCopy(renderer, texture, NULL, &textRect);
        SDL_DestroyTexture(texture);
        
        // Render defense game enemies if in defense mode
        if (game->currentGame == GAME_DEFENSE) {
            renderDefenseGame(renderer, game);
        }
    }

    // Show win/game over message only if game just ended
    if (game->gameJustEnded && game->winMessageTimer < 3.0f) {
        char centerMessage[64];
        if (game->won) {
            snprintf(centerMessage, sizeof(centerMessage), "VICTORY ! Final Score: %d", game->score);
        } else {
            snprintf(centerMessage, sizeof(centerMessage), "GAME OVER ! Final Score: %d", game->score);
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
                // Center the message
                messageRect.x = (windowWidth - messageRect.w) / 2;
                messageRect.y = (windowHeight - messageRect.h) / 2;
                
                SDL_RenderCopy(renderer, texture, NULL, &messageRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }

    // Message at bottom of screen (instructions)
    char message[128];
    if (!game->hasShapes) {
        snprintf(message, sizeof(message), "No shapes available! - Press SPACE to quit");
    } else if (!game->isPlaying && game->gameJustEnded) {
        snprintf(message, sizeof(message), "Press ENTER to restart GameMode: %s - G to change mode - SPACE to quit", getGameName(game->currentGame));
    } else if (!game->isPlaying) {
        snprintf(message, sizeof(message), "Press ENTER to start GameMode: %s - G to change mode - SPACE to quit", getGameName(game->currentGame));
    } else {
        if (game->currentGame == GAME_DEFENSE) {
            snprintf(message, sizeof(message), "Touch enemies to destroy them! Protect your shapes! - Press SPACE to quit");
        } else {
            snprintf(message, sizeof(message), "Press SPACE to quit");
        }
    }

    // Render bottom message
    SDL_Surface* surface = TTF_RenderText_Blended(font, message, textColor);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return;

    SDL_Rect messageRect = {0, 0, 0, 0};
    SDL_QueryTexture(texture, NULL, NULL, &messageRect.w, &messageRect.h);
    messageRect.w = (int)(messageRect.w * 1.5);
    messageRect.h = (int)(messageRect.h * 1.5);
    messageRect.x = (windowWidth - messageRect.w) / 2;
    messageRect.y = windowHeight - messageRect.h - 20;
    
    SDL_RenderCopy(renderer, texture, NULL, &messageRect);
    SDL_DestroyTexture(texture);
}

/**
 * @brief Restore shapes to their original state
 * @param game Pointer to the current game state
 * 
 * Restores all shapes from the saved state, maintaining their original properties.
 */
void restoreShapes(GameState* game) {
    // Clear current shapes
    shapeCount = 0;
    
    // Restore saved shapes to their original positions
    for (int i = 0; i < game->savedShapeCount; i++) {
        shapes[i] = game->savedShapes[i];
    }
    shapeCount = game->savedShapeCount;
}

/**
 * @brief Reset shapes to random positions
 * @param game Pointer to the current game state
 * @param window SDL window pointer
 * @param renderer SDL renderer pointer
 * 
 * Repositions all shapes randomly within the center area of the screen
 * while maintaining their original properties and dimensions.
 */
void resetShapes(GameState* game, SDL_Window* window, SDL_Renderer* renderer) {
    // Clear current shapes
    shapeCount = 0;
    
    // Get window dimensions for random positioning
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);
    
    // Calculate center area (middle 50% of screen)
    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;
    int spreadX = windowWidth / 4;  // 25% of width on each side of center
    int spreadY = windowHeight / 4; // 25% of height on each side of center
    
    // Restore saved shapes with random positions
    for (int i = 0; i < game->savedShapeCount; i++) {
        shapes[i] = game->savedShapes[i];
    }
    shapeCount = game->savedShapeCount;
    
    // Reset game-specific variables
    if (game->currentGame == GAME_DEFENSE) {
        game->timeLeft = 60.0f;
        game->enemyCount = 0;
        game->spawnTimer = 2.0f;
        game->basesRemaining = game->savedShapeCount;
        game->score = 0;
        game->won = false;
        game->gameJustEnded = false;
        game->winMessageTimer = 0.0f;
        initDefenseMode(game);
    } else {
        game->timeLeft = game->savedShapeCount * 10.0f;
        game->score = 0;
        game->won = false;
        game->gameJustEnded = false;
        game->winMessageTimer = 0.0f;
    }
    
    // Randomize positions in center area
    for (int i = 0; i < shapeCount; i++) {
        // Calculate margins based on shape type
        int marginX = 0, marginY = 0;
        switch (shapes[i].type) {
            case SHAPE_CIRCLE:
                marginX = marginY = shapes[i].data.circle.radius + 10;
                shapes[i].data.circle.x = centerX + (rand() % (2 * spreadX) - spreadX);
                shapes[i].data.circle.y = centerY + (rand() % (2 * spreadY) - spreadY);
                break;
            case SHAPE_RECTANGLE:
                marginX = shapes[i].data.rectangle.width / 2 + 10;
                marginY = shapes[i].data.rectangle.height / 2 + 10;
                shapes[i].data.rectangle.x = centerX + (rand() % (2 * spreadX) - spreadX);
                shapes[i].data.rectangle.y = centerY + (rand() % (2 * spreadY) - spreadY);
                break;
            case SHAPE_SQUARE:
                marginX = marginY = shapes[i].data.square.c / 2 + 10;
                shapes[i].data.square.x = centerX + (rand() % (2 * spreadX) - spreadX);
                shapes[i].data.square.y = centerY + (rand() % (2 * spreadY) - spreadY);
                break;
            case SHAPE_ELLIPSE:
                marginX = shapes[i].data.ellipse.rx + 10;
                marginY = shapes[i].data.ellipse.ry + 10;
                shapes[i].data.ellipse.x = centerX + (rand() % (2 * spreadX) - spreadX);
                shapes[i].data.ellipse.y = centerY + (rand() % (2 * spreadY) - spreadY);
                break;
            case SHAPE_TRIANGLE:
                marginX = marginY = shapes[i].data.triangle.radius + 10;
                shapes[i].data.triangle.cx = centerX + (rand() % (2 * spreadX) - spreadX);
                shapes[i].data.triangle.cy = centerY + (rand() % (2 * spreadY) - spreadY);
                break;
            case SHAPE_POLYGON:
                marginX = marginY = shapes[i].data.polygon.radius + 10;
                shapes[i].data.polygon.cx = centerX + (rand() % (2 * spreadX) - spreadX);
                shapes[i].data.polygon.cy = centerY + (rand() % (2 * spreadY) - spreadY);
                break;
            case SHAPE_LINE:
                marginX = marginY = shapes[i].data.line.thickness + 10;
                // Calculate current line length
                int oldDx = shapes[i].data.line.x2 - shapes[i].data.line.x1;
                int oldDy = shapes[i].data.line.y2 - shapes[i].data.line.y1;
                float length = sqrt(oldDx * oldDx + oldDy * oldDy);
                float angle = atan2(oldDy, oldDx);
                
                // Move starting point
                shapes[i].data.line.x1 = centerX + (rand() % (2 * spreadX) - spreadX);
                shapes[i].data.line.y1 = centerY + (rand() % (2 * spreadY) - spreadY);
                
                // Calculate new endpoint keeping same length and angle
                shapes[i].data.line.x2 = shapes[i].data.line.x1 + (int)(length * cos(angle));
                shapes[i].data.line.y2 = shapes[i].data.line.y1 + (int)(length * sin(angle));
                break;
            case SHAPE_ARC:
                marginX = marginY = shapes[i].data.arc.radius + 10;
                shapes[i].data.arc.x = centerX + (rand() % (2 * spreadX) - spreadX);
                shapes[i].data.arc.y = centerY + (rand() % (2 * spreadY) - spreadY);
                break;
        }
    }
}

/**
 * @brief Run the escape game logic
 * @param game Pointer to the current game state
 * @param cursorX Current X position of the cursor
 * @param cursorY Current Y position of the cursor
 * 
 * Handles shape movement, collision detection, and capture mechanics
 * for the shape escape game mode.
 */
void escapeRun(GameState* game, int cursorX, int cursorY) {
    if (!game || shapeCount <= 0) return;  // Safety check
    
    // Get window dimensions
    SDL_Window* window = SDL_GetWindowFromID(1);
    if (!window) return;  // Safety check
    SDL_Renderer* renderer = SDL_GetRenderer(window);
    if (!renderer) return;  // Safety check
    
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);
    
    // Process each shape
    for (int i = shapeCount - 1; i >= 0; i--) {
        if (i >= MAX_SHAPES) continue;  // Safety check
        
        // Calculate shape center and check if caught
        int shapeX = 0, shapeY = 0;
        bool isCaught = false;
        
        // Get shape center and check collision
        switch (shapes[i].type) {
            case SHAPE_CIRCLE:
                if (shapes[i].data.circle.radius <= 0) continue;  // Safety check
                shapeX = shapes[i].data.circle.x;
                shapeY = shapes[i].data.circle.y;
                {
                    isCaught = isPointInCircle(cursorX, cursorY,
                        shapes[i].data.circle.x, shapes[i].data.circle.y,
                        shapes[i].data.circle.radius);
                }
                break;
            case SHAPE_RECTANGLE:
                if (shapes[i].data.rectangle.width <= 0 || shapes[i].data.rectangle.height <= 0) continue;  // Safety check
                shapeX = shapes[i].data.rectangle.x + shapes[i].data.rectangle.width/2;
                shapeY = shapes[i].data.rectangle.y + shapes[i].data.rectangle.height/2;
                {
                    isCaught = isPointInRectangle(cursorX, cursorY,
                        shapes[i].data.rectangle.x, shapes[i].data.rectangle.y,
                        shapes[i].data.rectangle.width, shapes[i].data.rectangle.height);
                }
                break;
            case SHAPE_SQUARE:
                if (shapes[i].data.square.c <= 0) continue;  // Safety check
                shapeX = shapes[i].data.square.x + shapes[i].data.square.c/2;
                shapeY = shapes[i].data.square.y + shapes[i].data.square.c/2;
                {
                    isCaught = isPointInSquare(cursorX, cursorY,
                        shapes[i].data.square.x, shapes[i].data.square.y,
                        shapes[i].data.square.c);
                }

                break;
            case SHAPE_ELLIPSE:
                if (shapes[i].data.ellipse.rx <= 0 || shapes[i].data.ellipse.ry <= 0) continue;  // Safety check
                shapeX = shapes[i].data.ellipse.x;
                shapeY = shapes[i].data.ellipse.y;
                {
                    isCaught = isPointInEllipse(cursorX, cursorY,
                        shapes[i].data.ellipse.x, shapes[i].data.ellipse.y,
                        shapes[i].data.ellipse.rx, shapes[i].data.ellipse.ry);
                }
                break;
            case SHAPE_TRIANGLE:
                if (shapes[i].data.triangle.radius <= 0) continue;  // Safety check
                shapeX = shapes[i].data.triangle.cx;
                shapeY = shapes[i].data.triangle.cy;
                {
                    isCaught = isPointInTriangle(cursorX, cursorY,
                        shapes[i].data.triangle.cx, shapes[i].data.triangle.cy,
                        shapes[i].data.triangle.radius);
                }
                break;
            case SHAPE_POLYGON:
                if (shapes[i].data.polygon.radius <= 0) continue;  // Safety check
                shapeX = shapes[i].data.polygon.cx;
                shapeY = shapes[i].data.polygon.cy;
                {
                    isCaught = isPointInPolygon(cursorX, cursorY,
                        shapes[i].data.polygon.cx, shapes[i].data.polygon.cy,
                        shapes[i].data.polygon.radius,
                        shapes[i].data.polygon.sides);
                }
                break;
            case SHAPE_LINE:
                if (shapes[i].data.line.thickness <= 0) continue;  // Safety check
                // Calculer le centre de la ligne
                shapeX = (shapes[i].data.line.x1 + shapes[i].data.line.x2) / 2;
                shapeY = (shapes[i].data.line.y1 + shapes[i].data.line.y2) / 2;
                {
                    isCaught = isPointInLine(cursorX, cursorY,
                        shapes[i].data.line.x1, shapes[i].data.line.y1,
                        shapes[i].data.line.x2, shapes[i].data.line.y2,
                        shapes[i].data.line.thickness,
                        shapes[i].rotation);
                }
                break;
            case SHAPE_ARC:
                if (shapes[i].data.arc.radius <= 0) continue;  // Safety check
                shapeX = shapes[i].data.arc.x;
                shapeY = shapes[i].data.arc.y;
                {
                    isCaught = isPointInArc(cursorX, cursorY,
                        shapes[i].data.arc.x, shapes[i].data.arc.y,
                        shapes[i].data.arc.radius,
                        shapes[i].data.arc.start_angle,
                        shapes[i].data.arc.end_angle);
                }
                break;
            default:
                continue;  // Skip unsupported shapes
        }

        // Calculate escape vector
        float dx = shapeX - cursorX;
        float dy = shapeY - cursorY;
        float dist = sqrt(dx*dx + dy*dy);
        if (dist < 0.1f) dist = 0.1f;  // Prevent division by zero

        // If cursor is close, make shape run away
        if (dist < 300) {
            float speed = 12.0f;
            float speedMultiplier = 1.0f + (300.0f - dist) / 60.0f;
            speed *= speedMultiplier;
            
            float moveX = (dx/dist) * speed;
            float moveY = (dy/dist) * speed;
            
            int newX = shapeX + (int)moveX;
            int newY = shapeY + (int)moveY;
            
            // Calculate margins for shape
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
                case SHAPE_LINE:
                    marginX = marginY = shapes[i].data.line.thickness;
                    break;
                case SHAPE_ARC:
                    marginX = marginY = shapes[i].data.arc.radius;
                    break;
                default:
                    marginX = marginY = 25;
                    break;
            }
            
            marginX += 5;
            marginY += 5;
            
            // Handle window bounds with safety checks
            if (marginX > windowWidth/2) marginX = windowWidth/2;
            if (marginY > windowHeight/2) marginY = windowHeight/2;
            
            bool bounceX = newX < marginX || newX > windowWidth - marginX;
            bool bounceY = newY < marginY || newY > windowHeight - marginY;
            
            if (bounceX) {
                moveX = -moveX;
                if (newX < marginX) newX = marginX;
                if (newX > windowWidth - marginX) newX = windowWidth - marginX;
            }
            if (bounceY) {
                moveY = -moveY;
                if (newY < marginY) newY = marginY;
                if (newY > windowHeight - marginY) newY = windowHeight - marginY;
            }
            
            moveX = bounceX ? newX - shapeX : (int)moveX;
            moveY = bounceY ? newY - shapeY : (int)moveY;
            
            if (i < shapeCount) {  // Safety check before moving
                moveShape(&shapes[i], (int)moveX, (int)moveY);
            }
        }

        // Handle shape capture with safety checks
        if (isCaught && dist < 5 && i < shapeCount) {
            game->score += 1;
            deleteShape(i);
            
            if (shapeCount == 0) {
                game->isPlaying = false;
                game->won = true;
                game->gameJustEnded = true;
                game->winMessageTimer = 0.0f;
                return;
            }
        }
    }
}

/**
 * @brief Initialize the defense game mode
 * @param game Pointer to the current game state
 * 
 * Sets up initial values for the defense game mode and normalizes
 * the size of base shapes for balanced gameplay.
 */
void initDefenseMode(GameState* game) {
    // Initialize defense-specific variables
    game->enemyCount = 0;
    game->spawnTimer = 0.0f;  // Start spawning immediately
    game->basesRemaining = game->savedShapeCount;
    game->timeLeft = 60.0f;  // 1 minute game

    // Normalize base shapes size for better gameplay
    for (int i = 0; i < shapeCount; i++) {
        switch (shapes[i].type) {
            case SHAPE_CIRCLE:
                if (shapes[i].data.circle.radius > 50)
                    shapes[i].data.circle.radius = 50;
                break;
            case SHAPE_RECTANGLE:
                if (shapes[i].data.rectangle.width > 100)
                    shapes[i].data.rectangle.width = 100;
                if (shapes[i].data.rectangle.height > 100)
                    shapes[i].data.rectangle.height = 100;
                break;
            case SHAPE_SQUARE:
                if (shapes[i].data.square.c > 100)
                    shapes[i].data.square.c = 100;
                break;
            case SHAPE_ELLIPSE:
                if (shapes[i].data.ellipse.rx > 50)
                    shapes[i].data.ellipse.rx = 50;
                if (shapes[i].data.ellipse.ry > 50)
                    shapes[i].data.ellipse.ry = 50;
                break;
            case SHAPE_TRIANGLE:
                if (shapes[i].data.triangle.radius > 50)
                    shapes[i].data.triangle.radius = 50;
                break;
            case SHAPE_POLYGON:
                if (shapes[i].data.polygon.radius > 50)
                    shapes[i].data.polygon.radius = 50;
                break;
            case SHAPE_LINE:
                if (shapes[i].data.line.thickness > 10)
                    shapes[i].data.line.thickness = 10;
                break;
            case SHAPE_ARC:
                if (shapes[i].data.arc.radius > 50)
                    shapes[i].data.arc.radius = 50;
                break;
        }
    }
}

/**
 * @brief Spawn a new enemy in defense mode
 * @param game Pointer to the current game state
 * @param window SDL window pointer
 * @param renderer SDL renderer pointer
 * 
 * Creates a new enemy shape with random position and movement pattern.
 */
void spawnEnemy(GameState* game, SDL_Window* window, SDL_Renderer* renderer) {
    if (game->enemyCount >= 50) return;  // Max enemy limit
    
    // Get window dimensions
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);
    
    // Create a new enemy shape
    EnemyShape* enemy = &game->enemies[game->enemyCount];
    enemy->active = true;
    
    // Initialize the shape structure properly
    enemy->shape.type = SHAPE_CIRCLE;  // Changed to circle
    enemy->shape.selected = false;
    enemy->shape.isAnimating = false;
    enemy->shape.zoom = 1.0f;
    enemy->shape.zoom_direction = 1.0f;
    enemy->shape.bounce_velocity = 0;
    enemy->shape.bounce_direction = 0;
    enemy->shape.data.circle.radius = 15;  // Set radius before using it for spawn position
    enemy->shape.typeForm = (char*)malloc(7);
    if (enemy->shape.typeForm) {
        strcpy(enemy->shape.typeForm, "filled");
    }
    
    // Randomly choose spawn side (0: top, 1: right, 2: bottom, 3: left)
    int side = rand() % 4;
    float angle;
    
    // Ensure spawn position is outside window with a margin
    int tolerance = enemy->shape.data.circle.radius * 2;
    
    switch (side) {
        case 0:  // Top
            enemy->shape.data.circle.x = tolerance + rand() % (windowWidth - 2 * tolerance);  // Keep away from corners
            enemy->shape.data.circle.y = -tolerance;  // Just above window
            angle = (float)(M_PI / 2);  // Move straight down
            break;
        case 1:  // Right
            enemy->shape.data.circle.x = windowWidth + tolerance;  // Just right of window
            enemy->shape.data.circle.y = tolerance + rand() % (windowHeight - 2 * tolerance);  // Keep away from corners
            angle = (float)M_PI;  // Move straight left
            break;
        case 2:  // Bottom
            enemy->shape.data.circle.x = tolerance + rand() % (windowWidth - 2 * tolerance);  // Keep away from corners
            enemy->shape.data.circle.y = windowHeight + tolerance;  // Just below window
            angle = (float)(3 * M_PI / 2);  // Move straight up
            break;
        case 3:  // Left
            enemy->shape.data.circle.x = -tolerance;  // Just left of window
            enemy->shape.data.circle.y = tolerance + rand() % (windowHeight - 2 * tolerance);  // Keep away from corners
            angle = 0.0f;  // Move straight right
            break;
    }
    
    // Add some randomness to the angle (±30 degrees)
    angle += ((rand() % 60 - 30) * M_PI / 180.0f);
    
    int baseSpeed = 1;  // Minimum base speed in pixels per frame
    int speedVariation = rand() % 2 + 1;  // 1 to 2 pixel variation
    int totalSpeed = baseSpeed + speedVariation;  // 1 to 2 pixels per frame
    
    // Calculate X and Y components
    enemy->speedX = (int)(totalSpeed * cos(angle));
    enemy->speedY = (int)(totalSpeed * sin(angle));
    
    // Ensure minimum speed components
    if (enemy->speedX == 0) enemy->speedX = (cos(angle) >= 0) ? 1 : -1;
    if (enemy->speedY == 0) enemy->speedY = (sin(angle) >= 0) ? 1 : -1;
    
    // Setup enemy shape properties
    enemy->shape.data.circle.radius = 15;
    enemy->shape.color.r = 255;
    enemy->shape.color.g = 0;
    enemy->shape.color.b = 0;
    enemy->shape.color.a = 255;
    
    game->enemyCount++;
}

/**
 * @brief Update the defense game state
 * @param game Pointer to the current game state
 * @param deltaTime Time elapsed since last frame
 * @param cursorX Current X position of the cursor
 * @param cursorY Current Y position of the cursor
 * @param window SDL window pointer
 * @param renderer SDL renderer pointer
 * 
 * Handles enemy spawning, movement, and collision detection for defense mode.
 */
void updateDefenseGame(GameState* game, float deltaTime, int cursorX, int cursorY, SDL_Window* window, SDL_Renderer* renderer) {
    if (!game->isPlaying) return;
    
    // Update time
    game->timeLeft -= deltaTime;
    if (game->timeLeft <= 0 || game->basesRemaining <= 0) {
        game->isPlaying = false;
        game->won = (game->basesRemaining > 0 && game->timeLeft <= 0);  // Win if time ran out with bases remaining
        game->winMessageTimer = 0.0f;
        game->gameJustEnded = true;
        // Clean up all enemies
        for (int i = 0; i < game->enemyCount; i++) {
            if (game->enemies[i].shape.typeForm) {
                free(game->enemies[i].shape.typeForm);
            }
        }
        game->enemyCount = 0;  // Clear all enemies when game ends
        return;
    }
    
    // Spawn new enemies
    game->spawnTimer -= deltaTime;
    if (game->spawnTimer <= 0) {
        // Spawn multiple enemies at once
        int enemiesToSpawn = 2 + (60.0f - game->timeLeft) / 15.0f;  // 2 to 6 enemies at once
        for (int i = 0; i < enemiesToSpawn && game->enemyCount < 50; i++) {
            spawnEnemy(game, window, renderer);
        }
        game->spawnTimer = 3.0f - (60.0f - game->timeLeft) / 30.0f;  // Spawn every 3 to 1 seconds
        if (game->spawnTimer < 1.0f) game->spawnTimer = 1.0f;  // Minimum spawn time
    }
    
    // Get window dimensions
    int windowWidth, windowHeight;
    SDL_GetRendererOutputSize(renderer, &windowWidth, &windowHeight);
    
    // Update enemies
    for (int i = 0; i < game->enemyCount; i++) {
        if (!game->enemies[i].active) continue;
        
        // Move enemy
        game->enemies[i].shape.data.circle.x += game->enemies[i].speedX;
        game->enemies[i].shape.data.circle.y += game->enemies[i].speedY;
        
        // Check if enemy is trying to exit the window (not when entering)
        float enemyX = game->enemies[i].shape.data.circle.x;
        float enemyY = game->enemies[i].shape.data.circle.y;
        float tolerance = game->enemies[i].shape.data.circle.radius;
        
        // Check if enemy is near any boundary and moving towards it
        bool isExiting = false;
        
        // Near left boundary and moving left
        if (enemyX <= tolerance && game->enemies[i].speedX < 0) {
            isExiting = true;
        }
        // Near right boundary and moving right
        if (enemyX >= windowWidth - tolerance && game->enemies[i].speedX > 0) {
            isExiting = true;
        }
        // Near top boundary and moving up
        if (enemyY <= tolerance && game->enemies[i].speedY < 0) {
            isExiting = true;
        }
        // Near bottom boundary and moving down
        if (enemyY >= windowHeight - tolerance && game->enemies[i].speedY > 0) {
            isExiting = true;
        }

        if (isExiting) {
            // Free memory for typeForm
            if (game->enemies[i].shape.typeForm) {
                free(game->enemies[i].shape.typeForm);
            }
            // Remove enemy by swapping with last active enemy and decrementing count
            if (i < game->enemyCount - 1) {
                game->enemies[i] = game->enemies[game->enemyCount - 1];
            }
            game->enemyCount--;
            i--;  // Recheck this index since we swapped in a new enemy
            continue;
        }
        
        // Check collision with cursor (shooting)
        float dx = cursorX - game->enemies[i].shape.data.circle.x;
        float dy = cursorY - game->enemies[i].shape.data.circle.y;
        float enemyR = game->enemies[i].shape.data.circle.radius;
        if (dx*dx + dy*dy < enemyR * enemyR) {  // Using circle's radius for hit detection
            game->score += 1;
            // Free memory for typeForm
            if (game->enemies[i].shape.typeForm) {
                free(game->enemies[i].shape.typeForm);
            }
            // Remove enemy by swapping with last active enemy and decrementing count
            if (i < game->enemyCount - 1) {
                game->enemies[i] = game->enemies[game->enemyCount - 1];
            }
            game->enemyCount--;
            i--;  // Recheck this index since we swapped in a new enemy
            continue;
        }
        
        // Check collision with shapes (bases)
        bool collided = false;
        for (int j = 0; j < shapeCount && !collided; j++) {
            float enemyX = game->enemies[i].shape.data.circle.x;
            float enemyY = game->enemies[i].shape.data.circle.y;
            float enemyRadius = game->enemies[i].shape.data.circle.radius;
            
            switch (shapes[j].type) {
                case SHAPE_CIRCLE: {
                    float dx = enemyX - shapes[j].data.circle.x;
                    float dy = enemyY - shapes[j].data.circle.y;
                    float minDist = enemyRadius + shapes[j].data.circle.radius;
                    if (dx*dx + dy*dy < minDist*minDist) {
                        collided = true;
                    }
                    break;
                }
                case SHAPE_RECTANGLE: {
                    // Simple AABB collision
                    float shapeX = shapes[j].data.rectangle.x + shapes[j].data.rectangle.width/2;
                    float shapeY = shapes[j].data.rectangle.y + shapes[j].data.rectangle.height/2;
                    float dx = fabs(enemyX - shapeX);
                    float dy = fabs(enemyY - shapeY);
                    if (dx < shapes[j].data.rectangle.width/2 + enemyRadius &&
                        dy < shapes[j].data.rectangle.height/2 + enemyRadius) {
                        collided = true;
                    }
                    break;
                }
                case SHAPE_SQUARE: {
                    float shapeX = shapes[j].data.square.x + shapes[j].data.square.c/2;
                    float shapeY = shapes[j].data.square.y + shapes[j].data.square.c/2;
                    float dx = fabs(enemyX - shapeX);
                    float dy = fabs(enemyY - shapeY);
                    if (dx < shapes[j].data.square.c/2 + enemyRadius &&
                        dy < shapes[j].data.square.c/2 + enemyRadius) {
                        collided = true;
                    }
                    break;
                }
                case SHAPE_ELLIPSE: {
                    float dx = (enemyX - shapes[j].data.ellipse.x) / (float)(shapes[j].data.ellipse.rx + enemyRadius);
                    float dy = (enemyY - shapes[j].data.ellipse.y) / (float)(shapes[j].data.ellipse.ry + enemyRadius);
                    if (dx*dx + dy*dy <= 1.0f) {
                        collided = true;
                    }
                    break;
                }
                case SHAPE_TRIANGLE: {
                    float dx = enemyX - shapes[j].data.triangle.cx;
                    float dy = enemyY - shapes[j].data.triangle.cy;
                    float minDist = enemyRadius + shapes[j].data.triangle.radius;
                    if (dx*dx + dy*dy < minDist*minDist) {
                        collided = true;
                    }
                    break;
                }
                case SHAPE_POLYGON: {
                    float dx = enemyX - shapes[j].data.polygon.cx;
                    float dy = enemyY - shapes[j].data.polygon.cy;
                    float minDist = enemyRadius + shapes[j].data.polygon.radius;
                    if (dx*dx + dy*dy < minDist*minDist) {
                        collided = true;
                    }
                    break;
                }
                case SHAPE_ARC: {
                    float dx = enemyX - shapes[j].data.arc.x;
                    float dy = enemyY - shapes[j].data.arc.y;
                    float minDist = enemyRadius + shapes[j].data.arc.radius;
                    if (dx*dx + dy*dy < minDist*minDist) {
                        collided = true;
                    }
                    break;
                }
                case SHAPE_LINE: {
                    // Utiliser le centre de la ligne et sa longueur pour la collision
                    float lineX = (shapes[j].data.line.x1 + shapes[j].data.line.x2) / 2;
                    float lineY = (shapes[j].data.line.y1 + shapes[j].data.line.y2) / 2;
                    
                    // Utiliser isPointInLine pour une détection précise
                    if (isPointInLine(enemyX, enemyY,
                                     shapes[j].data.line.x1, shapes[j].data.line.y1,
                                     shapes[j].data.line.x2, shapes[j].data.line.y2,
                                     shapes[j].data.line.thickness + enemyRadius,
                                     shapes[j].rotation)) {
                        collided = true;
                    }
                    break;
                }
            }
            
            if (collided) {
                // Destroy both the enemy and the base
                deleteShape(j);
                if (i < game->enemyCount - 1) {
                    game->enemies[i] = game->enemies[game->enemyCount - 1];
                }
                game->enemyCount--;
                i--;  // Recheck this index since we swapped in a new enemy
                game->basesRemaining--;
                if (game->basesRemaining <= 0) {
                    game->isPlaying = false;
                    game->won = false;
                    game->winMessageTimer = 0.0f;
                    game->gameJustEnded = true;
                    game->enemyCount = 0;  // Clear all enemies when game ends
                }
                break;
            }
        }
    }
}

/**
 * @brief Render the defense game elements
 * @param renderer SDL renderer pointer
 * @param game Pointer to the current game state
 * 
 * Renders all active enemies in the defense game mode.
 */
void renderDefenseGame(SDL_Renderer* renderer, GameState* game) {
    // Render all active enemies
    for (int i = 0; i < game->enemyCount; i++) {
        if (!game->enemies[i].active) continue;
        
        // Draw enemy as a filled red circle
        renderShape(renderer, &game->enemies[i].shape);
    }
} 