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
int drawCircle(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int radius, Uint32 color, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantCircle type: %s\n", type);
    }
    else
    {
        SDL_SetRenderDrawColor(renderer, (color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
        
        if (handleEvents(renderer, texture) == -1) return -1;

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

        renderTexture(renderer, texture, 750);
        if (handleEvents(renderer, texture) == -1) return -1;
        // Rétablir la cible de rendu par défaut
        SDL_SetRenderTarget(renderer, NULL);
 
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
int drawEllipse(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int rx, int ry, Uint32 color, char *type) {
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantEllipse type: %s\n", type);
    }
    else
    {

        if (handleEvents(renderer, texture) == -1) return -1;

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
        
        renderTexture(renderer, texture, 750);
        if (handleEvents(renderer, texture) == -1) return -1;
        // Rétablir la cible de rendu par défaut
        SDL_SetRenderTarget(renderer, NULL); 
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
int drawArc(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int radius, int start_angle, int end_angle, Uint32 color) {

    if (handleEvents(renderer, texture) == -1) return -1;

    if (arcColor(renderer, x, y, radius, start_angle, end_angle, color) != 0) {
        printf("Error in arcColor: %s\n", SDL_GetError());
    }

    renderTexture(renderer, texture, 750);
    if (handleEvents(renderer, texture) == -1) return -1;
    // Rétablir la cible de rendu par défaut
    SDL_SetRenderTarget(renderer, NULL); 
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
int drawRectangle(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int w, int h, Uint32 color, char *type) {
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantRectangle type: %s\n", type);
    }
    else
    {
        // Définir la couleur de dessin
        Uint8 r = (color & 0xFF000000) >> 24; // Rouge
        Uint8 g = (color & 0x00FF0000) >> 16; // Vert
        Uint8 b = (color & 0x0000FF00) >> 8;  // Bleu
        Uint8 a = (color & 0x000000FF);       // Alpha
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        // Créer un SDL_Rect avec les dimensions fournies
        SDL_Rect rect = {x, y, w, h};

        if (handleEvents(renderer, texture) == -1) return -1;

        if (strcmp(type, "empty") == 0) {
            if (SDL_RenderDrawRect(renderer, &rect) != 0) {
                printf("Error in rectangleColor: %s\n", SDL_GetError());      
            }
        } else if (strcmp(type, "filled") == 0) {
            if (SDL_RenderFillRect(renderer, &rect) != 0) {
                printf("Error in boxColor: %s\n", SDL_GetError());                
            }
        } 
        renderTexture(renderer, texture, 750);
        if (handleEvents(renderer, texture) == -1) return -1;  
        SDL_SetRenderTarget(renderer, NULL);
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
int drawRoundedRectangle(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantRoundedRectangle type: %s\n", type);
    }
    else
    {
       
        if (handleEvents(renderer, texture) == -1) return -1;

        if (strcmp(type, "empty") == 0) {
            if (roundedRectangleColor(renderer, x1, y1, x2, y2, rad, color) != 0) {
                printf("Error in roundedRectangleColor: %s\n", SDL_GetError());
                 
            }
        } else if (strcmp(type, "filled") == 0) {
            if (roundedBoxColor(renderer, x1, y1, x2, y2, rad, color) != 0) {
                printf("Error in roundedBoxColor: %s\n", SDL_GetError());
                 
            }
        } 
        
        renderTexture(renderer, texture, 750);
        if (handleEvents(renderer, texture) == -1) return -1;
        // Rétablir la cible de rendu par défaut
        SDL_SetRenderTarget(renderer, NULL); 
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
void drawPolygon(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 *vx, Sint16 *vy, int n, Uint32 color, char *type)
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
        renderTexture(renderer, texture, 750);
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
int drawCustomPolygon(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 cx, Sint16 cy, int radius, int sides, Uint32 color, char *type) 
{
    if (sides < 3 || sides > 12) 
    {
        printf("Invalid number of sides: %d. Must be between 3 and 12.\n", sides);
    }
    else
    {

        Sint16 vx[12]; // Tableau pour les coordonnées X
        Sint16 vy[12]; // Tableau pour les coordonnées Y

        for (int i = 0; i < sides; i++) {
            if (handleEvents(renderer, texture) == -1) return -1;
            float angle = i * (2 * PI / sides); // Calculer l'angle pour chaque sommet
            vx[i] = cx + radius * cos(angle); // Coordonnée X
            vy[i] = cy + radius * sin(angle); // Coordonnée Y
        }

        // Appeler la fonction pour dessiner le polygone
        drawPolygon(renderer, texture, vx, vy, sides, color, type);
        if (handleEvents(renderer, texture) == -1) return -1;
        // Rétablir la cible de rendu par défaut
        SDL_SetRenderTarget(renderer, NULL); 
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
int drawLine(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color, Uint8 width, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantLine type: %s\n", type);
    }
    else
    {
        
        if (handleEvents(renderer, texture) == -1) return -1;

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
        
        renderTexture(renderer, texture, 750);
        if (handleEvents(renderer, texture) == -1) return -1;
        // Rétablir la cible de rendu par défaut
        SDL_SetRenderTarget(renderer, NULL);
    }
}




// Fonction pour dessiner un cercle de manière progressive 
int drawAnimatedCircle(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int radius, Uint32 color, char *type) { 

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid animatedCircle type: %s\n", type);
    }
    else
    {
        // Changer la cible de rendu pour la texture
        SDL_SetRenderTarget(renderer, texture);

        SDL_SetRenderDrawColor(renderer, (color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);

        for (int dy = -radius; dy <= radius; ++dy) 
        {
            // Gestion des événements pour permettre la fermeture
            if (handleEvents(renderer, texture) == -1) return -1;

            for (int dx = -radius; dx <= radius; ++dx) {
                // Vérifie si le point est sur le bord du cercle
                if(strcmp(type, "empty") == 0)
                {
                    int distanceSquared = dx * dx + dy * dy;
                    if (distanceSquared >= (radius - 1) * (radius - 1) && distanceSquared <= radius * radius) {
                        SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                        renderTexture(renderer, texture, 3);
                    }
                }
                else if(strcmp(type, "filled") == 0)
                { 
                    if (dx * dx + dy * dy <= radius * radius) {
                        SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                        renderTexture(renderer, texture, 0);
                    }
                }
            }
        }
    }
    // Rétablir la cible de rendu par défaut
    SDL_SetRenderTarget(renderer, NULL);

}


// Fonction pour dessiner un rectangle de manière progressive
int drawAnimatedRectangle(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int w, int h, Uint32 color, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid animatedCircle type: %s\n", type);
    }
    else
    {
        // Changer la cible de rendu pour la texture
        SDL_SetRenderTarget(renderer, texture);

        if(strcmp(type, "empty") == 0)
        {

            SDL_SetRenderDrawColor(renderer, (color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);

            // Dessiner le côté supérieur
            for (int i = 0; i < w; ++i) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x + i, y);
                renderTexture(renderer, texture, 3);
            }

            // Dessiner le côté droit
            for (int j = 0; j < h; ++j) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x + w - 1, y + j);
                renderTexture(renderer, texture, 3);
            }

            // Dessiner le côté inférieur
            for (int i = w - 1; i >= 0; --i) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x + i, y + h - 1);
                renderTexture(renderer, texture, 3);
            }

            // Dessiner le côté gauche
            for (int j = h - 1; j >= 0; --j) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x, y + j);
                renderTexture(renderer, texture, 3);
            }

        }
        else if(strcmp(type, "filled") == 0)
        {
            SDL_SetRenderDrawColor(renderer, (color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
            for (int i = 0; i < w; ++i) 
            {
                if (handleEvents(renderer, texture) == -1) return -1;

                for (int j = 0; j < h; ++j) {
                    SDL_RenderDrawPoint(renderer, x + i, y + j);
                }
                renderTexture(renderer, texture, 3);
            }
        }
        else
        {
            printf("Invalid animatedRectangle type: %s\n", type);
        }
        // Rétablir la cible de rendu par défaut
        SDL_SetRenderTarget(renderer, NULL);
    }
    
}


int drawAnimatedRoundedRectangle(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int w, int h, int radius, Uint32 color, char *type) {
    if (radius * 2 > w || radius * 2 > h) {
        printf("Invalid radius: too large for the rectangle dimensions.\n");
    }
    else if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)) {
        printf("Invalid animatedRoundedRectangle type: %s\n", type);
    }
    else
    {
        // Changer la cible de rendu pour la texture
        SDL_SetRenderTarget(renderer, texture);

        SDL_SetRenderDrawColor(renderer, (color >> 24) & 0xFF, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);

        if (strcmp(type, "empty") == 0) {
            // Dessiner les coins arrondis (arcs partiels)
            for (double theta = 0; theta <= M_PI / 2; theta += 0.01) {
                if (handleEvents(renderer, texture) == -1) return -1;

                // Coin supérieur gauche
                SDL_RenderDrawPoint(renderer, x + radius - (int)(radius * cos(theta)), y + radius - (int)(radius * sin(theta)));
                renderTexture(renderer, texture, 1);

                // Coin supérieur droit
                SDL_RenderDrawPoint(renderer, x + w - radius + (int)(radius * cos(theta)), y + radius - (int)(radius * sin(theta)));
                renderTexture(renderer, texture, 1);

                // Coin inférieur gauche
                SDL_RenderDrawPoint(renderer, x + radius - (int)(radius * cos(theta)), y + h - radius + (int)(radius * sin(theta)));
                renderTexture(renderer, texture, 1);

                // Coin inférieur droit
                SDL_RenderDrawPoint(renderer, x + w - radius + (int)(radius * cos(theta)), y + h - radius + (int)(radius * sin(theta)));
                renderTexture(renderer, texture, 1);
            }

            // Dessiner les côtés droits et gauches
            for (int j = radius; j < h - radius; ++j) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x, y + j); // Gauche
                SDL_RenderDrawPoint(renderer, x + w - 1, y + j); // Droite
                renderTexture(renderer, texture, 2);
            }

            // Dessiner les côtés supérieur et inférieur
            for (int i = radius; i < w - radius; ++i) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x + i, y); // Haut
                SDL_RenderDrawPoint(renderer, x + i, y + h - 1); // Bas
                renderTexture(renderer, texture, 4);
            }
        } 
        else if (strcmp(type, "filled") == 0) {
            // Dessiner l'intérieur du rectangle avec coins arrondis
            for (int j = 0; j < h; ++j) {
                for (int i = 0; i < w; ++i) {
                    // Vérifier si le point est à l'intérieur des coins arrondis
                    int dist_top_left = (i - radius) * (i - radius) + (j - radius) * (j - radius);
                    int dist_top_right = (i - (w - radius - 1)) * (i - (w - radius - 1)) + (j - radius) * (j - radius);
                    int dist_bottom_left = (i - radius) * (i - radius) + (j - (h - radius - 1)) * (j - (h - radius - 1));
                    int dist_bottom_right = (i - (w - radius - 1)) * (i - (w - radius - 1)) + (j - (h - radius - 1)) * (j - (h - radius - 1));

                    if ((dist_top_left > radius * radius && j < radius && i < radius) ||
                        (dist_top_right > radius * radius && j < radius && i >= w - radius) ||
                        (dist_bottom_left > radius * radius && j >= h - radius && i < radius) ||
                        (dist_bottom_right > radius * radius && j >= h - radius && i >= w - radius)) {
                        continue; // En dehors des coins arrondis
                    }

                    SDL_RenderDrawPoint(renderer, x + i, y + j);
                    
                }
                if (handleEvents(renderer, texture) == -1) return -1;
                renderTexture(renderer, texture, 6);
            }
        }
        // Rétablir la cible de rendu par défaut
        SDL_SetRenderTarget(renderer, NULL); 
    }
}


