#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "main.h"

// Animation functions
void animation_rotate(Shape *shape);
void animation_zoom(Shape *shape);
void animation_color(Shape *shape);
void apply_zoom_to_shape(Shape *shape, float zoom);

#endif // ANIMATIONS_H