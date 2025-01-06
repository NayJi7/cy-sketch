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

// Game state structure
typedef struct {
    int score;
    bool isPlaying;
    bool isGameMode;  // Nouveau flag pour l'état intermédiaire
    float timeLeft;
    int initialShapeCount;
    bool won;
    bool hasShapes;
    Shape savedShapes[MAX_SHAPES];  // Pour sauvegarder les formes
    int savedShapeCount;           // Nombre de formes sauvegardées
} GameState;

// Function prototypes
void initGame(GameState* game);
void updateGame(GameState* game, float deltaTime, int cursorX, int cursorY);
void renderGameUI(SDL_Renderer* renderer, TTF_Font* font, GameState* game);

extern Shape shapes[];
extern int shapeCount;

#endif /* GAME_H */ 