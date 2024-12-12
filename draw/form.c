#include "../files.h/form.h"

/**
 * @brief Draws a circle on the SDL renderer.
 * 
 * @param renderer The SDL renderer where the circle will be drawn.
 * @param x The x-coordinate of the circle's center.
 * @param y The y-coordinate of the circle's center.
 * @param radius The radius of the circle.
 * @param color The color of the circle in Uint32 format (0xRRGGBBAA).
 * @param type Specifies the type of circle ("empty" for outline, "filled" for solid).
 */
void drawCircle(SDL_Renderer *renderer, int x, int y, int radius, Uint32 color, char *type) {
    // Ajout dans la liste
    if (shapeCount < MAX_SHAPES) {
        Shape newShape = {0};
        newShape.type = SHAPE_CIRCLE;
        newShape.selected = false;
        newShape.color = color;
        newShape.typeForm = malloc(10 * sizeof(char));
        newShape.data.circle.x = x;
        newShape.data.circle.y = y;
        newShape.data.circle.radius = radius;
        shapes[shapeCount++] = newShape;
    }

    // Dessin
    if (strcmp(type, "empty") == 0) {
        if (circleColor(renderer, x, y, radius, color) != 0) {
            printf("Error in circleColor: %s\n", SDL_GetError());
        }
    } else if (strcmp(type, "filled") == 0) {
        if (filledCircleColor(renderer, x, y, radius, color) != 0) {
            printf("Error in filledCircleColor: %s\n", SDL_GetError());
        }
    } else {
        printf("Invalid circle type: %s\n", type);
        return;
    }

    SDL_RenderPresent(renderer);
}


/**
 * @brief Draws an ellipse on the SDL renderer.
 * 
 * @param renderer The SDL renderer where the ellipse will be drawn.
 * @param x The x-coordinate of the ellipse's center.
 * @param y The y-coordinate of the ellipse's center.
 * @param rx The horizontal radius of the ellipse.
 * @param ry The vertical radius of the ellipse.
 * @param color The color of the ellipse in Uint32 format (0xRRGGBBAA).
 * @param type Specifies the type of ellipse ("empty" for outline, "filled" for solid).
 */
void drawEllipse(SDL_Renderer* renderer, int x, int y, int rx, int ry, Uint32 color, char *type) {
    // Ajout dans la liste
    if (shapeCount < MAX_SHAPES) {
        Shape newShape = {0};
        newShape.type = SHAPE_ELLIPSE;
        newShape.selected = false;
        newShape.color = color;
        newShape.typeForm = malloc(10 * sizeof(char));
        newShape.data.ellipse.x = x;
        newShape.data.ellipse.y = y;
        newShape.data.ellipse.rx = rx;
        newShape.data.ellipse.ry = ry;
        shapes[shapeCount++] = newShape;
    }

    // Dessin
    if (strcmp(type, "empty") == 0) {
        if (ellipseColor(renderer, x, y, rx, ry, color) != 0) {
            printf("Error in ellipseColor: %s\n", SDL_GetError());
        }
    } else if (strcmp(type, "filled") == 0) {
        if (filledEllipseColor(renderer, x, y, rx, ry, color) != 0) {
            printf("Error in filledEllipseColor: %s\n", SDL_GetError());
        }
    } else {
        printf("Invalid ellipse type: %s\n", type);
        return;
    }

    SDL_RenderPresent(renderer);
}


/**
 * @brief Draws an arc on the SDL renderer.
 * 
 * @param renderer The SDL renderer where the arc will be drawn.
 * @param x The x-coordinate of the arc's center.
 * @param y The y-coordinate of the arc's center.
 * @param radius The radius of the arc.
 * @param start_angle The starting angle of the arc in degrees.
 * @param end_angle The ending angle of the arc in degrees.
 * @param color The color of the arc in Uint32 format (0xRRGGBBAA).
 */
void drawArc(SDL_Renderer *renderer, int x, int y, int radius, int start_angle, int end_angle, Uint32 color) {
    // Ajout dans la liste
    if (shapeCount < MAX_SHAPES) {
        Shape newShape = {0};
        newShape.type = SHAPE_ARC;
        newShape.selected = false;
        newShape.color = color;
        newShape.typeForm = malloc(10 * sizeof(char));
        newShape.data.arc.x = x;
        newShape.data.arc.y = y;
        newShape.data.arc.radius = radius;
        newShape.data.arc.start_angle = start_angle;
        newShape.data.arc.end_angle = end_angle;
        shapes[shapeCount++] = newShape;
    }

    if (arcColor(renderer, x, y, radius, start_angle, end_angle, color) != 0) {
        printf("Error in arcColor: %s\n", SDL_GetError());
        cleanResources(renderer, NULL, true, false, -1);
    }
}


