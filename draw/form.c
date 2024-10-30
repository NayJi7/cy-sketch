#include "../files.h/form.h"


void drawCircle(SDL_Renderer* renderer, int x, int y, int radius, Uint32 color, char *type)
{   

    if(strcmp(type, "empty") == 0) 
    {
        if(circleColor(renderer, x, y, radius, color) != 0) 
        {
            printf("Error circleColor : %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    }
    else if(strcmp(type, "filled") == 0) 
    {
        if(filledCircleColor(renderer, x, y, radius, color) != 0) 
        {
            printf("Error filledCircleColor : %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    }
    else 
    {
        printf("Invalid circle type: %s\n", type);
        return;  
    }

    // Pause of 1s to see the result
    SDL_Delay(1000);    
    //Display the drawing
    SDL_RenderPresent(renderer);

}


void drawEllipse(SDL_Renderer* renderer, int x, int y, int rx, int ry, Uint32 color, char *type) 
{

    if(strcmp(type, "empty") == 0) 
    {
        // Dessiner une ellipse vide
        if(ellipseColor(renderer, x, y, rx, ry, color) != 0) 
        {
            printf("Error in ellipseColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    }
    else if(strcmp(type, "filled") == 0) 
    {
        if(filledEllipseColor(renderer, x, y, rx, ry, color) != 0) 
        {
            printf("Error in filledEllipseColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    }  
    else
    {
        printf("Invalid ellipse type: %s\n", type);
        return;
    }

    SDL_Delay(1000);
    SDL_RenderPresent(renderer);   

}


void drawRectangle(SDL_Renderer *renderer, int x, int y, int w, int h, Uint32 color, char *type)
{

    if(strcmp(type, "empty") == 0)
    {
        if(rectangleColor(renderer, x, y, w, h, color) != 0)
        {
            printf("Error in rectangleColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    }
    else if(strcmp(type, "filled") == 0)
    {
        if(boxColor(renderer, x, y, w, h, color) != 0)
        {
            printf("Error in boxColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    }
    else
    {
        printf("Invalid rectangle type: %s\n", type);
        return;
    }

    SDL_Delay(1000);
    SDL_RenderPresent(renderer);  

}

void drawRoundedRectangle(SDL_Renderer *renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color, char *type)
{

    if(strcmp(type, "empty") == 0)
    {
        if(roundedRectangleColor(renderer, x1, y1, x2, y2, rad, color) != 0)
        {
            printf("Error in roundedRectangleColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    }
    else if(strcmp(type, "filled") == 0)
    {
        if(roundedBoxColor(renderer, x1, y1, x2, y2, rad, color) != 0)
        {
            printf("Error in roundedBoxColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    }
    else
    {
        printf("Invalid roundedRectangle type: %s\n", type);
        return;
    }

    SDL_Delay(1000);
    SDL_RenderPresent(renderer); 

}


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

//FULL CHATGPT pour calculer les coordonnées X (vx) et Y (vy)
void drawCustomPolygon(SDL_Renderer *renderer, Sint16 cx, Sint16 cy, int radius, int sides, Uint32 color, char *type) 
{
    if (sides < 3 || sides > 12) 
    {
        printf("Invalid number of sides: %d. Must be between 3 and 12.\n", sides);
        return;
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



void drawLine(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, Uint32 color, int thickness, char *type) 
{

    if(strcmp(type, "empty") == 0)
    {
        if(lineColor(renderer, x1, y1, x2, y2, color) != 0) 
        {
            printf("Error in lineColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    } 
    else if(strcmp(type, "filled") == 0) 
    {
        if(thickLineColor(renderer, x1, y1, x2, y2, thickness, color) != 0) 
        {
            printf("Error in thickLineColor: %s\n", SDL_GetError());
            cleanResources(renderer, NULL, true, false, -1);
        }
    } 
    else 
    {
        printf("Invalid line type: %s\n", type);
        return;
    }

    SDL_Delay(1000);
    SDL_RenderPresent(renderer); 

}