// Fonction pour dessiner une ellipse de manière progressive
int drawAnimatedEllipse(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int rx, int ry, Uint32 color, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid animatedEllipse type: %s\n", type);
    }
    else
    {
        // Changer la cible de rendu pour la texture
        SDL_SetRenderTarget(renderer, texture);

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
                if (handleEvents(renderer, texture) == -1) return -1;

                // Calculer l'angle en radians
                float angle = (i * 2 * M_PI) / steps;
                // Calculer les coordonnées du point sur le contour de l'ellipse
                int dx = (int)(rx * cos(angle));
                int dy = (int)(ry * sin(angle));
                // Dessiner un seul point sur le contour de l'ellipse
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                renderTexture(renderer, texture, 6);
            }
        } 
        else if (strcmp(type, "filled") == 0) 
        {
            const int steps = 360;
            for (int i = 0; i < steps; i++) 
            {
                if (handleEvents(renderer, texture) == -1) return -1;

                // Calculer l'angle en radians
                float angle = (i * 2 * M_PI) / steps;

                // Calculer les coordonnées du point sur le contour de l'ellipse
                int dx = (int)(rx * cos(angle));
                int dy = (int)(ry * sin(angle));
                

                // Dessiner les points à l'intérieur de l'ellipse
                for (int xOffset = -dx; xOffset <= dx; xOffset++) {
                    if (handleEvents(renderer, texture) == -1) return -1;

                    for (int yOffset = -dy; yOffset <= dy; yOffset++){
                        if ((xOffset * xOffset * ry * ry + yOffset * yOffset * rx * rx) <= (rx * rx * ry * ry)) {
                            SDL_RenderDrawPoint(renderer, x + xOffset, y + yOffset);
                        }
                    }                
                }
                renderTexture(renderer, texture, 6);
            }
        }
        // Rétablir la cible de rendu par défaut
        SDL_SetRenderTarget(renderer, NULL);  
    }
}


