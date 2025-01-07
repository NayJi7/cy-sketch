#include "../files.h/animations.h"
#include <math.h>

/**
 * @brief Applies or removes an animation to/from a shape
 * 
 * This function manages the animation stack of a shape. It can:
 * - Add a new animation if it's not already present
 * - Remove an existing animation if it's already present
 * - Maintain the animation array organized (no gaps between animations)
 * 
 * The function handles up to 3 simultaneous animations per shape.
 * The animation to apply/remove is determined by shape->animation_parser.
 * 
 * @param shape Pointer to the shape whose animations are being modified
 */
void applyAnimation(Shape *shape) {
    // Check if we've already reached the maximum number of animations
    if (shape->num_animations >= 3) {
        return;  // Don't add more animations if we've reached the limit
    }

    int check = 0;
    
    // Check if the animation is already present and remove it if so
    for (int i = 0; i < 3; i++) {
        if (shape->animations[i] == shape->animation_parser) {
            check = 1;
        }
    }

    // Add the new animation if it wasn't already present
    if (check == 0) {
        shape->animations[shape->num_animations] = shape->animation_parser;
        shape->num_animations++;
    }
}

/**
 * @brief Applies or removes an animation to/from a shape
 * 
 * This function manages the animation stack of a shape. It can:
 * - Add a new animation if it's not already present
 * - Remove an existing animation if it's already present
 * - Maintain the animation array organized (no gaps between animations)
 * 
 * The function handles up to 3 simultaneous animations per shape.
 * The animation to apply/remove is determined by shape->animation_parser.
 * 
 * @param shape Pointer to the shape whose animations are being modified
 */
void unapplyAnimation(Shape *shape) {
    int check = 0;
    
    // Check if the animation is already present and remove it if so
    for (int i = 0; i < 3; i++) {
        if (shape->animations[i] == shape->animation_parser) {
            shape->animations[i] = ANIM_NONE;
            if (shape->num_animations > 0) {
                shape->num_animations--;
            }
        }
    }

    // Reorganize the array to avoid gaps between animations
    for (int i = 0; i < 2; i++) {
        if (shape->animations[i] == ANIM_NONE && shape->animations[i + 1] != ANIM_NONE) {
            shape->animations[i] = shape->animations[i + 1];
            shape->animations[i + 1] = ANIM_NONE;
        }
    }
}

/**
 * @brief Applies a bouncing animation to a shape
 * 
 * Implements a DVD logo style bouncing animation where the shape moves at a constant
 * velocity and bounces off the window boundaries. The movement is slowed down by
 * only updating every 7 frames.
 * 
 * @param shape Pointer to the shape to animate
 * @param animation The type of animation being applied
 * @param width The window width for boundary checking
 * @param height The window height for boundary checking
 */
void animation_bounce(Shape *shape, AnimationType animation, int width, int height) {
    static const float velocity = 1.0f;  // Base velocity
    static int frameCounter = 0;  // Counter to slow down movement
    
    // Initialize velocities if not set
    if (shape->bounce_velocity == 0 && shape->bounce_direction == 0) {
        shape->bounce_velocity = velocity;  // Initial x velocity
        shape->bounce_direction = velocity; // Initial y velocity
    }

    // Only move every 7 frames to slow down movement
    frameCounter++;
    if (frameCounter >= 7) {
        frameCounter = 0;
        moveShape(shape, (int)shape->bounce_velocity, (int)shape->bounce_direction);
    }

    // Check and handle boundary collisions based on shape type
    switch (shape->type) {
        case SHAPE_CIRCLE: {
            if (shape->data.circle.x - shape->data.circle.radius <= 0 || 
                shape->data.circle.x + shape->data.circle.radius >= width) {
                shape->bounce_velocity *= -1;  // Reverse x direction
            }
            if (shape->data.circle.y - shape->data.circle.radius <= 0 || 
                shape->data.circle.y + shape->data.circle.radius >= height) {
                shape->bounce_direction *= -1;  // Reverse y direction
            }
            break;
        }
        case SHAPE_RECTANGLE: {
            if (shape->data.rectangle.x <= 0 || 
                shape->data.rectangle.x + shape->data.rectangle.width >= width) {
                shape->bounce_velocity *= -1;
            }
            if (shape->data.rectangle.y <= 0 || 
                shape->data.rectangle.y + shape->data.rectangle.height >= height) {
                shape->bounce_direction *= -1;
            }
            break;
        }
        case SHAPE_SQUARE: {
            if (shape->data.square.x <= 0 || 
                shape->data.square.x + shape->data.square.c >= width) {
                shape->bounce_velocity *= -1;
            }
            if (shape->data.square.y <= 0 || 
                shape->data.square.y + shape->data.square.c >= height) {
                shape->bounce_direction *= -1;
            }
            break;
        }
        case SHAPE_ELLIPSE: {
            if (shape->data.ellipse.x - shape->data.ellipse.rx <= 0 || 
                shape->data.ellipse.x + shape->data.ellipse.rx >= width) {
                shape->bounce_velocity *= -1;
            }
            if (shape->data.ellipse.y - shape->data.ellipse.ry <= 0 || 
                shape->data.ellipse.y + shape->data.ellipse.ry >= height) {
                shape->bounce_direction *= -1;
            }
            break;
        }
        case SHAPE_POLYGON:
        case SHAPE_TRIANGLE: {
            if (shape->data.polygon.cx - shape->data.polygon.radius <= 0 || 
                shape->data.polygon.cx + shape->data.polygon.radius >= width) {
                shape->bounce_velocity *= -1;
            }
            if (shape->data.polygon.cy - shape->data.polygon.radius <= 0 || 
                shape->data.polygon.cy + shape->data.polygon.radius >= height) {
                shape->bounce_direction *= -1;
            }
            break;
        }
        default:
            break;
    }
}

