#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct {
    int x, y;               // Current position
    SDL_Color color;        // Cursor color
    int thickness;          // Cursor thickness
    bool visible;           // Cursor visibility
} Cursor;

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

typedef enum
{
    ANIM_NONE,
    ANIM_ROTATE,
    ANIM_ZOOM
} AnimationType;

typedef struct {
    ShapeType type;       // Type of shape
    bool selected;        // Indicates whether the shape is selected
    SDL_Color color;         // Shape colour
    double rotation;      // Rotation in degrees
    char* typeForm;
    int zIndex;          // Z-index for layer ordering
    AnimationType animation;  // Type of animation
    bool isAnimating;        // Whether animation is currently active
    float zoom;          // Current zoom factor for zoom animation
    float zoom_direction;    // Direction of zoom animation (1.0 = growing, -1.0 = shrinking)
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

void mainLoop(SDL_Renderer *renderer, SDL_Event event, Cursor cursor);

#endif