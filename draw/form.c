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
int drawCircle(SDL_Renderer *renderer, SDL_Window* window, int x, int y, int radius, Uint32 color, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantCircle type: %s\n", type);
    }
    else
    {
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

        if (handleEvents(renderer, window) == -1) return -1;

        // Dessin
        if (strcmp(type, "empty") == 0) {
            if (circleColor(renderer, x, y, radius, color) != 0) {
                printf("Error in circleColor: %s\n", SDL_GetError());
            }

        } else if (strcmp(type, "filled") == 0) {
            if (filledCircleColor(renderer, x, y, radius, color) != 0) {
                printf("Error in filledCircleColor: %s\n", SDL_GetError());
            }
        } 
        sleep(1);
        SDL_RenderPresent(renderer);
        if (handleEvents(renderer, window) == -1) return -1;
        sleep(1);
    }
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
int drawEllipse(SDL_Renderer* renderer, SDL_Window* window, int x, int y, int rx, int ry, Uint32 color, char *type) {
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantEllipse type: %s\n", type);
    }
    else
    {
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

        if (handleEvents(renderer, window) == -1) return -1;

        // Dessin
        if (strcmp(type, "empty") == 0) {
            if (ellipseColor(renderer, x, y, rx, ry, color) != 0) {
                printf("Error in ellipseColor: %s\n", SDL_GetError());
            }
        } else if (strcmp(type, "filled") == 0) {
            if (filledEllipseColor(renderer, x, y, rx, ry, color) != 0) {
                printf("Error in filledEllipseColor: %s\n", SDL_GetError());
            }
        } 
        
        sleep(1);
        SDL_RenderPresent(renderer);
        if (handleEvents(renderer, window) == -1) return -1;
        sleep(1);
    }  
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
int drawArc(SDL_Renderer *renderer, SDL_Window* window, int x, int y, int radius, int start_angle, int end_angle, Uint32 color) {
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

    if (handleEvents(renderer, window) == -1) return -1;

    if (arcColor(renderer, x, y, radius, start_angle, end_angle, color) != 0) {
        printf("Error in arcColor: %s\n", SDL_GetError());
    }
    sleep(1);
    SDL_RenderPresent(renderer);
    if (handleEvents(renderer, window) == -1) return -1;
    sleep(1);
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
int drawRectangle(SDL_Renderer *renderer, SDL_Window* window, Sint16 x, Sint16 y, Sint16 w, Sint16 h, Uint32 color, char *type) {
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantRectangle type: %s\n", type);
    }
    else
    {
        if (handleEvents(renderer, window) == -1) return -1;

        if (strcmp(type, "empty") == 0) {
            if (rectangleColor(renderer, x, y, w, h, color) != 0) {
                printf("Error in rectangleColor: %s\n", SDL_GetError());      
            }
        } else if (strcmp(type, "filled") == 0) {
            if (boxColor(renderer, x, y, w, h, color) != 0) {
                printf("Error in boxColor: %s\n", SDL_GetError());                
            }
        } 
        SDL_RenderPresent(renderer);
        if (handleEvents(renderer, window) == -1) return -1;
        sleep(1);  
    }
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
int drawRoundedRectangle(SDL_Renderer *renderer, SDL_Window* window, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantRoundedRectangle type: %s\n", type);
    }
    else
    {
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
        
        if (handleEvents(renderer, window) == -1) return -1;

        if (strcmp(type, "empty") == 0) {
            if (roundedRectangleColor(renderer, x1, y1, x2, y2, rad, color) != 0) {
                printf("Error in roundedRectangleColor: %s\n", SDL_GetError());
                 
            }
        } else if (strcmp(type, "filled") == 0) {
            if (roundedBoxColor(renderer, x1, y1, x2, y2, rad, color) != 0) {
                printf("Error in roundedBoxColor: %s\n", SDL_GetError());
                 
            }
        } 
        SDL_Delay(4);
        if (handleEvents(renderer, window) == -1) return -1;
        SDL_RenderPresent(renderer); 
    } 
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
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantPolygon type: %s\n", type);
    }
    else
    {

        if(strcmp(type, "empty") == 0)
        {
            if(polygonColor(renderer, vx, vy, n, color) != 0)
            {
                printf("Error in polygonColor: %s\n", SDL_GetError());
                 
            }
        }
        else if(strcmp(type, "filled") == 0)
        {
            if(filledPolygonColor(renderer, vx, vy, n, color) != 0)
            {
                printf("Error in filledPolygonColor: %s\n", SDL_GetError());
                 
            }
        }
        SDL_Delay(1000);
        SDL_RenderPresent(renderer); 
    }

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
int drawCustomPolygon(SDL_Renderer *renderer, SDL_Window* window, Sint16 cx, Sint16 cy, int radius, int sides, Uint32 color, char *type) 
{
    if (sides < 3 || sides > 12) 
    {
        printf("Invalid number of sides: %d. Must be between 3 and 12.\n", sides);
    }
    else
    {
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
            if (handleEvents(renderer, window) == -1) return -1;
            float angle = i * (2 * PI / sides); // Calculer l'angle pour chaque sommet
            vx[i] = cx + radius * cos(angle); // Coordonnée X
            vy[i] = cy + radius * sin(angle); // Coordonnée Y
        }

        // Appeler la fonction pour dessiner le polygone
        drawPolygon(renderer, vx, vy, sides, color, type);
        sleep(1);
        if (handleEvents(renderer, window) == -1) return -1;
    }
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
int drawLine(SDL_Renderer *renderer, SDL_Window *window, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color, Uint8 width, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantLine type: %s\n", type);
    }
    else
    {
        SDL_RenderPresent(renderer);
        if (handleEvents(renderer, window) == -1) return -1;

        if (strcmp(type, "empty") == 0) 
        {
            if (lineColor(renderer, x1, y1, x2, y2, color) != 0) {
                printf("Error in lineColor: %s\n", SDL_GetError());
            }   
        } 
        else if (strcmp(type, "filled") == 0) 
        {
            if (thickLineColor(renderer, x1, y1, x2, y2, width, color) != 0) {
                printf("Error in thickLineColor: %s\n", SDL_GetError());
            }
        }
        
        if (handleEvents(renderer, window) == -1) return -1;
        sleep(1);
    }
}





