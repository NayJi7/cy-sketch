#ifndef STRUCTFORMS_H
#define STRUCTFORMS_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct {
    int x, y;               // Position actuelle
    SDL_Color color;        // Couleur du curseur
    int thickness;          // Épaisseur du curseur
    bool visible;           // Visibilité du curseur
} Cursor;


typedef struct {
    int x, y;       // Coordonnées du centre
    int radius;     // Rayon
    SDL_Color color; // Couleur de la forme
    bool selected;  // Indique si la forme est sélectionnée
} Circle;


int handleEvents(SDL_Renderer* renderer, SDL_Texture* texture);

Cursor createCursor(int x, int y, SDL_Color color, int thickness, bool visible);

void renderCursor(SDL_Renderer *renderer, const Cursor *cursor);

void moveCursor(Cursor *cursor, int dx, int dy);

int findShapeAtCursor(int x, int y);

void handleCursorSelection(int cursorX, int cursorY);

void handleShapeDeletion(int cursorX, int cursorY);

#endif // STRUCTFORMS_H