#include "../files.h/form.h"


/**
 * @brief Common parameters for functions.
 * 
 * @param renderer Pointer to the SDL renderer used for rendering.
 * @param texture Target texture for the drawing.
 * @param color Color in RGBA format (Uint32).
 * @param type Type of the shape: "filled" or "empty".
 */



/**
 * @brief Draws a circle on the SDL renderer.
 * 
 * @param x The x-coordinate of the circle's center.
 * @param y The y-coordinate of the circle's center.
 * @param radius The radius of the circle.
 */
int drawCircle(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int radius, Uint32 color, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantCircle type: %s\n", type);
    }
    else
    {
        setRenderColor(renderer, color);

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
 * @param x The x-coordinate of the ellipse's center.
 * @param y The y-coordinate of the ellipse's center.
 * @param rx The horizontal radius of the ellipse.
 * @param ry The vertical radius of the ellipse.
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
 * @param x The x-coordinate of the arc's center.
 * @param y The y-coordinate of the arc's center.
 * @param radius The radius of the arc.
 * @param start_angle The starting angle of the arc in degrees.
 * @param end_angle The ending angle of the arc in degrees.
 */
int drawArc(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int radius, int start_angle, int end_angle, Uint32 color, char *type) {

    if (handleEvents(renderer, texture) == -1) return -1;

    if (strcmp(type, "filled") == 0) {
    // Convertir les angles en radians
        double startRad = start_angle * M_PI / 180.0;
        double endRad = end_angle * M_PI / 180.0;
            // Décomposer la couleur
        Uint8 r = (color >> 24) & 0xFF;
        Uint8 g = (color >> 16) & 0xFF;
        Uint8 b = (color >> 8) & 0xFF;
        Uint8 a = color & 0xFF;

        // Définir la couleur pour le dessin
        SDL_SetRenderDrawColor(renderer, r, g, b, a);

        // Décomposer l'arc en points concentriques pour le remplir
        for (int r = 0; r <= radius; ++r) { // Itérer sur chaque rayon jusqu'à `radius`
            for (double angle = startRad; angle <= endRad; angle += 0.01) { // Itérer sur chaque angle
                int px = x + (int)(r * cos(angle));
                int py = y + (int)(r * sin(angle));
                // Dessiner une ligne du centre au bord de l'arc
                SDL_RenderDrawLine(renderer, x, y, px, py);
            }
        }
    } else if (strcmp(type, "empty") == 0) {
        // Dessiner un arc classique
        if (arcColor(renderer, x, y, radius, start_angle, end_angle, color) != 0) {
            printf("Error in arcColor: %s\n", SDL_GetError());
        }
    }

    renderTexture(renderer, texture, 750);
    if (handleEvents(renderer, texture) == -1) return -1;
    // Rétablir la cible de rendu par défaut
    SDL_SetRenderTarget(renderer, NULL); 
}


/**
 * @brief Draws a rectangle on the SDL renderer.
 * 
 * @param x The x-coordinate of the top-left corner.
 * @param y The y-coordinate of the top-left corner.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
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
 * @param x1 The x-coordinate of the top-left corner.
 * @param y1 The y-coordinate of the top-left corner.
 * @param x2 The x-coordinate of the bottom-right corner.
 * @param y2 The y-coordinate of the bottom-right corner.
 * @param rad The radius of the corners.
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
 * @param vx Array of x-coordinates for the vertices of the polygon.
 * @param vy Array of y-coordinates for the vertices of the polygon.
 * @param n The number of vertices in the polygon.
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
 * @param cx The x-coordinate of the polygon's center.
 * @param cy The y-coordinate of the polygon's center.
 * @param radius The radius from the center to the vertices.
 * @param sides The number of sides of the polygon (must be between 3 and 12).
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
 * @param x1 The x-coordinate of the starting point of the line.
 * @param y1 The y-coordinate of the starting point of the line.
 * @param x2 The x-coordinate of the ending point of the line.
 * @param y2 The y-coordinate of the ending point of the line. 
 * @param width The thickness of the line (ignored if type is "empty").
 */
int drawLine(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 width, Uint32 color, char *type) {


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




/**
 * @brief Draws an animated circle progressively.
 * 
 * @param x X-coordinate of the circle's center.
 * @param y Y-coordinate of the circle's center.
 * @param radius Radius of the circle.
 * @return -1 if an event interrupts the drawing, 0 otherwise.
 */
int drawAnimatedCircle(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int radius, Uint32 color, char *type) { 

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid animatedCircle type: %s\n", type);
    }
    else
    {
        // Changer la cible de rendu pour la texture
        SDL_SetRenderTarget(renderer, texture);

        setRenderColor(renderer, color);

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

/**
 * @brief Draws an animated rectangle progressively.
 * 
 * @param x X-coordinate of the rectangle's top-left corner.
 * @param y Y-coordinate of the rectangle's top-left corner.
 * @param w Width of the rectangle.
 * @param h Height of the rectangle.
 * @return -1 if an event interrupts the drawing, 0 otherwise.
 */
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

/**
 * @brief Draws an animated rounded rectangle progressively.
 * 
 * @param x X-coordinate of the top-left corner.
 * @param y Y-coordinate of the top-left corner.
 * @param w Width of the rounded rectangle.
 * @param h Height of the rounded rectangle.
 * @param radius Radius of the rounded corners.
 * @return -1 if an event interrupts the drawing, 0 otherwise.
 */
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


/**
 * @brief Draws an animated ellipse progressively.
 * 
 * @param x X-coordinate of the ellipse's center.
 * @param y Y-coordinate of the ellipse's center.
 * @param rx Radius along the x-axis.
 * @param ry Radius along the y-axis.
 * @return -1 if an event interrupts the drawing, 0 otherwise.
 */
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

/**
 * @brief Draws an animated arc progressively.
 * 
 * @param x X-coordinate of the arc's center.
 * @param y Y-coordinate of the arc's center.
 * @param radius Radius of the arc.
 * @param start_angle Starting angle in degrees.
 * @param end_angle Ending angle in degrees.
 * @return -1 if an event interrupts the drawing, 0 otherwise.
 */
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

        // Convertir les angles en radians
        double startRad = start_angle * M_PI / 180.0;
        double endRad = end_angle * M_PI / 180.0;

        if (strcmp(type, "empty") == 0) {
            // Dessin progressif pour un arc vide
            for (double theta = startRad; theta <= endRad; theta += 0.01) {
                int px = (int)(radius * cos(theta));
                int py = (int)(radius * sin(theta));

                SDL_RenderDrawPoint(renderer, x + px, y - py); // Dessiner le point
                renderTexture(renderer, texture, 5);          // Mettre à jour la texture
                if (handleEvents(renderer, texture) == -1) return -1; // Gérer les événements
            }
        } else if (strcmp(type, "filled") == 0) {
            // Dessin progressif pour un arc rempli
            for (int currentRadius = 0; currentRadius <= radius; ++currentRadius) { // Progresser en augmentant le rayon
                for (double angle = startRad; angle <= endRad; angle += 0.01) { // Itérer sur chaque angle
                    int px = x + (int)(currentRadius * cos(angle));
                    int py = y + (int)(currentRadius * sin(angle));

                    SDL_RenderDrawLine(renderer, x, y, px, py); // Dessiner le point correspondant
                }
                renderTexture(renderer, texture, 5);          // Mettre à jour la texture
                if (handleEvents(renderer, texture) == -1) return -1; // Gérer les événements       
            }
        }
        // Rétablir la cible de rendu par défaut
        SDL_SetRenderTarget(renderer, NULL); 
    }
}


/**
 * @brief Draws an animated custom polygon progressively.
 * 
 * @param cx X-coordinate of the polygon's center.
 * @param cy Y-coordinate of the polygon's center.
 * @param radius Radius of the polygon.
 * @param sides Number of sides of the polygon (minimum 3).
 * @return -1 if an event interrupts the drawing, 0 otherwise.
 */
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


/**
 * @brief Draws an animated line progressively.
 * 
 * @param x1 X-coordinate of the line's starting point.
 * @param y1 Y-coordinate of the line's starting point.
 * @param x2 X-coordinate of the line's ending point.
 * @param y2 Y-coordinate of the line's ending point.
 * @param thickness Thickness of the line.
 * @return -1 if an event interrupts the drawing, 0 otherwise.
 */
int drawAnimatedLine(SDL_Renderer *renderer, SDL_Texture *texture, int x1, int y1, int x2, int y2, int thickness, Uint32 color, char *type) {

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


/**
 * @brief Draws and registers a shape based on the specified parameters.
 *
 * @param shape String representing the type of shape to draw ("circle", "rectangle", "arc", etc.).
 * @param mode Drawing mode: "animated" for progressive drawing or any other value for instant drawing.
 * @param ... Variable arguments based on the shape:
 *      - "circle": int x, int y, int radius
 *      - "rectangle": int x, int y, int w, int h
 *      - "arc": int x, int y, int radius, int start_angle, int end_angle
 *      - "roundedRectangle": (animated) int x, int y, int w, int h, int radius 
 *                            (instant) Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 radius
 *      - "ellipse": int x, int y, int rx, int ry
 *      - "line": Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 thickness
 *      - "polygon": int cx, int cy, int radius, int sides
 *
 * @return void
 *
 * @details
 * This function handles the rendering and registration of different shapes.
 * It uses variadic arguments (`...`) to dynamically adapt to the parameters
 * required for each shape type. The function:
 * - Sets the target texture for rendering.
 * - Verifies the drawing mode ("animated" or instant).
 * - Calls the appropriate drawing function based on the shape type.
 * - Registers the drawn shape in a global shape list for further operations like transformation or selection.
 * - Restores the default rendering target after drawing.
 *
 * @note Each shape has specific constraints:
 * - The radius for "circle" and "arc" must be >= 5 to avoid rendering issues.
 * - Angles for "arc" must be within [0, 360].
 * - "polygon" must have a minimum of 3 sides and a reasonable number of sides for proper rendering.
 * - Ensure that "type" is either "filled" or "empty".
 */
int drawShape(SDL_Renderer *renderer, SDL_Texture *texture, char *shape, char *mode, char *type, Uint32 color, ...) {
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
        // Rayon minimum pour éviter des bugs d'affichage
        if (radius < 5) radius = 5;

        // Gestion des angles
        int start_angle = va_arg(args, int);
        if (start_angle < 0) start_angle = 0; // Minimum 0
        if (start_angle > 360) start_angle = 360; // Maximum 360

        int end_angle = va_arg(args, int);
        if (end_angle < 0) end_angle = 0; // Minimum 0
        if (end_angle > 360) end_angle = 360; // Maximum 360

        // Option : vérifier si start_angle > end_angle
        if (start_angle > end_angle) {
            int temp = start_angle; // Inverser si nécessaire
            start_angle = end_angle;
            end_angle = temp;
        }

        if (isAnimated) {
            if(drawAnimatedArc(renderer, texture, x, y, radius, start_angle, end_angle, color, type) == -1) return -1;
        } else {
            if(drawArc(renderer, texture, x, y, radius, start_angle, end_angle, color, type) == -1) return -1;
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

        if (isAnimated) {
            if(drawAnimatedLine(renderer, texture, x1, y1, x2, y2, thick, color, type) == -1) return -1;
        } else {
            if(drawLine(renderer, texture, x1, y1, x2, y2, thick, color, type) == -1) return -1;
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
