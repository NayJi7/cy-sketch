#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <Python.h>  // Inclus pour utiliser Python C API

#include "form.h"

void cleanResources(SDL_Renderer *renderer, SDL_Window *window, bool freeRenderer, bool freeWindow, int exitCode);

int setWindowColor(SDL_Renderer *renderer, SDL_Color color);

#endif