/**
 * @brief Draws a rectangle on the SDL renderer.
 * 
 * @param renderer The SDL renderer where the rectangle will be drawn.
 * @param x The x-coordinate of the top-left corner.
 * @param y The y-coordinate of the top-left corner.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param color The color of the rectangle in Uint32 format (0xRRGGBBAA).
 * @param type Specifies the type of rectangle ("empty" for outline, "filled" for solid).
 */
void drawRectangle(SDL_Renderer *renderer, int x, int y, int w, int h, Uint32 color, char *type) {
    // Ajout dans la liste
    if (shapeCount < MAX_SHAPES) {
        Shape newShape = {0};
        newShape.type = SHAPE_RECTANGLE;
        newShape.selected = false;
        newShape.color = color;
        newShape.typeForm = malloc(10 * sizeof(char));
        newShape.data.rectangle.x = x;
        newShape.data.rectangle.y = y;
        newShape.data.rectangle.w = w;
        newShape.data.rectangle.h = h;
        shapes[shapeCount++] = newShape;
    }

    if (strcmp(type, "empty") == 0) {
        if (rectangleColor(renderer, x, y, w, h, color) != 0) {
            printf("Error in rectangleColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    } else if (strcmp(type, "filled") == 0) {
        if (boxColor(renderer, x, y, w, h, color) != 0) {
            printf("Error in boxColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    } else {
        printf("Invalid rectangle type: %s\n", type);
        return;
    }

    SDL_Delay(1000);
    SDL_RenderPresent(renderer);  
}

/**
 * @brief Draws a rounded rectangle on the SDL renderer.
 * 
 * @param renderer The SDL renderer where the rounded rectangle will be drawn.
 * @param x1 The x-coordinate of the top-left corner.
 * @param y1 The y-coordinate of the top-left corner.
 * @param x2 The x-coordinate of the bottom-right corner.
 * @param y2 The y-coordinate of the bottom-right corner.
 * @param rad The radius of the corners.
 * @param color The color of the rounded rectangle in Uint32 format (0xRRGGBBAA).
 * @param type Specifies the type of rounded rectangle ("empty" for outline, "filled" for solid).
 */
void drawRoundedRectangle(SDL_Renderer *renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color, char *type) {
    // Ajout dans la liste
    if (shapeCount < MAX_SHAPES) {
        Shape newShape = {0};
        newShape.type = SHAPE_ROUNDEDRECTANGLE;
        newShape.selected = false;
        newShape.color = color;
        newShape.typeForm = malloc(10 * sizeof(char));
        newShape.data.roundedrectangle.x1 = x1;
        newShape.data.roundedrectangle.y1 = y1;
        newShape.data.roundedrectangle.x2 = x2;
        newShape.data.roundedrectangle.y2 = y2;
        newShape.data.roundedrectangle.rad = rad;
        shapes[shapeCount++] = newShape;
    }

    if (strcmp(type, "empty") == 0) {
        if (roundedRectangleColor(renderer, x1, y1, x2, y2, rad, color) != 0) {
            printf("Error in roundedRectangleColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    } else if (strcmp(type, "filled") == 0) {
        if (roundedBoxColor(renderer, x1, y1, x2, y2, rad, color) != 0) {
            printf("Error in roundedBoxColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    } else {
        printf("Invalid roundedRectangle type: %s\n", type);
        return;
    }

    SDL_Delay(1000);
    SDL_RenderPresent(renderer); 
}


/**
 * @brief Draws a polygon on the SDL renderer.
 * 
 * @param renderer The SDL renderer where the polygon will be drawn.
 * @param vx Array of x-coordinates for the vertices of the polygon.
 * @param vy Array of y-coordinates for the vertices of the polygon.
 * @param n The number of vertices in the polygon.
 * @param color The color of the polygon in Uint32 format (0xRRGGBBAA).
 * @param type Specifies the type of polygon ("empty" for outline, "filled" for solid).
 */
void drawPolygon(SDL_Renderer *renderer, Sint16 *vx, Sint16 *vy, int n, Uint32 color, char *type)
{

    if(strcmp(type, "empty") == 0)
    {
        if(polygonColor(renderer, vx, vy, n, color) != 0)
        {
            printf("Error in polygonColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    }
    else if(strcmp(type, "filled") == 0)
    {
        if(filledPolygonColor(renderer, vx, vy, n, color) != 0)
        {
            printf("Error in filledPolygonColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    }
    else
    {
        printf("Invalid polygon type: %s\n", type);
        return;
    }

    SDL_Delay(1000);
    SDL_RenderPresent(renderer);  

}


/**
 * @brief Draws a custom polygon with a specified number of sides, centered at (cx, cy).
 * 
 * @param renderer The SDL renderer where the custom polygon will be drawn.
 * @param cx The x-coordinate of the polygon's center.
 * @param cy The y-coordinate of the polygon's center.
 * @param radius The radius from the center to the vertices.
 * @param sides The number of sides of the polygon (must be between 3 and 12).
 * @param color The color of the polygon in Uint32 format (0xRRGGBBAA).
 * @param type Specifies the type of polygon ("empty" for outline, "filled" for solid).
 */
//FULL CHATGPT pour calculer les coordonnées X (vx) et Y (vy)
void drawCustomPolygon(SDL_Renderer *renderer, Sint16 cx, Sint16 cy, int radius, int sides, Uint32 color, char *type) 
{
    if (sides < 3 || sides > 12) 
    {
        printf("Invalid number of sides: %d. Must be between 3 and 12.\n", sides);
        return;
    }

    // Ajout dans la liste
    if (shapeCount < MAX_SHAPES) {
        Shape newShape = {0};
        newShape.type = SHAPE_POLYGON;
        newShape.selected = false;
        newShape.color = color;
        newShape.typeForm = malloc(10 * sizeof(char));
        newShape.data.polygon.cx = cx;
        newShape.data.polygon.cy = cy;
        newShape.data.polygon.radius = radius;
        newShape.data.polygon.sides = sides;
        shapes[shapeCount++] = newShape;
    }

    Sint16 vx[12]; // Tableau pour les coordonnées X
    Sint16 vy[12]; // Tableau pour les coordonnées Y

    for (int i = 0; i < sides; i++) {
        float angle = i * (2 * PI / sides); // Calculer l'angle pour chaque sommet
        vx[i] = cx + radius * cos(angle); // Coordonnée X
        vy[i] = cy + radius * sin(angle); // Coordonnée Y
    }

    // Appeler la fonction pour dessiner le polygone
    drawPolygon(renderer, vx, vy, sides, color, type);
}


/**
 * @brief Draws a line on the SDL renderer with optional thickness.
 * 
 * @param renderer The SDL renderer where the line will be drawn.
 * @param x1 The x-coordinate of the starting point of the line.
 * @param y1 The y-coordinate of the starting point of the line.
 * @param x2 The x-coordinate of the ending point of the line.
 * @param y2 The y-coordinate of the ending point of the line.
 * @param color The color of the line in Uint32 format (0xRRGGBBAA).
 * @param thickness The thickness of the line (ignored if type is "empty").
 * @param type Specifies the type of line ("empty" for thin line, "filled" for thick line).
 */
void drawLine(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, Uint32 color, int thickness, char *type) {
    // Ajout dans la liste
    if (shapeCount < MAX_SHAPES) {
        Shape newShape = {0};
        newShape.type = SHAPE_LINE;
        newShape.selected = false;
        newShape.color = color;
        newShape.typeForm = malloc(10 * sizeof(char));
        newShape.data.line.x1 = x1;
        newShape.data.line.y1 = y1;
        newShape.data.line.x2 = x2;
        newShape.data.line.y2 = y2;
        newShape.data.line.thickness = thickness;  
        shapes[shapeCount++] = newShape;
    }

    if (strcmp(type, "empty") == 0) {
        if (lineColor(renderer, x1, y1, x2, y2, color) != 0) {
            printf("Error in lineColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    } else if (strcmp(type, "filled") == 0) {
        if (thickLineColor(renderer, x1, y1, x2, y2, thickness, color) != 0) {
            printf("Error in thickLineColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    } else {
        printf("Invalid line type: %s\n", type);
        return;
    }

    SDL_Delay(1000);
    SDL_RenderPresent(renderer); 
}