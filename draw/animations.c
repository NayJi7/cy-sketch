#include "../files.h/animations.h"
#include <math.h>

void animation_rotate(Shape *shape) {
    // Rotation animation (0.15 degrees per frame)
    shape->rotation += 0.15;
    if (shape->rotation >= 360.0f) {
        shape->rotation = 0.0f;
    }
}

void animation_zoom(Shape *shape) {
    // Update zoom value using shape's own direction (slower speed)
    shape->zoom += 0.0005f * shape->zoom_direction;
    
    // Check bounds and reverse direction if needed
    if (shape->zoom >= 1.5f) {
        shape->zoom = 1.5f;
        shape->zoom_direction = -1.0f;
    } else if (shape->zoom <= 0.5f) {
        shape->zoom = 0.5f;
        shape->zoom_direction = 1.0f;
    }
    
    apply_zoom_to_shape(shape, shape->zoom);
}

void apply_zoom_to_shape(Shape *shape, float zoom) {
    // Apply zoom based on current zoom value
    switch (shape->type) {
        case SHAPE_RECTANGLE: {
            int originalWidth = 200;  // Base width
            int originalHeight = 50;  // Base height
            shape->data.rectangle.width = (int)(originalWidth * zoom);
            shape->data.rectangle.height = (int)(originalHeight * zoom);
            break;
        }
        case SHAPE_CIRCLE: {
            shape->data.circle.radius = (int)(60 * zoom);  // Base radius 60
            break;
        }
        case SHAPE_ELLIPSE: {
            shape->data.ellipse.rx = (int)(70 * zoom);  // Base rx 70
            shape->data.ellipse.ry = (int)(50 * zoom);  // Base ry 50
            break;
        }
        case SHAPE_POLYGON: {
            shape->data.polygon.radius = (int)(100 * zoom);  // Base radius 100
            break;
        }
        case SHAPE_ARC: {
            shape->data.arc.radius = (int)(100 * zoom);  // Base radius 100
            break;
        }
        case SHAPE_LINE: {
            shape->animation = ANIM_NONE; // disabling animation for line
            break;
        }
        case SHAPE_ROUNDED_RECTANGLE: {
            int centerX = (shape->data.rounded_rectangle.x1 + shape->data.rounded_rectangle.x2) / 2;
            int centerY = (shape->data.rounded_rectangle.y1 + shape->data.rounded_rectangle.y2) / 2;
            int originalWidth = 200;  // Base width
            int originalHeight = 100;  // Base height
            int newWidth = (int)(originalWidth * zoom);
            int newHeight = (int)(originalHeight * zoom);
            shape->data.rounded_rectangle.x1 = centerX - newWidth/2;
            shape->data.rounded_rectangle.y1 = centerY - newHeight/2;
            shape->data.rounded_rectangle.x2 = centerX + newWidth/2;
            shape->data.rounded_rectangle.y2 = centerY + newHeight/2;
            shape->data.rounded_rectangle.radius = (int)(20 * zoom);  // Base radius 20
            break;
        }
    }
}

void animation_color(Shape *shape) {
    // Update the color phase (controls the position in the color cycle)
    shape->color_phase += 0.0004f;
    if (shape->color_phase >= 1.0f) {
        shape->color_phase = 0.0f;
    }

    // Convert HSV to RGB (hue = phase * 360, saturation = 1, value = 1)
    float hue = shape->color_phase * 360.0f;
    float saturation = 1.0f;
    float value = 1.0f;

    // HSV to RGB conversion
    float c = value * saturation;
    float x = c * (1 - fabsf(fmodf(hue / 60.0f, 2) - 1));
    float m = value - c;

    float r, g, b;
    if (hue < 60) {
        r = c; g = x; b = 0;
    } else if (hue < 120) {
        r = x; g = c; b = 0;
    } else if (hue < 180) {
        r = 0; g = c; b = x;
    } else if (hue < 240) {
        r = 0; g = x; b = c;
    } else if (hue < 300) {
        r = x; g = 0; b = c;
    } else {
        r = c; g = 0; b = x;
    }

    // Update the shape's color (convert from 0-1 range to 0-255)
    shape->color.r = (Uint8)((r + m) * 255);
    shape->color.g = (Uint8)((g + m) * 255);
    shape->color.b = (Uint8)((b + m) * 255);
}