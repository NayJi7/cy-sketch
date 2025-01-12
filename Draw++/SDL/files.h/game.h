#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#include "formEvents.h"
#include "cursorEvents.h"
#include "form.h"
#include "animations.h"
#include "colors.h"

// Game types
typedef enum {
    GAME_ESCAPE,     // Current escape game
    GAME_DEFENSE,    // Shape defense game
    GAME_COUNT       // Number of game types
} GameType;

// Structure for enemy shapes in defense mode
typedef struct {
    Shape shape;
    float speedX;
    float speedY;
    bool active;
} EnemyShape;

// Game state structure
typedef struct {
    int score;
    bool isPlaying;
    bool isGameMode;
    float timeLeft;
    int initialShapeCount;
    bool won;
    bool hasShapes;
    float winMessageTimer;
    bool gameJustEnded;
    Shape savedShapes[MAX_SHAPES];
    int savedShapeCount;
    GameType currentGame;
    // Defense mode specific
    EnemyShape enemies[50];  // Array of enemy shapes
    int enemyCount;          // Current number of enemies
    float spawnTimer;        // Timer for enemy spawning
    int basesRemaining;      // Number of bases (original shapes) still alive
} GameState;

// Function prototypes
void initGame(GameState* game);
void updateGame(GameState* game, float deltaTime, int cursorX, int cursorY, SDL_Window* window, SDL_Renderer* renderer);
void renderGameUI(SDL_Renderer* renderer, TTF_Font* font, GameState* game, int bgR, int bgG, int bgB);
void restoreShapes(GameState* game);  // Restore shapes to original positions
void resetShapes(GameState* game, SDL_Window* window, SDL_Renderer* renderer);    // Reset shapes to random positions
void escapeRun(GameState* game, int cursorX, int cursorY);  // Run the escape game logic
const char* getGameName(GameType type);  // Get the name of the current game

// Defense game functions
void initDefenseMode(GameState* game);
void updateDefenseGame(GameState* game, float deltaTime, int cursorX, int cursorY, SDL_Window* window, SDL_Renderer* renderer);
void renderDefenseGame(SDL_Renderer* renderer, GameState* game);
void spawnEnemy(GameState* game, SDL_Window* window, SDL_Renderer* renderer);

extern Shape shapes[];
extern int shapeCount;

#endif /* GAME_H */