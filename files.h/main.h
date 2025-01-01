#ifndef MAIN_H
#define MAIN_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <unistd.h>

#include "form.h"
#include "formEvents.h"


void mainLoop(SDL_Renderer *renderer, SDL_Event event, Cursor cursor);


#endif