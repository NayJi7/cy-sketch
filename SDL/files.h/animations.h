#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "main.h"
#include "formEvents.h"

// Animation functions
void applyAnimation(Shape *shape);
void animation_rotate(Shape *shape, AnimationType animation);
void animation_zoom(Shape *shape, AnimationType animation);
void animation_color(Shape *shape, AnimationType animation);
void animation_bounce(Shape *shape, AnimationType animation);
void apply_zoom_to_shape(Shape *shape, float zoom, AnimationType animation);

#endif // ANIMATIONS_H