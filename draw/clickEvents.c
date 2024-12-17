#include "../files.h/clickEvents.h"
#include "../files.h/structForms.h"

#include <math.h>

// Fonction pour gérer les événements
int handleEvents(SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyTexture(texture);
            SDL_Quit();
            return -1;
        }
    }
    return 0;
}

void renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, int time) {
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    if(time != 0) SDL_Delay(time);
    SDL_RenderPresent(renderer);
    SDL_SetRenderTarget(renderer, texture);
}


bool isCursorInCircle(const Cursor *cursor, const Circle *circle) {
    int dx = cursor->x - circle->x;
    int dy = cursor->y - circle->y;
    int distanceSquared = dx * dx + dy * dy;
    return distanceSquared <= (circle->radius * circle->radius);
}

/**
 * @brief Initializes a cursor with default properties.
 * 
 * @param x Initial x-coordinate.
 * @param y Initial y-coordinate.
 * @param color Initial color of the cursor.
 * @param thickness Line thickness for the cursor.
 * @param visible Visibility of the cursor.
 * 
 * @return A new instance of Cursor.
 */
Cursor createCursor(int x, int y, SDL_Color color, int thickness, bool visible) {
    Cursor cursor;
    cursor.x = x;
    cursor.y = y;
    cursor.color = color;
    cursor.thickness = thickness;
    cursor.visible = visible;
    return cursor;
}

/**
 * @brief Renders the cursor as an arrow with a smaller point below the triangle.
 * 
 * @param renderer The SDL renderer used for drawing.
 * @param cursor The cursor to render.
 */
void renderCursor(SDL_Renderer *renderer, const Cursor *cursor) {
    if (!cursor->visible) return;

    // Définir la couleur de la flèche
    SDL_SetRenderDrawColor(renderer, cursor->color.r, cursor->color.g, cursor->color.b, cursor->color.a);

    // Points du triangle
    SDL_Point triangle[4] = {
        {cursor->x, cursor->y},                                // Pointe de la flèche
        {cursor->x - cursor->thickness, cursor->y + 2 * cursor->thickness}, // Coin gauche de la base
        {cursor->x + cursor->thickness, cursor->y + 2 * cursor->thickness}, // Coin droit de la base
        {cursor->x, cursor->y}                                 // Retour à la pointe
    };

    // Dessiner le triangle
    SDL_RenderDrawLines(renderer, triangle, 4);

    // Dessiner un point plus petit sous le triangle (queue de la flèche)
    int pointSize = cursor->thickness / 2; // Réduire la taille du point
    SDL_Rect tailPoint = {
        cursor->x - pointSize / 2,             // Centrer le point horizontalement
        cursor->y + 3 * cursor->thickness,    // Positionner le point sous le triangle
        pointSize,                            // Largeur du point
        pointSize                             // Hauteur du point
    };
    SDL_RenderFillRect(renderer, &tailPoint);
}

/**
 * @brief Moves the cursor by a relative offset.
 * 
 * @param cursor Pointer to the cursor to move.
 * @param dx Horizontal offset in pixels.
 * @param dy Vertical offset in pixels.
 */
void moveCursor(Cursor *cursor, int dx, int dy) {
    cursor->x += dx;
    cursor->y += dy;
}


