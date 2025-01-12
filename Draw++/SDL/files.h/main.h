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
    SHAPE_POLYGON, 
    SHAPE_TRIANGLE,
    SHAPE_SQUARE,
    SHAPE_LINE 
} ShapeType;

typedef enum
{
    ANIM_NONE,
    ANIM_ROTATE,
    ANIM_ZOOM,
    ANIM_COLOR,
    ANIM_BOUNCE
} AnimationType;

typedef struct {
    ShapeType type;       // Type of shape
    bool selected;        // Indicates whether the shape is selected
    SDL_Color color;         // Shape colour
    SDL_Color initial_color;  // Initial color when created
    double rotation;      // Rotation in degrees
    double initial_rotation;  // Initial rotation when created
    char* typeForm;
    int zIndex;          // Z-index for layer ordering
    AnimationType animations[3];  // List of 3 animations max
    int num_animations;                       // Number of animations currently stored
    AnimationType animation_parser;                  // Current animation
    bool isAnimating;                         // Whether animations are currently active
    float zoom;                              // Current zoom factor for zoom animation
    float zoom_direction;                     // Direction of zoom animation (1.0 = growing, -1.0 = shrinking)
    float color_phase;    // Phase for color cycling animation (0.0 to 1.0)
    float bounce_velocity; // Velocity for bounce animation
    float bounce_direction; // Direction for bounce animation
    union {
        struct { 
            int x, y, radius;
            int initial_radius;  // Initial values
        } circle;
        struct { 
            int x, y, rx, ry;
            int initial_rx, initial_ry;  // Initial values
            float aspect_ratio;  // Aspect ratio of the ellipse
        } ellipse;
        struct { 
            int x, y, radius, start_angle, end_angle;
            int initial_radius, initial_start_angle, initial_end_angle;  // Initial values
        } arc;
        struct { 
            int x, y, width, height;
            int initial_width, initial_height;  // Initial values
        } rectangle;
        struct { 
            int cx, cy, radius, sides;
            int initial_radius, initial_sides;  // Initial values
        } polygon;
        struct { 
            Sint16 x1, y1, x2, y2;
            Uint8 thickness;
            Uint8 initial_thickness;  // Initial values
        } line;
        struct { 
            int cx, cy, radius;
            int initial_cx, initial_cy, initial_radius;  // Initial values
        } triangle;
        struct { 
            int x, y, c;
            int initial_x, initial_y, initial_c;  // Initial values
        } square;
    } data;
} Shape;

#endif