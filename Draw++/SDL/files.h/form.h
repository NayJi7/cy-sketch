#ifndef FORM_H
#define FORM_H

#include "main.h"
#include "cursorEvents.h"
#include "formEvents.h"
#define PI 3.14159265

int drawCircle(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int radius, SDL_Color color, char *type);

int drawEllipse(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int rx, int ry, SDL_Color color, char *type);

int drawArc(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int radius, int start_angle, int end_angle, SDL_Color color, char *type);

int drawRectangle(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int w, int h, SDL_Color color, char *type);

int drawRoundedRectangle(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, SDL_Color color, char *type);

int drawPolygon(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 *vx, Sint16 *vy, int n, SDL_Color color, char *type);

int drawCustomPolygon(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 cx, Sint16 cy, int radius, int sides, SDL_Color color, char *type);

int drawLine(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 width, SDL_Color color, char *type);

int drawTriangle(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 cx, Sint16 cy, int radius, SDL_Color color, char *type);

int drawAnimatedTriangle(SDL_Renderer *renderer, SDL_Texture *texture, int cx, int cy, int radius, SDL_Color color, char *type);

int drawAnimatedCircle(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int radius, SDL_Color color, char *type);

int drawAnimatedRectangle(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int w, int h, SDL_Color color, char *type);

int drawAnimatedRoundedRectangle(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int w, int h, int radius, SDL_Color color, char *type);

int drawAnimatedEllipse(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int rx, int ry, SDL_Color color, char *type);

int drawAnimatedArc(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int radius, int start_angle, int end_angle, SDL_Color color, char *type);

int drawAnimatedCustomPolygon(SDL_Renderer *renderer, SDL_Texture *texture, int cx, int cy, int radius, int sides, SDL_Color color, char *type);

int drawAnimatedLine(SDL_Renderer *renderer, SDL_Texture *texture, int x1, int y1, int x2, int y2, int thickness, SDL_Color color, char *type);


int drawShape(SDL_Renderer *renderer, SDL_Texture *texture, char *shape, char *mode, char *type, SDL_Color color, ...);

#endif