// Fonction pour dessiner un cercle de manière progressive 
int drawAnimatedCircle(SDL_Renderer* renderer, SDL_Window *window, int x, int y, int radius, Uint32 color, char *type) { 

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid animatedCircle type: %s\n", type);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, (color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);

        for (int dy = -radius; dy <= radius; ++dy) 
        {
            // Gestion des événements pour permettre la fermeture
            if (handleEvents(renderer, window) == -1) return -1;

            for (int dx = -radius; dx <= radius; ++dx) {
                // Vérifie si le point est sur le bord du cercle
                if(strcmp(type, "empty") == 0)
                {
                    int distanceSquared = dx * dx + dy * dy;
                    if (distanceSquared >= (radius - 1) * (radius - 1) && distanceSquared <= radius * radius) {
                        SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(4); 
                    }
                }
                else if(strcmp(type, "filled") == 0)
                { 
                    if (dx * dx + dy * dy <= radius * radius) {
                        SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                        SDL_RenderPresent(renderer); // Mettre à jour l'écran pour chaque pixel
                    }
                }
            }
        }
    }
}


// Fonction pour dessiner un rectangle de manière progressive
int drawAnimatedRectangle(SDL_Renderer* renderer, SDL_Window* window, int x, int y, int w, int h, Uint32 color, char *type) {

    if(strcmp(type, "empty") == 0)
    {

        SDL_SetRenderDrawColor(renderer, (color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);

        // Dessiner le côté supérieur
        for (int i = 0; i < w; ++i) {
            if (handleEvents(renderer, window) == -1) return -1;

            SDL_RenderDrawPoint(renderer, x + i, y);
            SDL_Delay(4);
            SDL_RenderPresent(renderer);
        }

        // Dessiner le côté droit
        for (int j = 0; j < h; ++j) {
            if (handleEvents(renderer, window) == -1) return -1;

            SDL_RenderDrawPoint(renderer, x + w - 1, y + j);
            SDL_Delay(4);
            SDL_RenderPresent(renderer);
        }

        // Dessiner le côté inférieur
        for (int i = w - 1; i >= 0; --i) {
            if (handleEvents(renderer, window) == -1) return -1;

            SDL_RenderDrawPoint(renderer, x + i, y + h - 1);
            SDL_Delay(4);
            SDL_RenderPresent(renderer);
        }

        // Dessiner le côté gauche
        for (int j = h - 1; j >= 0; --j) {
            if (handleEvents(renderer, window) == -1) return -1;

            SDL_RenderDrawPoint(renderer, x, y + j);
            SDL_Delay(4);
            SDL_RenderPresent(renderer);
        }

    }
    else if(strcmp(type, "filled") == 0)
    {
        SDL_SetRenderDrawColor(renderer, (color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
        for (int i = 0; i < w; ++i) 
        {
            if (handleEvents(renderer, window) == -1) return -1;

            for (int j = 0; j < h; ++j) {
                SDL_RenderDrawPoint(renderer, x + i, y + j);
                SDL_RenderPresent(renderer); // Mettre à jour l'écran pour chaque pixel
            }
        }
    }
    else
    {
        printf("Invalid animatedRectangle type: %s\n", type);
    }
}


// Fonction pour dessiner une ellipse de manière progressive
int drawAnimatedEllipse(SDL_Renderer* renderer, SDL_Window* window, int x, int y, int rx, int ry, Uint32 color, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid animatedEllipse type: %s\n", type);
    }
    else
    {
        // Décomposer la couleur en RGBA
        Uint8 r = (color >> 24) & 0xFF;
        Uint8 g = (color >> 16) & 0xFF;
        Uint8 b = (color >> 8) & 0xFF;
        Uint8 a = color & 0xFF;
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        if (strcmp(type, "empty") == 0) 
        {
            // Paramètre pour parcourir l'ellipse avec un pas angulaire
            const int steps = 360;  // Plus de steps = plus précis, mais plus lent
            for (int i = 0; i < steps; i++) 
            {
                if (handleEvents(renderer, window) == -1) return -1;

                // Calculer l'angle en radians
                float angle = (i * 2 * M_PI) / steps;
                // Calculer les coordonnées du point sur le contour de l'ellipse
                int dx = (int)(rx * cos(angle));
                int dy = (int)(ry * sin(angle));
                // Dessiner un seul point sur le contour de l'ellipse
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                SDL_Delay(4);
                SDL_RenderPresent(renderer);
            }
        } 
        else if (strcmp(type, "filled") == 0) 
        {
            const int steps = 360;
            for (int i = 0; i < steps; i++) 
            {
                if (handleEvents(renderer, window) == -1) return -1;

                // Calculer l'angle en radians
                float angle = (i * 2 * M_PI) / steps;

                // Calculer les coordonnées du point sur le contour de l'ellipse
                int dx = (int)(rx * cos(angle));
                int dy = (int)(ry * sin(angle));
                

                // Dessiner les points à l'intérieur de l'ellipse
                for (int xOffset = -dx; xOffset <= dx; xOffset++) {
                    if (handleEvents(renderer, window) == -1) return -1;

                    for (int yOffset = -dy; yOffset <= dy; yOffset++){
                        if ((xOffset * xOffset * ry * ry + yOffset * yOffset * rx * rx) <= (rx * rx * ry * ry)) {
                            SDL_RenderDrawPoint(renderer, x + xOffset, y + yOffset);
                        }
                    }                
                }
                SDL_Delay(4);
                SDL_RenderPresent(renderer);
            }
        } 
    }
}


// Fonction pour dessiner un polygon de manière progressive
int drawAnimatedCustomPolygon(SDL_Renderer *renderer, SDL_Window* window, int cx, int cy, int radius, int sides, Uint32 color, char *type) 
{
    if (sides < 3 || sides > 12) {
        printf("Invalid number of sides: %d. Must be between 3 and 12.\n", sides);
        return -1;
    }

    Sint16 vx[12]; // Tableau pour les coordonnées X
    Sint16 vy[12]; // Tableau pour les coordonnées Y

    // Calculer les sommets du polygone
    for (int i = 0; i < sides; i++) 
    {
        if (handleEvents(renderer, window) == -1) return -1;

        float angle = i * (2 * PI / sides);
        vx[i] = cx + radius * cos(angle);
        vy[i] = cy + radius * sin(angle);
    }

    // Couleur (décomposer Uint32 en RGB)
    Uint8 r = (color >> 16) & 0xFF;
    Uint8 g = (color >> 8) & 0xFF;
    Uint8 b = color & 0xFF;

    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);

    // Dessiner chaque segment du polygone avec mise à jour pixel par pixel
    for (int i = 0; i < sides; i++) 
    {
        if (handleEvents(renderer, window) == -1) return -1;

        int x1 = vx[i];
        int y1 = vy[i];
        int x2 = vx[(i + 1) % sides];
        int y2 = vy[(i + 1) % sides];

        // Algorithme de tracé de ligne (Bresenham par exemple)
        int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
        int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1; 
        int err = dx + dy, e2; /* erreur combinée */

        while (1) {
            if (handleEvents(renderer, window) == -1) return -1;

            SDL_RenderDrawPoint(renderer, x1, y1); // Dessiner un point
            SDL_Delay(4); 
            SDL_RenderPresent(renderer); // Mettre à jour l'écran après avoir dessiné le polygone entier

            if (x1 == x2 && y1 == y2) break;
            e2 = 2 * err;
            if (e2 >= dy) { err += dy; x1 += sx; }
            if (e2 <= dx) { err += dx; y1 += sy; }
        }
    }

}


// Fonction pour dessiner une ligne de manière progressive
int drawAnimatedLine(SDL_Renderer *renderer, SDL_Window* window, int x1, int y1, int x2, int y2, Uint32 color, int thickness, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid animatedLine type: %s\n", type);
    }
    else
    {
        // Décomposer la couleur en RGBA
        Uint8 r = (color >> 24) & 0xFF;
        Uint8 g = (color >> 16) & 0xFF;
        Uint8 b = (color >> 8) & 0xFF;
        Uint8 a = color & 0xFF;
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        // Calculer les deltas pour l'algorithme de Bresenham
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;

        // Tracer la ligne pixel par pixel
        while (true) {
            if (handleEvents(renderer, window) == -1) return -1;
            // Dessiner selon le type spécifié
            if (strcmp(type, "filled") == 0) {
                // Dessiner une ligne épaisse
                for (int tx = -thickness / 2; tx <= thickness / 2; tx++) {
                    for (int ty = -thickness / 2; ty <= thickness / 2; ty++) {
                        SDL_RenderDrawPoint(renderer, x1 + tx, y1 + ty);
                    }
                }
            } else if (strcmp(type, "empty") == 0) {
                // Dessiner une ligne fine
                SDL_RenderDrawPoint(renderer, x1, y1);
            }
            SDL_RenderPresent(renderer);
            SDL_Delay(10);  // Ajout d'un délai pour l'animation

            // Vérifier si nous avons atteint la fin de la ligne
            if (x1 == x2 && y1 == y2) break;

            // Calculer la prochaine position
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x1 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y1 += sy;
            }
            
        }
    }
}