/**
 * @brief Applies a rotation animation to a shape
 * 
 * Rotates the shape by 0.15 degrees per frame. The rotation wraps around
 * at 360 degrees back to 0.
 * 
 * @param shape Pointer to the shape to animate
 * @param animation The type of animation being applied
 */
void animation_rotate(Shape *shape, AnimationType animation) {
    shape->rotation += 0.15;  // Increment rotation by 0.15 degrees
    if (shape->rotation >= 360.0f) {
        shape->rotation = 0.0f;  // Wrap around at 360 degrees
    }
}

/**
 * @brief Applies a zoom animation to a shape
 * 
 * Creates a pulsing effect by scaling the shape between 50% and 150%
 * of its original size. The zoom direction reverses at these limits.
 * 
 * @param shape Pointer to the shape to animate
 * @param animation The type of animation being applied
 */
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

/**
 * @brief Applies the current zoom factor to a shape's dimensions
 * 
 * Updates the shape's dimensions based on the current zoom value.
 * Each shape type has its own base dimensions that are scaled.
 * 
 * @param shape Pointer to the shape being zoomed
 * @param zoom Current zoom factor (0.5 to 1.5)
 * @param animation The type of animation being applied
 */
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
    }
}

/**
 * @brief Applies a color cycling animation to a shape
 * 
 * Creates a rainbow effect by cycling through the HSV color space
 * and converting to RGB for display. The cycle completes when the
 * color phase reaches 1.0.
 * 
 * @param shape Pointer to the shape to animate
 * @param animation The type of animation being applied
 */
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

/**
 * @brief Updates all active animations for all shapes
 * 
 * This function handles the animation updates for all shapes that have active animations.
 * It processes each animation type (rotate, zoom, color, bounce) for each shape.
 * 
 * @param shapes Array of shapes to animate
 * @param shapeCount Number of shapes in the array
 * @param windowWidth Width of the window for bounce animation boundaries
 * @param windowHeight Height of the window for bounce animation boundaries
 */
void updateAnimations(Shape *shapes, int shapeCount, int windowWidth, int windowHeight) {
    for (int i = 0; i < shapeCount; i++) {
        if (shapes[i].isAnimating) {
            for (int j = 0; j < shapes[i].num_animations; j++) {
                switch (shapes[i].animations[j]) {
                    case ANIM_ROTATE:
                        animation_rotate(&shapes[i], shapes[i].animations[j]);
                        break;
                    case ANIM_ZOOM:
                        animation_zoom(&shapes[i], shapes[i].animations[j]);
                        break;
                    case ANIM_COLOR:
                        animation_color(&shapes[i], shapes[i].animations[j]);
                        break;
                    case ANIM_BOUNCE:
                        animation_bounce(&shapes[i], shapes[i].animations[j], windowWidth, windowHeight);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}