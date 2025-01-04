#include "../files.h/animations.h"
#include <math.h>

void applyAnimation(Shape *shape){
    int check = 0;
    if (shape->animation_parser == ANIM_ROTATE) {
        for (int i = 0; i < shape->num_animations; i++) {
            if (shape->animations[i] == ANIM_ROTATE) {
               shape->animations[i] = ANIM_NONE;
               check = 1;
               shape->num_animations--;
            }
        }
        if (check == 0) {
            shape->animations[shape->num_animations] = ANIM_ROTATE;
            shape->num_animations++;
        }
    } else if (shape->animation_parser == ANIM_ZOOM) {
        for (int i = 0; i < shape->num_animations; i++) {
            if (shape->animations[i] == ANIM_ZOOM) {
               shape->animations[i] = ANIM_NONE;
               check = 1;
               shape->num_animations--;
            }
        }
        if (check == 0) {
            shape->animations[shape->num_animations] = ANIM_ZOOM;
            shape->num_animations++;
        }
    } else if (shape->animation_parser == ANIM_COLOR) {
        for (int i = 0; i < shape->num_animations; i++) {
            if (shape->animations[i] == ANIM_COLOR) {
               shape->animations[i] = ANIM_NONE;
               check = 1;
               shape->num_animations--;
            }
        }
        if (check == 0) {
            shape->animations[shape->num_animations] = ANIM_COLOR;
            shape->num_animations++;
        }
    } else if (shape->animation_parser == ANIM_BOUNCE) {
        for (int i = 0; i < shape->num_animations; i++) {
            if (shape->animations[i] == ANIM_BOUNCE) {
               shape->animations[i] = ANIM_NONE;
               check = 1;
               shape->num_animations--;
            }
        }
        if (check == 0) {
            shape->animations[shape->num_animations] = ANIM_BOUNCE;
            shape->num_animations++;
        }
    }
}

void animation_bounce(Shape *shape, AnimationType animation) {
    // DVD logo style bouncing - constant velocity with direction changes
    static const float velocity = 1.0f;  // Base velocity
    static int frameCounter = 0;  // Counter to slow down movement
    
    // Initialize velocities if not set
    if (shape->bounce_velocity == 0 && shape->bounce_direction == 0) {
        shape->bounce_velocity = velocity;  // Initial x velocity
        shape->bounce_direction = velocity;      // Initial y velocity
    }

    // Only move every 7 frames to slow down movement
    frameCounter++;
    if (frameCounter >= 7) {
        frameCounter = 0;
        // Move shape based on current velocities
        moveShape(shape, (int)shape->bounce_velocity, (int)shape->bounce_direction);
    }

    // Bounce off window boundaries
    switch (shape->type) {
        case SHAPE_CIRCLE: {
            if (shape->data.circle.x - shape->data.circle.radius <= 0 || 
                shape->data.circle.x + shape->data.circle.radius >= 800) {
                shape->bounce_velocity *= -1;  // Reverse x direction
            }
            if (shape->data.circle.y - shape->data.circle.radius <= 0 || 
                shape->data.circle.y + shape->data.circle.radius >= 600) {
                shape->bounce_direction *= -1;  // Reverse y direction
            }
            break;
        }
        case SHAPE_RECTANGLE: {
            if (shape->data.rectangle.x <= 0 || 
                shape->data.rectangle.x + shape->data.rectangle.width >= 800) {
                shape->bounce_velocity *= -1;
            }
            if (shape->data.rectangle.y <= 0 || 
                shape->data.rectangle.y + shape->data.rectangle.height >= 600) {
                shape->bounce_direction *= -1;
            }
            break;
        }
        case SHAPE_SQUARE: {
            if (shape->data.square.x <= 0 || 
                shape->data.square.x + shape->data.square.c >= 800) {
                shape->bounce_velocity *= -1;
            }
            if (shape->data.square.y <= 0 || 
                shape->data.square.y + shape->data.square.c >= 600) {
                shape->bounce_direction *= -1;
            }
            break;
        }
        case SHAPE_ELLIPSE: {
            if (shape->data.ellipse.x - shape->data.ellipse.rx <= 0 || 
                shape->data.ellipse.x + shape->data.ellipse.rx >= 800) {
                shape->bounce_velocity *= -1;
            }
            if (shape->data.ellipse.y - shape->data.ellipse.ry <= 0 || 
                shape->data.ellipse.y + shape->data.ellipse.ry >= 600) {
                shape->bounce_direction *= -1;
            }
            break;
        }
        case SHAPE_POLYGON:
        case SHAPE_TRIANGLE: {
            if (shape->data.polygon.cx - shape->data.polygon.radius <= 0 || 
                shape->data.polygon.cx + shape->data.polygon.radius >= 800) {
                shape->bounce_velocity *= -1;
            }
            if (shape->data.polygon.cy - shape->data.polygon.radius <= 0 || 
                shape->data.polygon.cy + shape->data.polygon.radius >= 600) {
                shape->bounce_direction *= -1;
            }
            break;
        }
        default:
            break;
    }
}

void animation_rotate(Shape *shape, AnimationType animation) {
    // Rotation animation (0.15 degrees per frame)
    shape->rotation += 0.15;
    if (shape->rotation >= 360.0f) {
        shape->rotation = 0.0f;
    }
}

void animation_zoom(Shape *shape, AnimationType animation) {
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
    
    apply_zoom_to_shape(shape, shape->zoom, animation);
}

void apply_zoom_to_shape(Shape *shape, float zoom, AnimationType animation) {
    // Apply zoom based on current zoom value
    switch (shape->type) {
        case SHAPE_RECTANGLE: {
            int originalWidth = 200;  // Base width
            int originalHeight = 50;  // Base height
            shape->data.rectangle.width = (int)(originalWidth * zoom);
            shape->data.rectangle.height = (int)(originalHeight * zoom);
            break;
        }

        case SHAPE_SQUARE: {
            shape->data.square.c = (int)(100 * zoom); // Base side length 100
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
        case SHAPE_TRIANGLE: {
            shape->data.triangle.radius = (int)(100 * zoom);  // Base radius 100
            break;
        }
        case SHAPE_ARC: {
            shape->data.arc.radius = (int)(100 * zoom);  // Base radius 100
            break;
        }
        case SHAPE_LINE: {
            animation = ANIM_NONE; // disabling animation for line
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

void animation_color(Shape *shape, AnimationType animation) {
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