int drawShape(SDL_Renderer *renderer, SDL_Texture *texture, char *mode, char *shape, ...) {
    va_list args;
    va_start(args, shape);

    // Définir la texture comme cible temporaire
    SDL_SetRenderTarget(renderer, texture);


    // Vérification du mode
    int isAnimated = (strcmp(mode, "animated") == 0);

    // Sélection du type de forme
    if (strcmp(shape, "circle") == 0) {
        int x = va_arg(args, int);
        int y = va_arg(args, int);
        int radius = va_arg(args, int);
        Uint32 color = va_arg(args, Uint32);
        char *type = va_arg(args, char*);

        if (isAnimated) {
            if(drawAnimatedCircle(renderer, texture, x, y, radius, color, type) == -1) return -1;
        } else {
            if(drawCircle(renderer, texture, x, y, radius, color, type) == -1) return -1;
        }
    } else if (strcmp(shape, "rectangle") == 0) {
        Sint16 x = va_arg(args, int);
        Sint16 y = va_arg(args, int);
        Sint16 w = va_arg(args, int);
        Sint16 h = va_arg(args, int);
        Uint32 color = va_arg(args, Uint32);
        char *type = va_arg(args, char*);

        if (isAnimated) {
            if(drawAnimatedRectangle(renderer, texture, x, y, w, h, color, type) == -1) return -1;
        } else {
            if(drawRectangle(renderer, texture, x, y, w, h, color, type) == -1) return -1;
        }
    } else if (strcmp(shape, "arc") == 0) {
        int x = va_arg(args, int);
        int y = va_arg(args, int);
        int radius = va_arg(args, int);
        int start_angle = va_arg(args, int);
        int end_angle = va_arg(args, int);
        Uint32 color = va_arg(args, Uint32);
        char *type = va_arg(args, char*);

        if (isAnimated) {
            if(drawAnimatedArc(renderer, texture, x, y, radius, start_angle, end_angle, color, type) == -1) return -1;
        } else {
            if(drawArc(renderer, texture, x, y, radius, start_angle, end_angle, color) == -1) return -1;
        }

    } else if (strcmp(shape, "roundedRectangle") == 0) {
        if (isAnimated) {
            int x = va_arg(args, int);
            int y = va_arg(args, int);
            int w = va_arg(args, int);
            int h = va_arg(args, int);
            int radius = va_arg(args, int);
            Uint32 color = va_arg(args, Uint32);
            char *type = va_arg(args, char*);

            if(drawAnimatedRoundedRectangle(renderer, texture, x, y, w, h, radius, color, type) == -1) return -1;
        } else {
            Sint16 x1 = va_arg(args, int);
            Sint16 y1 = va_arg(args, int);
            Sint16 x2 = va_arg(args, int);
            Sint16 y2 = va_arg(args, int);
            Sint16 radius = va_arg(args, int);
            Uint32 color = va_arg(args, Uint32);
            char *type = va_arg(args, char*);

            if(drawRoundedRectangle(renderer, texture, x1, y1, x2, y2, radius, color, type) == -1) return -1;
        }

    } else if (strcmp(shape, "ellipse") == 0) {
        int x = va_arg(args, int);
        int y = va_arg(args, int);
        int rx = va_arg(args, int);
        int ry = va_arg(args, int);
        Uint32 color = va_arg(args, Uint32);
        char *type = va_arg(args, char*);

        if (isAnimated) {
            if(drawAnimatedEllipse(renderer, texture, x, y, rx, ry, color, type) == -1) return -1;
        } else {
            if(drawEllipse(renderer, texture, x, y, rx, ry, color, type) == -1) return -1;
        }
    } 
    else if (strcmp(shape, "line") == 0) {
        Sint16 x1 = va_arg(args, int);
        Sint16 y1 = va_arg(args, int);
        Sint16 x2 = va_arg(args, int);
        Sint16 y2 = va_arg(args, int);
        Uint8 thick = va_arg(args, int);
        Uint32 color = va_arg(args, Uint32);
        char *type = va_arg(args, char*);

        if (isAnimated) {
            if(drawAnimatedLine(renderer, texture, x1, y1, x2, y2, color, thick, type) == -1) return -1;
        } else {
            if(drawLine(renderer, texture, x1, y1, x2, y2, color, thick, type) == -1) return -1;
        }
    } 
    else if (strcmp(shape, "polygon") == 0) {
        int cx = va_arg(args, int);
        int cy = va_arg(args, int);
        int radius = va_arg(args, int);
        int sides = va_arg(args, int);
        Uint32 color = va_arg(args, Uint32);
        char *type = va_arg(args, char*);

        if (isAnimated) {
            if(drawAnimatedCustomPolygon(renderer, texture, cx, cy, radius, sides, color, type) == -1) return -1;
        } else {
            if(drawCustomPolygon(renderer, texture, cx, cy, radius, sides, color, type) == -1) return -1;
        }
    } 
    else {
        printf("Invalid shape type: %s\n", shape);
    }

    // Rétablir le rendu par défaut
    SDL_SetRenderTarget(renderer, NULL);

    va_end(args);
}