int drawAnimatedArc(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int radius, int start_angle, int end_angle, Uint32 color, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid animatedArc type: %s\n", type);
    }
    else
    {
        // Changer la cible de rendu pour la texture
        SDL_SetRenderTarget(renderer, texture);

        // Décomposer la couleur en RGBA
        Uint8 r = (color >> 24) & 0xFF;
        Uint8 g = (color >> 16) & 0xFF;
        Uint8 b = (color >> 8) & 0xFF;
        Uint8 a = color & 0xFF;
        
        
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        // Convertir les angles de degrés en radians
        double start_rad = start_angle * M_PI / 180.0;
        double end_rad = end_angle * M_PI / 180.0;

        // Tracer l'arc
        for (double theta = start_rad; theta <= end_rad; theta += 0.01) {
            int dx = (int)(radius * cos(theta));
            int dy = (int)(radius * sin(theta));
            
            if (handleEvents(renderer, texture) == -1) return -1;

            if (strcmp(type, "empty") == 0) {
                SDL_RenderDrawPoint(renderer, x + dx, y - dy);
            } else if (strcmp(type, "filled") == 0) {
                for (int r = radius - 2; r <= radius; ++r) { 
                    int fx = (int)(r * cos(theta));
                    int fy = (int)(r * sin(theta));
                    SDL_RenderDrawPoint(renderer, x + fx, y - fy);
                }
            }
            
            renderTexture(renderer, texture, 7);
        }
        // Rétablir la cible de rendu par défaut
        SDL_SetRenderTarget(renderer, NULL); 
    }
}


