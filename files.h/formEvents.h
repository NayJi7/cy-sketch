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
    ShapeType type;       // Type de la forme
    bool selected;        // Indique si la forme est sélectionnée
    Uint32 color;         // Couleur de la forme
    double rotation;      // Rotation en degrés
    union {
        struct { int x, y, radius; } circle;           // Données pour un cercle
        struct { int x, y, rx, ry; } ellipse;          // Données pour une ellipse
        struct { int x, y, radius, start_angle, end_angle; } arc; // Données pour un arc
        struct { int x, y, width, height; } rectangle; // Données pour un rectangle
        struct { Sint16 x1, y1, x2, y2, radius; int x, y, w, h, rad ;} rounded_rectangle; // Rectangle arrondi
        struct { int cx, cy, radius, sides; } polygon; // Données pour un polygone
        struct { Sint16 x1, y1, x2, y2; Uint8 thickness; } line; // Données pour une ligne
    } data; // Union contenant les données spécifiques à chaque forme
} Shape;

// Déclaration de la liste des formes et de son compteur
#define MAX_SHAPES 100
extern Shape shapes[MAX_SHAPES];
extern int shapeCount;


int handleEvents(SDL_Renderer* renderer, SDL_Texture* texture);
void renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, int time);
void updateShapeOnTexture(SDL_Renderer *renderer, SDL_Texture *texture, Shape *shape);

void addShape(Shape shape);
void deleteShape(int index);
void rotatePoint(int *x, int *y, int cx, int cy, double angle);
void renderShape(SDL_Renderer *renderer, Shape *shape);
int isPointInCircle(int x, int y, int cx, int cy, int radius);
int isPointInEllipse(int x, int y, int cx, int cy, int rx, int ry);
int isPointInArc(int x, int y, int cx, int cy, int radius, int startAngle, int endAngle);
int isPointInRectangle(int x, int y, int rectX, int rectY, int rectW, int rectH);
int isPointInRoundedRectangle(int px, int py, int rx, int ry, int width, int height, int radius);
int isPointInPolygon(int px, int py, int cx, int cy, int radius, int sides);
int isPointInLine(int x, int y, int x1, int y1, int x2, int y2, int tolerance);



#endif // CLICKEVENTS_H

