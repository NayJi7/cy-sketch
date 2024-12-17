#ifndef STRUCTFORMS_H
#define STRUCTFORMS_H

#include <SDL2/SDL.h>
#include <stdbool.h>

/*
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
extern int shapeCount;*/


typedef struct {
    int x, y;               // Position actuelle
    SDL_Color color;        // Couleur du curseur
    int thickness;          // Épaisseur du curseur
    bool visible;           // Visibilité du curseur
} Cursor;

//test 
typedef struct {
    int x, y;       // Coordonnées du centre
    int radius;     // Rayon
    SDL_Color color; // Couleur de la forme
    bool selected;  // Indique si la forme est sélectionnée
} Circle;


#endif // STRUCTFORMS_H