#ifndef CLICKEVENTS_H
#define CLICKEVENTS_H

#include "cursorEvents.h"
#include "main.h"
#include "form.h"

typedef enum 
{ 
    SHAPE_CIRCLE, 
    SHAPE_ELLIPSE, 
    SHAPE_ARC, 
    SHAPE_RECTANGLE, 
    SHAPE_ROUNDED_RECTANGLE, 
    SHAPE_POLYGON, 
    SHAPE_LINE 
} ShapeType;

typedef struct {
    ShapeType type;       // Type of shape
    bool selected;        // Indicates whether the shape is selected
    Uint32 color;         // Shape colour
    double rotation;      // Rotation in degrees
    char* typeForm;
    union {
        struct { int x, y, radius; } circle;           // Data for a circle
        struct { int x, y, rx, ry; } ellipse;          // Data for an ellipse
        struct { int x, y, radius, start_angle, end_angle; } arc; // Data for an arc
        struct { int x, y, width, height;} rectangle; // Data for a rectangle
        struct { Sint16 x1, y1, x2, y2, radius; int x, y, w, h, rad ;} rounded_rectangle; // Data for a rounded Rectangle
        struct { int cx, cy, radius, sides; } polygon; // Data for a polygon
        struct { Sint16 x1, y1, x2, y2; Uint8 thickness; } line; // Data for a line
    } data; //  Union containing data specific to each form
} Shape;

// Declaration of the list of shapes and its counter
#define MAX_SHAPES 100
extern Shape shapes[MAX_SHAPES];
extern int shapeCount;

void setRenderColor(SDL_Renderer* renderer, Uint32 color);

Uint32 adjustBrightness(Uint32 color, float factor);

void renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, int time);
void renderShape(SDL_Renderer *renderer, Shape *shape);
void addShape(Shape shape);
void deleteShape(int index);
void zoomShape(Shape *shape, float zoomFactor);
void rotateShape(Shape *shape, int angle);

void moveShapesWithMouse(Shape *shapes, int shapeCount, SDL_Event *event, Cursor *cursor);
void moveSelectedShapes(Shape *shapes, int shapeCount, int dx, int dy);

int isPointInCircle(int x, int y, int cx, int cy, int radius);
int isPointInEllipse(int x, int y, int cx, int cy, int rx, int ry);
int isPointInArc(int x, int y, int cx, int cy, int radius, int startAngle, int endAngle);
int isPointInRectangle(int x, int y, int rectX, int rectY, int rectW, int rectH);
int isPointInRoundedRectangle(Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Sint16 width, Sint16 height, Sint16 radius);
int isPointInPolygon(int x, int y, int cx, int cy, int radius, int sides);
int isPointInLine(int x, int y, int x1, int y1, int x2, int y2, int tolerance);



#endif // CLICKEVENTS_H

