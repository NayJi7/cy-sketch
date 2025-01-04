#ifndef FORMEVENTS_H
#define FORMEVENTS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdbool.h>
#include "main.h"
#include "cursorEvents.h"
#include "form.h"

// Declaration of the list of shapes and its counter
#define MAX_SHAPES 100
extern Shape shapes[MAX_SHAPES];
extern int shapeCount;

void setRenderColor(SDL_Renderer* renderer, SDL_Color color);
SDL_Color selectColor(SDL_Color color);

int renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, int time);
void renderShape(SDL_Renderer *renderer, Shape *shape);
void renderAllShapes(SDL_Renderer *renderer);
void addShape(Shape shape);
void deleteShape(int index);
void zoomShape(Shape *shape, float zoomFactor);
void rotateShape(Shape *shape, float angle);

void moveShapesWithMouse(Shape *shapes, int shapeCount, SDL_Event *event, Cursor *cursor);
void moveSelectedShapes(Shape *shapes, int shapeCount, int dx, int dy);

int isPointInCircle(int x, int y, int cx, int cy, int radius);
int isPointInEllipse(int x, int y, int cx, int cy, int rx, int ry);
int isPointInArc(int x, int y, int cx, int cy, int radius, int startAngle, int endAngle);
int isPointInRectangle(int x, int y, int rectX, int rectY, int rectW, int rectH);
int isPointInRoundedRectangle(Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Sint16 width, Sint16 height, Sint16 radius);
int isPointInPolygon(int x, int y, int cx, int cy, int radius, int sides);
int isPointInLine(int x, int y, int x1, int y1, int x2, int y2, int tolerance);

void moveShapeUp(void);
void moveShapeDown(void);
void deleteSelectedShape(void);
void toggleAnimation(void);
void resetShape(Shape *shape);
void moveShape(Shape *shape, int dx, int dy);

#endif