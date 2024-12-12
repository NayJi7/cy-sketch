#ifndef STRUCTFORMS_H
#define STRUCTFORMS_H

#include "form.h"


typedef enum 
{ 
    SHAPE_CIRCLE, 
    SHAPE_ELLIPSE, 
    SHAPE_ARC, 
    SHAPE_RECTANGLE, 
    SHAPE_ROUNDEDRECTANGLE, 
    SHAPE_POLYGON, 
    SHAPE_LINE 
} ShapeType;

typedef struct 
{
    ShapeType type;      // Type de la forme (cercle, ellipse, rectangle, etc.)
    bool selected;       // Indique si la forme est sélectionnée
    Uint32 color;        // Couleur de la forme
    char *typeForm;
    union 
    {
        struct { int x, y, radius; } circle;            // Données pour un cercle
        struct { int x, y, rx, ry; } ellipse;           // Données pour une ellipse
        struct { int x, y, radius, start_angle, end_angle; } arc; // Données pour un arc
        struct { int x, y, w, h; } rectangle;           // Données pour un rectangle
        struct { Sint16 x1, y1, x2, y2, rad; } roundedrectangle; // Données pour un rectangle arrondi
        struct { Sint16 cx, cy; int radius, sides; } polygon;      // Données pour un polygone
        struct { int x1, y1, x2, y2, thickness; } line;             // Données pour une ligne
    } data;  // Union contenant les données spécifiques à chaque forme
} Shape;


// Déclaration de la liste des formes et de son compteur
#define MAX_SHAPES 100
extern Shape shapes[MAX_SHAPES];
extern int shapeCount;


#endif // STRUCTFORMS_H