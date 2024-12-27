#include "../files.h/cursorEvents.h"
#include "../files.h/formEvents.h"
#include "../files.h/main.h"


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
    if (!cursor->visible) return; // Ne rien dessiner si le curseur n'est pas visible

    // Définir la couleur de la flèche
    SDL_SetRenderDrawColor(renderer, cursor->color.r, cursor->color.g, cursor->color.b, cursor->color.a);

    // Dessiner le triangle représentant la pointe du curseur
    SDL_Point triangle[4] = {
        {cursor->x, cursor->y},                                // Pointe
        {cursor->x - cursor->thickness, cursor->y + 2 * cursor->thickness}, // Coin gauche
        {cursor->x + cursor->thickness, cursor->y + 2 * cursor->thickness}, // Coin droit
        {cursor->x, cursor->y}                                 // Retour à la pointe
    };
    SDL_RenderDrawLines(renderer, triangle, 4);

    // Dessiner un point pour la base du curseur
    SDL_Rect tail = {
        cursor->x - cursor->thickness / 2,
        cursor->y + 3 * cursor->thickness,
        cursor->thickness,
        cursor->thickness
    };
    SDL_RenderFillRect(renderer, &tail);
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


int findShapeAtCursor(int x, int y) {
    for (int i = 0; i < shapeCount; i++) {
        Shape *shape = &shapes[i];

        // Vérifiez que le typeForm est valide pour éviter des formes incorrectes
        if ((strcmp(shape->typeForm, "filled") != 0) && (strcmp(shape->typeForm, "empty") != 0)) {
            continue; // Ignorer cette forme
        }

        switch (shape->type) 
        {
            case SHAPE_CIRCLE:
                if (isPointInCircle(x, y, shape->data.circle.x, shape->data.circle.y, shape->data.circle.radius)) return i;
                break;
            case SHAPE_RECTANGLE:
                if (isPointInRectangle(x, y, shape->data.rectangle.x, shape->data.rectangle.y, shape->data.rectangle.width, shape->data.rectangle.height)) return i;
                break;
            case SHAPE_ELLIPSE:
                if (isPointInEllipse(x, y, shape->data.ellipse.x, shape->data.ellipse.y, shape->data.ellipse.rx, shape->data.ellipse.ry)) return i;
                break;
            case SHAPE_ARC:
                if(isPointInArc(x, y, shape->data.arc.x, shape->data.arc.y, shape->data.arc.radius, shape->data.arc.start_angle, shape->data.arc.end_angle))return i;
                break;
            case SHAPE_ROUNDED_RECTANGLE:
                if(isPointInRoundedRectangle(x, y, shape->data.rounded_rectangle.x1, shape->data.rounded_rectangle.y1, 
                                                shape->data.rounded_rectangle.x2, 
                                                shape->data.rounded_rectangle.y2, 
                                                shape->data.rounded_rectangle.radius))return i;
                break;
            case SHAPE_POLYGON:
                if(isPointInPolygon(x, y, shape->data.polygon.cx, shape->data.polygon.cy, shape->data.polygon.radius, shape->data.polygon.sides))return i;
                break;
            case SHAPE_LINE:
                if(isPointInLine(x, y, shape->data.line.x1, shape->data.line.y1, shape->data.line.x2, shape->data.line.y2, shape->data.line.thickness))return i;        
                break;
        }
    }
    return -1; // Aucune forme trouvée
}

void handleCursorSelection(int cursorX, int cursorY) {
    int index = findShapeAtCursor(cursorX, cursorY);
    if (index != -1) {
        // Désélectionner toutes les autres formes
        for (int i = 0; i < shapeCount; i++) {
            shapes[i].selected = false;
        }
        // Sélectionner la forme trouvée
        shapes[index].selected = true;
    } else {
        // Si aucune forme n'est trouvée, désélectionner toutes les formes
        for (int i = 0; i < shapeCount; i++) {
            shapes[i].selected = false;
        }
    }
}


void handleShapeDeletion(int cursorX, int cursorY) {
    int index = findShapeAtCursor(cursorX, cursorY);
    if (index != -1) {
        deleteShape(index);
    }
}
