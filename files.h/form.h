#ifndef FORM_H
#define FORM_H

#include "main.h"
#include "structForms.h"
#define PI 3.14159265

int drawCircle(SDL_Renderer *renderer, SDL_Window *window, int x, int y, int radius, Uint32 color, char *type);

int drawEllipse(SDL_Renderer *renderer, SDL_Window *window, int x, int y, int rx, int ry, Uint32 color, char *type);

int drawArc(SDL_Renderer *renderer, SDL_Window *window, int x, int y, int radius, int start_angle, int end_angle, Uint32 color);

int drawRectangle(SDL_Renderer *renderer, SDL_Window *window, Sint16 x, Sint16 y, Sint16 w, Sint16 h, Uint32 color, char *type);

int drawRoundedRectangle(SDL_Renderer *renderer, SDL_Window *window, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color, char *type);

void drawPolygon(SDL_Renderer *renderer, Sint16 *vx, Sint16 *vy, int n, Uint32 color, char *type);

int drawCustomPolygon(SDL_Renderer *renderer, SDL_Window *window, Sint16 cx, Sint16 cy, int radius, int sides, Uint32 color, char *type);

int drawLine(SDL_Renderer *renderer, SDL_Window *window, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color, Uint8 width, char *type);


int drawAnimatedCircle(SDL_Renderer* renderer, SDL_Window *window, int x, int y, int radius, Uint32 color, char *type);

int drawAnimatedRectangle(SDL_Renderer* renderer, SDL_Window* window, int x, int y, int w, int h, Uint32 color, char *type);

int drawAnimatedEllipse(SDL_Renderer* renderer, SDL_Window* window, int x, int y, int rx, int ry, Uint32 color, char *type);

int drawAnimatedCustomPolygon(SDL_Renderer *renderer, SDL_Window* window, int cx, int cy, int radius, int sides, Uint32 color, char *type);

int drawAnimatedLine(SDL_Renderer *renderer, SDL_Window* window, int x1, int y1, int x2, int y2, Uint32 color, int thickness, char *type);

#endif