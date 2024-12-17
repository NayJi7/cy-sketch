#ifndef CLICKEVENTS_H
#define CLICKEVENTS_H

#include "structForms.h"
#include "main.h"

int handleEvents(SDL_Renderer* renderer, SDL_Texture* texture);

void renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, int time);

bool isCursorInCircle(const Cursor *cursor, const Circle *circle);
Cursor createCursor(int x, int y, SDL_Color color, int thickness, bool visible);
void renderCursor(SDL_Renderer *renderer, const Cursor *cursor);
void moveCursor(Cursor *cursor, int dx, int dy);


int drawShape(SDL_Renderer *renderer, SDL_Texture* texture, char *mode, char *shape, ...);

#endif // CLICKEVENTS_H