// Fonction pour dessiner un polygon de manière progressive
int drawAnimatedCustomPolygon(SDL_Renderer *renderer, SDL_Texture *texture, int cx, int cy, int radius, int sides, Uint32 color, char *type) 
{
    
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)) {
        printf("Invalid animatedPolygon type: %s\n", type);
    } 
    else if (sides < 3 || sides > 12) {
        printf("Invalid number of sides: %d. Must be between 3 and 12.\n", sides);
    }
    else {

        // Changer la cible de rendu pour la texture
        SDL_SetRenderTarget(renderer, texture);

        Sint16 vx[12]; // Tableau pour les coordonnées X
        Sint16 vy[12]; // Tableau pour les coordonnées Y

        // Calculer les sommets du polygone
        for (int i = 0; i < sides; i++) 
        {
            float angle = i * (2 * PI / sides);
            vx[i] = cx + radius * cos(angle);
            vy[i] = cy + radius * sin(angle);
        }

        // Couleur (décomposer Uint32 en RGB)
        Uint8 r = (color >> 16) & 0xFF;
        Uint8 g = (color >> 8) & 0xFF;
        Uint8 b = color & 0xFF;
        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);

        if (strcmp(type, "empty") == 0) {
            // Dessiner les contours du polygone
            for (int i = 0; i < sides; i++) 
            {
                int x1 = vx[i];
                int y1 = vy[i];
                int x2 = vx[(i + 1) % sides];
                int y2 = vy[(i + 1) % sides];

                int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
                int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1; 
                int err = dx + dy, e2;

                while (1) {
                    if (handleEvents(renderer, texture) == -1) return -1;
                    SDL_RenderDrawPoint(renderer, x1, y1);
                    renderTexture(renderer, texture, 2);

                    if (x1 == x2 && y1 == y2) break;
                    e2 = 2 * err;
                    if (e2 >= dy) { err += dy; x1 += sx; }
                    if (e2 <= dx) { err += dx; y1 += sy; }
                }
            }
        } 
        else if (strcmp(type, "filled") == 0) {
            // Remplir le polygone en traçant des lignes horizontales
            int ymin = vy[0], ymax = vy[0];
            for (int i = 1; i < sides; i++) {
                if (vy[i] < ymin) ymin = vy[i];
                if (vy[i] > ymax) ymax = vy[i];
            }

            for (int y = ymin; y <= ymax; y++) {
                int intersections[12]; // Points d'intersection pour une ligne horizontale
                int count = 0;

                // Calculer les points d'intersection entre la ligne horizontale et chaque bord
                for (int i = 0; i < sides; i++) {
                    int x1 = vx[i], y1 = vy[i];
                    int x2 = vx[(i + 1) % sides], y2 = vy[(i + 1) % sides];

                    if ((y1 <= y && y2 > y) || (y2 <= y && y1 > y)) { // Intersecte la ligne horizontale
                        int x = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
                        intersections[count++] = x;
                    }
                }

                // Trier les intersections par ordre croissant
                for (int i = 0; i < count - 1; i++) {
                    for (int j = i + 1; j < count; j++) {
                        if (intersections[i] > intersections[j]) {
                            int temp = intersections[i];
                            intersections[i] = intersections[j];
                            intersections[j] = temp;
                        }
                    }
                }

                // Dessiner des lignes entre les paires d'intersections
                for (int i = 0; i < count; i += 2) {
                    if (handleEvents(renderer, texture) == -1) return -1;
                    for (int x = intersections[i]; x <= intersections[i + 1]; x++) {
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                    renderTexture(renderer, texture, 4);
                }
            }
        }
        // Rétablir la cible de rendu par défaut
        SDL_SetRenderTarget(renderer, NULL); 
    } 
}


