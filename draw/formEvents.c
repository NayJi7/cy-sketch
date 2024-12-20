#include "../files.h/formEvents.h"
#include "../files.h/cursorEvents.h"

#include <math.h>

// Tableau global pour stocker les formes
Shape shapes[MAX_SHAPES];
int shapeCount = 0;

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
void updateShapeOnTexture(SDL_Renderer *renderer, SDL_Texture *texture, Shape *shape) {
    // Définir la texture comme cible de rendu
    SDL_SetRenderTarget(renderer, texture);

    // Effacer la région autour de la forme pour éviter des artefacts
    SDL_Rect clearRect;
    switch (shape->type) {
        case SHAPE_CIRCLE:
            clearRect.x = shape->data.circle.x - shape->data.circle.radius - 10;
            clearRect.y = shape->data.circle.y - shape->data.circle.radius - 10;
            clearRect.w = (shape->data.circle.radius * 2) + 20;
            clearRect.h = (shape->data.circle.radius * 2) + 20;
            break;
        case SHAPE_RECTANGLE:
            clearRect.x = shape->data.rectangle.x - 10;
            clearRect.y = shape->data.rectangle.y - 10;
            clearRect.w = shape->data.rectangle.width + 20;
            clearRect.h = shape->data.rectangle.height + 20;
            break;
    }

    // Effacer la zone concernée sur la texture
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Couleur de fond (noir)
    SDL_RenderFillRect(renderer, &clearRect);

    // Redessiner la forme avec ses nouvelles coordonnées
    switch (shape->type) {
        case SHAPE_CIRCLE:
            drawShape(renderer, texture, "instant", "circle", 
                      shape->data.circle.x, shape->data.circle.y, 
                      shape->data.circle.radius, shape->color, "filled");
            break;
        case SHAPE_RECTANGLE:
            drawShape(renderer, texture, "instant", "rectangle", 
                      shape->data.rectangle.x, shape->data.rectangle.y, 
                      shape->data.rectangle.width, shape->data.rectangle.height, 
                      shape->color, "filled");
            break;
    }

    // Rétablir la cible de rendu par défaut
    SDL_SetRenderTarget(renderer, NULL);
}

void addShape(Shape shape) {
    if (shapeCount >= MAX_SHAPES) {
        printf("Erreur : Nombre maximal de formes atteint.\n");
        return;
    }
    shapes[shapeCount++] = shape;
}
void deleteShape(int index) {
    if (index < 0 || index >= shapeCount) return;
    for (int i = index; i < shapeCount - 1; i++) {
        shapes[i] = shapes[i + 1];
    }
    shapeCount--;
}
void rotatePoint(int *x, int *y, int cx, int cy, double angle) {
    double rad = angle * M_PI / 180.0;
    int newX = cos(rad) * (*x - cx) - sin(rad) * (*y - cy) + cx;
    int newY = sin(rad) * (*x - cx) + cos(rad) * (*y - cy) + cy;
    *x = newX;
    *y = newY;
}


