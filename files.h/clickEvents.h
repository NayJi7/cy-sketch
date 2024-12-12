#ifndef CLICKEVENTS_H
#define CLICKEVENTS_H

#include "structForms.h"

int isPointInCircle(int x, int y, int cx, int cy, int radius);
int isPointInEllipse(int px, int py, int cx, int cy, int rx, int ry);
int isPointInArc(int x, int y, int cx, int cy, int radius, int startAngle, int endAngle);
int isPointInRectangle(int x, int y, int rectX, int rectY, int rectW, int rectH);
int isPointInRoundedRectangle(int px, int py, int rx, int ry, int width, int height, int radius);
int isPointInPolygon(int px, int py, int cx, int cy, int radius, int sides);
int isPointInLine(int x, int y, int x1, int y1, int x2, int y2, int tolerance);

bool selectShape(int mouseX, int mouseY);

void renderShapes(SDL_Renderer* renderer);

#endif // CLICKEVENTS_H