// Fonction pour dessiner une ligne de manière progressive
int drawAnimatedLine(SDL_Renderer *renderer, SDL_Texture *texture, int x1, int y1, int x2, int y2, Uint32 color, int thickness, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid animatedLine type: %s\n", type);
    }
    else
    {
        // Changer la cible de rendu pour la texture
        SDL_SetRenderTarget(renderer, texture);

        // Décomposer la couleur en RGBA
        Uint8 r = (color >> 24) & 0xFF;
        Uint8 g = (color >> 16) & 0xFF;
        Uint8 b = (color >> 8) & 0xFF;
        Uint8 a = color & 0xFF;
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        // Initialisation des variables pour l'algorithme de Bresenham
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;

        while (x1 != x2 || y1 != y2) {
            if (handleEvents(renderer, texture) == -1) return -1;

            // Dessiner le point actuel
            if (strcmp(type, "filled") == 0) {
                for (int tx = -thickness / 2; tx <= thickness / 2; tx++) {
                    for (int ty = -thickness / 2; ty <= thickness / 2; ty++) {
                        SDL_RenderDrawPoint(renderer, x1 + tx, y1 + ty);
                    }
                }
            } else {
                SDL_RenderDrawPoint(renderer, x1, y1);
            }

            // Afficher la progression
            renderTexture(renderer, texture, 10);

            // Mise à jour des coordonnées pour l'algorithme de Bresenham
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x1 += sx; }
            if (e2 < dx) { err += dx; y1 += sy; }
        }
        // Rétablir la cible de rendu par défaut
        SDL_SetRenderTarget(renderer, NULL); 
    }
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

        // Enregistrer la forme
        Shape newShape;
        newShape.type = SHAPE_CIRCLE;
        newShape.selected = false;
        newShape.color = color;
        newShape.rotation = 0;
        newShape.typeForm = type;
        newShape.data.circle.x = x;
        newShape.data.circle.y = y;
        newShape.data.circle.radius = radius;
        addShape(newShape);

    } 
    else if (strcmp(shape, "rectangle") == 0) {
        int x = va_arg(args, int);
        int y = va_arg(args, int);
        int w = va_arg(args, int);
        int h = va_arg(args, int);
        Uint32 color = va_arg(args, Uint32);
        char *type = va_arg(args, char*);

        if (isAnimated) {

            if(drawAnimatedRectangle(renderer, texture, x, y, w, h, color, type) == -1) return -1;

        } else {


            if(drawRectangle(renderer, texture, x, y, w, h, color, type) == -1) return -1;
        }

        // Enregistrer la forme
        Shape newShape;
        newShape.type = SHAPE_RECTANGLE;
        newShape.selected = false;
        newShape.color = color;
        newShape.rotation = 0;
        newShape.typeForm = type;
        newShape.data.rectangle.x = x;
        newShape.data.rectangle.y = y;
        newShape.data.rectangle.width = w;
        newShape.data.rectangle.height = h;
        addShape(newShape);

    } 
    else if (strcmp(shape, "arc") == 0) {
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

        // Enregistrer la forme
        Shape newShape;
        newShape.type = SHAPE_ARC;
        newShape.selected = false;
        newShape.color = color;
        newShape.rotation = 0;
        newShape.typeForm = type;
        newShape.data.arc.x = x;
        newShape.data.arc.y = y;
        newShape.data.arc.radius = radius;
        newShape.data.arc.start_angle = start_angle;
        newShape.data.arc.end_angle = end_angle;
        addShape(newShape);

    } 
    else if (strcmp(shape, "roundedRectangle") == 0) {
        
        if (isAnimated) {
            int x = va_arg(args, int);
            int y = va_arg(args, int);
            int w = va_arg(args, int);
            int h = va_arg(args, int);
            int rad = va_arg(args, int);
            Uint32 color = va_arg(args, Uint32);
            char *type = va_arg(args, char*);
            printf("%d", rad);
            
            if(drawAnimatedRoundedRectangle(renderer, texture, x, y, w, h, rad, color, type) == -1) return -1;

            // Enregistrer la forme
            Shape newShape;
            newShape.type = SHAPE_ROUNDED_RECTANGLE;
            newShape.selected = false;
            newShape.color = color;
            newShape.rotation = 0;
            newShape.typeForm = type;
            newShape.data.rounded_rectangle.x = x;
            newShape.data.rounded_rectangle.y = y;
            newShape.data.rounded_rectangle.w = w;
            newShape.data.rounded_rectangle.h = h;
            newShape.data.rounded_rectangle.rad = rad;
            addShape(newShape);

        } else {
            Sint16 x1 = va_arg(args, int);
            Sint16 y1 = va_arg(args, int);
            Sint16 x2 = va_arg(args, int);
            Sint16 y2 = va_arg(args, int);
            Sint16 radius = va_arg(args, int);
            Uint32 color = va_arg(args, Uint32);
            char *type = va_arg(args, char*);

            if(drawRoundedRectangle(renderer, texture, x1, y1, x2, y2, radius, color, type) == -1) return -1;

            // Enregistrer la forme
            Shape newShape;
            newShape.type = SHAPE_ROUNDED_RECTANGLE;
            newShape.selected = false;
            newShape.color = color;
            newShape.rotation = 0;
            newShape.typeForm = type;
            newShape.data.rounded_rectangle.x1 = x1;
            newShape.data.rounded_rectangle.y1 = y1;
            newShape.data.rounded_rectangle.x2 = x2;
            newShape.data.rounded_rectangle.y2 = y2;
            newShape.data.rounded_rectangle.radius = radius;
            addShape(newShape);

        }
    } 
    else if (strcmp(shape, "ellipse") == 0) {
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

        // Enregistrer la forme
        Shape newShape;
        newShape.type = SHAPE_ELLIPSE;
        newShape.selected = false;
        newShape.color = color;
        newShape.rotation = 0;
        newShape.typeForm = type;
        newShape.data.ellipse.x = x;
        newShape.data.ellipse.y = y;
        newShape.data.ellipse.rx = rx;
        newShape.data.ellipse.ry = ry;
        addShape(newShape);
        
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

        // Enregistrer la forme
        Shape newShape;
        newShape.type = SHAPE_LINE;
        newShape.selected = false;
        newShape.color = color;
        newShape.rotation = 0;
        newShape.typeForm = type;
        newShape.data.line.x1 = x1;
        newShape.data.line.y1 = y1;
        newShape.data.line.x2 = x2;
        newShape.data.line.y2 = y2;
        newShape.data.line.thickness = thick;
        addShape(newShape);

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

        // Enregistrer la forme
        Shape newShape;
        newShape.type = SHAPE_POLYGON;
        newShape.selected = false;
        newShape.color = color;
        newShape.rotation = 0;
        newShape.typeForm = type;
        newShape.data.polygon.cx = cx;
        newShape.data.polygon.cy = cy;
        newShape.data.polygon.radius = radius;
        newShape.data.polygon.sides = sides;
        addShape(newShape);

    } 
    else {
        printf("Invalid shape type: %s\n", shape);
    }

    // Rétablir le rendu par défaut
    SDL_SetRenderTarget(renderer, NULL);

    va_end(args);
}