// Vérifie si un point (x, y) est à l'intérieur d'un cercle
int isPointInCircle(int x, int y, int cx, int cy, int radius) {
    // Calculer la distance entre le point (x, y) et le centre du cercle (cx, cy)
    int dx = x - cx;
    int dy = y - cy;
    // Si la distance est inférieure ou égale au rayon, le point est à l'intérieur
    return (dx * dx + dy * dy <= radius * radius);
}
// Vérifie si un point (x, y) est à l'intérieur d'une ellipse
int isPointInEllipse(int x, int y, int cx, int cy, int rx, int ry) {
    // Vérifie si le point respecte l'équation de l'ellipse (x-cx)^2 / rx^2 + (y-cy)^2 / ry^2 <= 1
    return ((x - cx) * (x - cx)) / (rx * rx) + ((y - cy) * (y - cy)) / (ry * ry) <= 1;
}
// Vérifie si un point (x, y) est à l'intérieur d'un arc
int isPointInArc(int x, int y, int cx, int cy, int radius, int startAngle, int endAngle) {
    // Calculer la distance entre le point et le centre
    int dx = x - cx;
    int dy = y - cy;
    int distanceSquared = dx * dx + dy * dy;
    
    // Vérifier si le point est dans le rayon
    if (distanceSquared > radius * radius) {
        return 0; // Point trop loin
    }
    
    // Calculer l'angle du point par rapport au centre (cx, cy)
    double angle = atan2(dy, dx) * 180.0 / M_PI; // en degrés
    
    // Assurez-vous que l'angle soit positif (dans le cas où atan2 donne des angles négatifs)
    if (angle < 0) {
        angle += 360;
    }

    // Vérifier si l'angle est dans la plage de l'arc
    return (angle >= startAngle && angle <= endAngle);
}
// Vérifie si un point (x, y) est à l'intérieur d'un rectangle
int isPointInRectangle(int x, int y, int rectX, int rectY, int rectW, int rectH) {
    // Vérifie si le point est dans les limites du rectangle
    return (x >= rectX && x <= rectX + rectW && y >= rectY && y <= rectY + rectH);
}
// Vérifie si un point (px, py) est à l'intérieur d'un rectangle arrondi
int isPointInRoundedRectangle(int px, int py, int rx, int ry, int width, int height, int radius) {
    // Vérification si le point est dans la partie du rectangle sans arrondi
    if (px >= rx && px <= rx + width && py >= ry && py <= ry + height) {
        return 1; // Le point est dans le rectangle principal
    }
    
    // Vérification des coins arrondis : en vérifiant si le point est à l'intérieur de chaque arc de cercle
    // Coin supérieur gauche
    if (isPointInCircle(px, py, rx + radius, ry + radius, radius)) return 1;
    // Coin supérieur droit
    if (isPointInCircle(px, py, rx + width - radius, ry + radius, radius)) return 1;
    // Coin inférieur gauche
    if (isPointInCircle(px, py, rx + radius, ry + height - radius, radius)) return 1;
    // Coin inférieur droit
    if (isPointInCircle(px, py, rx + width - radius, ry + height - radius, radius)) return 1;
    
    return 0; // Le point n'est pas dans le rectangle arrondi
}
int isPointInPolygon(int px, int py, int cx, int cy, int radius, int sides) {
    int i, j, c = 0;
    float angle, x1, y1, x2, y2;
    
    // Vérifier si le nombre de côtés est valide
    if (sides < 3) return 0;

    // Calculer les coordonnées des sommets du polygone
    for (i = 0, j = sides - 1; i < sides; j = i++) {
        // Calcul des coordonnées des sommets
        angle = (2 * M_PI * i) / sides; // Angle pour le i-ème sommet
        x1 = cx + radius * cos(angle);
        y1 = cy + radius * sin(angle);

        angle = (2 * M_PI * j) / sides; // Angle pour le j-ème sommet
        x2 = cx + radius * cos(angle);
        y2 = cy + radius * sin(angle);

        // Vérification si le point (px, py) se trouve dans la zone du polygone
        if (((y1 > py) != (y2 > py)) &&
            (px < (x2 - x1) * (py - y1) / (y2 - y1) + x1)) {
            c = !c;
        }
    }

    return c; // Retourne 1 si le point est à l'intérieur, sinon 0
}
// Vérifie si un point (x, y) est à proximité d'une ligne définie par deux points (x1, y1) et (x2, y2)
int isPointInLine(int x, int y, int x1, int y1, int x2, int y2, int tolerance) {
    // Calculer la distance du point à la ligne
    float lineLength = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    // Convertir les entiers en flottants pour utiliser fabs correctement
    float distance = fabs((float)(y2 - y1) * x - (float)(x2 - x1) * y + (float)(x2 * y1) - (float)(y2 * x1)) / lineLength;
    
    // Si la distance est inférieure à la tolérance, le point est proche de la ligne
    return (distance <= tolerance);
}

