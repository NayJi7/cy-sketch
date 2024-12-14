#ifndef FORM_H
#define FORM_H

#include "main.h"
#include "structForms.h"
#define PI 3.14159265

void drawCircle(SDL_Renderer* renderer, int x, int y, int radius, Uint32 color, char *type);

void drawEllipse(SDL_Renderer* renderer, int x, int y, int rx, int ry, Uint32 color, char *type);

void drawArc(SDL_Renderer *renderer, int x, int y, int radius, int start_angle, int end_angle, Uint32 color);

void drawRectangle(SDL_Renderer *renderer, int x, int y, int w, int h, Uint32 color, char *type);

void drawRoundedRectangle(SDL_Renderer *renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color, char *type);

void drawPolygon(SDL_Renderer *renderer, Sint16 *vx, Sint16 *vy, int n, Uint32 color, char *type);

void drawCustomPolygon(SDL_Renderer *renderer, Sint16 cx, Sint16 cy, int radius, int sides, Uint32 color, char *type);

void drawLine(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, Uint32 color, int thickness, char *type);

void drawAnimatedCircle(SDL_Renderer* renderer, int x, int y, int radius, Uint32 color, char *type);

void drawAnimatedRectangle(SDL_Renderer* renderer, int x, int y, int w, int h, Uint32 color, char *type);

void drawAnimatedEllipse(SDL_Renderer* renderer, int x, int y, int rx, int ry, Uint32 color, char *type);

void drawAnimatedCustomPolygon(SDL_Renderer *renderer, Sint16 cx, Sint16 cy, int radius, int sides, Uint32 color, char *type);

void drawAnimatedLine(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, Uint32 color, int thickness, char *type);


#endif