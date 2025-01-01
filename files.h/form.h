#ifndef FORM_H
#define FORM_H

#include "main.h"
#include "cursorEvents.h"
#include "formEvents.h"
#define PI 3.14159265

#define red          0xFF0000FF  
//#define green        0x00FF00FF  
#define blue         0x0000FFFF  
#define white        0xFFFFFFFF  
#define black        0x000000FF  
#define yellow       0xFFFF00FF  
#define cyan         0x00FFFFFF  
#define magenta      0xFF00FFFF  
#define gray         0x808080FF   
#define light_gray   0xD3D3D3FF   
#define dark_gray    0xA9A9A9FF   
#define orange       0xFFA500FF  
//#define purple       0x800080FF  
#define brown        0xA52A2AFF  
#define pink         0xFFC0CBFF

int drawCircle(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int radius, Uint32 color, char *type);

int drawEllipse(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int rx, int ry, Uint32 color, char *type);

int drawArc(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int radius, int start_angle, int end_angle, Uint32 color, char *type);

int drawRectangle(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int w, int h, Uint32 color, char *type);

int drawRoundedRectangle(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color, char *type);

void drawPolygon(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 *vx, Sint16 *vy, int n, Uint32 color, char *type);

int drawCustomPolygon(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 cx, Sint16 cy, int radius, int sides, Uint32 color, char *type);

int drawLine(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 width, Uint32 color, char *type);


int drawAnimatedCircle(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int radius, Uint32 color, char *type);

int drawAnimatedRectangle(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int w, int h, Uint32 color, char *type);

int drawAnimatedRoundedRectangle(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int w, int h, int radius, Uint32 color, char *type);

int drawAnimatedEllipse(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int rx, int ry, Uint32 color, char *type);

int drawAnimatedArc(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int radius, int start_angle, int end_angle, Uint32 color, char *type);

int drawAnimatedCustomPolygon(SDL_Renderer *renderer, SDL_Texture *texture, int cx, int cy, int radius, int sides, Uint32 color, char *type);

int drawAnimatedLine(SDL_Renderer *renderer, SDL_Texture *texture, int x1, int y1, int x2, int y2, int thickness, Uint32 color, char *type);


int drawShape(SDL_Renderer *renderer, SDL_Texture *texture, char *shape, char *mode, char *type, Uint32 color, ...);

#endif