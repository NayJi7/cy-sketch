#ifndef CURSOREVENTS_H
#define CURSOREVENTS_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <SDL2/SDL_ttf.h>
#include "main.h"

typedef struct {
    int x, y;       // Centre contact details
    int radius;     // Radius
    SDL_Color color; // Form color
    bool selected;  // Indicates whether the shape is selected
} Circle;

int handleEvents(SDL_Renderer* renderer, SDL_Texture* texture);
Cursor createCursor(int x, int y, SDL_Color color, int thickness, bool visible);
void renderCursor(SDL_Renderer *renderer, const Cursor *cursor);
void moveCursor(Cursor *cursor, int dx, int dy);
int findShapeAtCursor(int x, int y);
void handleCursorSelection(int cursorX, int cursorY);
void renderCursorCoordinates(SDL_Renderer *renderer, TTF_Font *font, int x, int y);
char* getAnimationName(AnimationType animation);
void renderShapeInfo(SDL_Renderer *renderer, TTF_Font *font, Shape *shape);
void renderLastKeyPressed(SDL_Renderer *renderer, TTF_Font *font);
void handleShapeDeletion(int cursorX, int cursorY);
void cleanup(SDL_Texture* texture, SDL_Renderer* renderer, SDL_Window* window);

extern char lastKeyPressed[32];  // Buffer to store the last pressed key

#endif // CURSOREVENTS_H
