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

void renderShape(SDL_Renderer *renderer, Shape *shape) {

    
    if (shape->typeForm == NULL || 
        (strcmp(shape->typeForm, "filled") != 0 && strcmp(shape->typeForm, "empty") != 0)) {
        return; // Ne rien dessiner pour ce shape
    }
    
    switch (shape->type) {  
        case SHAPE_CIRCLE:
            Uint8 rC = (shape->color >> 24) & 0xFF; // Rouge
            Uint8 gC = (shape->color >> 16) & 0xFF; // Vert
            Uint8 bC = (shape->color >> 8) & 0xFF;  // Bleu
            Uint8 aC = shape->color & 0xFF;         // Alpha

            // Appliquer la couleur par défaut pour le remplissage
            SDL_SetRenderDrawColor(renderer, rC, gC, bC, aC);

            if (strcmp(shape->typeForm, "filled") == 0) {
                filledCircleColor(renderer, shape->data.circle.x, shape->data.circle.y, shape->data.circle.radius, shape->color);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                circleColor(renderer, shape->data.circle.x, shape->data.circle.y, shape->data.circle.radius, shape->color);
            }

            if (shape->selected) {
                circleColor(renderer, shape->data.circle.x, shape->data.circle.y, shape->data.circle.radius + 5, 0xFFFFFF00);
            }

            // Dessiner une "aiguille" ou un indicateur pour montrer la rotation
            double angle1 = shape->rotation * M_PI / 180.0; // Convertir l'angle en radians
            int endX = shape->data.circle.x + cos(angle1) * shape->data.circle.radius;
            int endY = shape->data.circle.y + sin(angle1) * shape->data.circle.radius;

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Rouge pour l'aiguille
            SDL_RenderDrawLine(renderer, shape->data.circle.x, shape->data.circle.y, endX, endY);
            break;
    
        case SHAPE_RECTANGLE:

            Uint8 rR = (shape->color >> 24) & 0xFF;
            Uint8 gR = (shape->color >> 16) & 0xFF;
            Uint8 bR = (shape->color >> 8) & 0xFF;
            Uint8 aR = shape->color & 0xFF;

            SDL_SetRenderDrawColor(renderer, rR, gR, bR, aR);

            int cx = shape->data.rectangle.x + shape->data.rectangle.width / 2;
            int cy = shape->data.rectangle.y + shape->data.rectangle.height / 2;
            double angle = shape->rotation * M_PI / 180.0;

            SDL_Point points[5];
            SDL_Point selectionPoints[5];
            int offset = 5; // Espace autour pour le contour

            // Calcul des points du rectangle
            points[0].x = cos(angle) * (shape->data.rectangle.x - cx) - sin(angle) * (shape->data.rectangle.y - cy) + cx;
            points[0].y = sin(angle) * (shape->data.rectangle.x - cx) + cos(angle) * (shape->data.rectangle.y - cy) + cy;

            points[1].x = cos(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) - sin(angle) * (shape->data.rectangle.y - cy) + cx;
            points[1].y = sin(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) + cos(angle) * (shape->data.rectangle.y - cy) + cy;

            points[2].x = cos(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) - sin(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cx;
            points[2].y = sin(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) + cos(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cy;

            points[3].x = cos(angle) * (shape->data.rectangle.x - cx) - sin(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cx;
            points[3].y = sin(angle) * (shape->data.rectangle.x - cx) + cos(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cy;

            points[4] = points[0]; // Fermer le rectangle

            // Calcul des points pour le contour jaune
            for (int i = 0; i < 4; i++) {
                double dx = points[i].x - cx;
                double dy = points[i].y - cy;

                // Ajuster les points du contour
                selectionPoints[i].x = points[i].x + (dx > 0 ? offset : -offset);
                selectionPoints[i].y = points[i].y + (dy > 0 ? offset : -offset);
            }
            selectionPoints[4] = selectionPoints[0]; // Fermer le contour

            // Dessiner le rectangle
            if (strcmp(shape->typeForm, "filled") == 0) {
                filledPolygonColor(renderer, 
                                    (Sint16[]){points[0].x, points[1].x, points[2].x, points[3].x}, 
                                    (Sint16[]){points[0].y, points[1].y, points[2].y, points[3].y}, 
                                    4, shape->color);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                SDL_RenderDrawLines(renderer, points, 5);
            }

            // Dessiner le contour jaune si sélectionné
            if (shape->selected) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_RenderDrawLines(renderer, selectionPoints, 5);
            }
            break;

        case SHAPE_ELLIPSE:
            Uint8 rE = (shape->color >> 24) & 0xFF; // Rouge
            Uint8 gE = (shape->color >> 16) & 0xFF; // Vert
            Uint8 bE = (shape->color >> 8) & 0xFF;  // Bleu
            Uint8 aE = shape->color & 0xFF;         // Alpha

            SDL_SetRenderDrawColor(renderer, rE, gE, bE, aE);

            int cxE = shape->data.ellipse.x;
            int cyE = shape->data.ellipse.y;
            double angleE = shape->rotation * M_PI / 180.0; // Convertir l'angle en radians

            // Points de l'ellipse pour la rotation
            SDL_Point ellipsePoints[36]; // Points pour dessiner l'ellipse
            int pointCount = 36; // Nombre de points pour approximer l'ellipse

            for (int i = 0; i < pointCount; i++) {
                double theta = (2 * M_PI * i) / pointCount;
                double dx = shape->data.ellipse.rx * cos(theta);
                double dy = shape->data.ellipse.ry * sin(theta);
                ellipsePoints[i].x = cos(angleE) * dx - sin(angleE) * dy + cxE;
                ellipsePoints[i].y = sin(angleE) * dx + cos(angleE) * dy + cyE;
            }

            // Dessiner l'ellipse
            if (strcmp(shape->typeForm, "filled") == 0) {
                // Remplir l'ellipse avec un polygone
                Sint16 xCoords[36];
                Sint16 yCoords[36];
                for (int i = 0; i < pointCount; i++) {
                    xCoords[i] = ellipsePoints[i].x;
                    yCoords[i] = ellipsePoints[i].y;
                }
                filledPolygonColor(renderer, xCoords, yCoords, pointCount, shape->color);
            } 
            else if (strcmp(shape->typeForm, "empty") == 0) {
                // Dessiner uniquement le contour
                SDL_RenderDrawLines(renderer, ellipsePoints, pointCount);
                SDL_RenderDrawLine(renderer, ellipsePoints[pointCount - 1].x, ellipsePoints[pointCount - 1].y, ellipsePoints[0].x, ellipsePoints[0].y);
            }

            // Contour jaune si sélectionné
            if (shape->selected) {
                SDL_Point selectionPoints[36];
                for (int i = 0; i < pointCount; i++) {
                    double theta = (2 * M_PI * i) / pointCount;
                    double dx = (shape->data.ellipse.rx + 5) * cos(theta); // Augmenter les rayons
                    double dy = (shape->data.ellipse.ry + 5) * sin(theta);
                    selectionPoints[i].x = cos(angleE) * dx - sin(angleE) * dy + cxE;
                    selectionPoints[i].y = sin(angleE) * dx + cos(angleE) * dy + cyE;
                }

                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Jaune
                SDL_RenderDrawLines(renderer, selectionPoints, pointCount);
                SDL_RenderDrawLine(renderer, selectionPoints[pointCount - 1].x, selectionPoints[pointCount - 1].y, selectionPoints[0].x, selectionPoints[0].y);
            }
            break;

        case SHAPE_LINE:
            Uint8 rL = (shape->color >> 24) & 0xFF; // Rouge
            Uint8 gL = (shape->color >> 16) & 0xFF; // Vert
            Uint8 bL = (shape->color >> 8) & 0xFF;  // Bleu
            Uint8 aL = shape->color & 0xFF;         // Alpha

            SDL_SetRenderDrawColor(renderer, rL, gL, bL, aL);

            int x1 = shape->data.line.x1;
            int y1 = shape->data.line.y1;
            int x2 = shape->data.line.x2;
            int y2 = shape->data.line.y2;
            int thickness = shape->data.line.thickness;

            // Calcul des nouveaux points de la ligne avec rotation accumulée
            if (shape->rotation != 0) {
                int cx = (shape->data.line.x1 + shape->data.line.x2) / 2; // Centre de rotation
                int cy = (shape->data.line.y1 + shape->data.line.y2) / 2;
                double angle = shape->rotation * M_PI / 180.0;

                // Rotation autour du centre
                int rotatedX1 = cos(angle) * (shape->data.line.x1 - cx) - sin(angle) * (shape->data.line.y1 - cy) + cx;
                int rotatedY1 = sin(angle) * (shape->data.line.x1 - cx) + cos(angle) * (shape->data.line.y1 - cy) + cy;
                int rotatedX2 = cos(angle) * (shape->data.line.x2 - cx) - sin(angle) * (shape->data.line.y2 - cy) + cx;
                int rotatedY2 = sin(angle) * (shape->data.line.x2 - cx) + cos(angle) * (shape->data.line.y2 - cy) + cy;

                // Mettre à jour les coordonnées pour conserver la rotation accumulée
                x1 = rotatedX1;
                y1 = rotatedY1;
                x2 = rotatedX2;
                y2 = rotatedY2;
            }

            // Dessiner la ligne
            thickLineColor(renderer, x1, y1, x2, y2, thickness, shape->color);

            // Dessiner un contour jaune si sélectionné
            if (shape->selected) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Jaune
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            }
            break;



        case SHAPE_ROUNDED_RECTANGLE:
            Uint8 rRR = (shape->color >> 24) & 0xFF; // Rouge
            Uint8 gRR = (shape->color >> 16) & 0xFF; // Vert
            Uint8 bRR = (shape->color >> 8) & 0xFF;  // Bleu
            Uint8 aRR = shape->color & 0xFF;         // Alpha

            SDL_SetRenderDrawColor(renderer, rRR, gRR, bRR, aRR);

            if (strcmp(shape->typeForm, "filled") == 0) {
                // Dessiner un rectangle arrondi rempli
                roundedBoxColor(renderer,
                                shape->data.rounded_rectangle.x1,
                                shape->data.rounded_rectangle.y1,
                                shape->data.rounded_rectangle.x2,
                                shape->data.rounded_rectangle.y2,
                                shape->data.rounded_rectangle.radius,
                                shape->color);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                // Dessiner le contour d'un rectangle arrondi
                roundedRectangleColor(renderer,
                                    shape->data.rounded_rectangle.x1,
                                    shape->data.rounded_rectangle.y1,
                                    shape->data.rounded_rectangle.x2,
                                    shape->data.rounded_rectangle.y2,
                                    shape->data.rounded_rectangle.radius,
                                    shape->color);
            }

            if (shape->selected) {
                // Dessiner un contour jaune autour du rectangle arrondi
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                roundedRectangleColor(renderer,
                                    shape->data.rounded_rectangle.x1 - 5,
                                    shape->data.rounded_rectangle.y1 - 5,
                                    shape->data.rounded_rectangle.x2 + 5,
                                    shape->data.rounded_rectangle.y2 + 5,
                                    shape->data.rounded_rectangle.radius + 5,
                                    0xFFFFFF00); // Jaune
            }
            break;

            
    }
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

void zoomShape(Shape *shape, float zoomFactor) {
    switch (shape->type) {
        case SHAPE_RECTANGLE:
            shape->data.rectangle.width += (int)(zoomFactor * 10);
            shape->data.rectangle.height += (int)(zoomFactor * 10);
            break;

        case SHAPE_CIRCLE:
            shape->data.circle.radius += (int)(zoomFactor * 5);
            break;

        case SHAPE_ELLIPSE:
            shape->data.ellipse.rx += (int)(zoomFactor * 5);
            shape->data.ellipse.ry += (int)(zoomFactor * 5);
            break;

        case SHAPE_LINE:
            int cx = (shape->data.line.x1 + shape->data.line.x2) / 2;
            int cy = (shape->data.line.y1 + shape->data.line.y2) / 2;

            shape->data.line.x1 += (int)((shape->data.line.x1 - cx) * zoomFactor * 0.1);
            shape->data.line.y1 += (int)((shape->data.line.y1 - cy) * zoomFactor * 0.1);
            shape->data.line.x2 += (int)((shape->data.line.x2 - cx) * zoomFactor * 0.1);
            shape->data.line.y2 += (int)((shape->data.line.y2 - cy) * zoomFactor * 0.1);
            break;

        case SHAPE_ROUNDED_RECTANGLE:
            shape->data.rounded_rectangle.x2 += (int)(zoomFactor * 10);
            shape->data.rounded_rectangle.y2 += (int)(zoomFactor * 10);
            shape->data.rounded_rectangle.radius += (int)(zoomFactor * 2);
            break;

        default:
            // Ajouter d'autres formes si nécessaire
            break;
    }
}

void moveShapesWithMouse(Shape *shapes, int shapeCount, SDL_Event *event, Cursor *cursor) {
    for (int i = 0; i < shapeCount; i++) {
        if (shapes[i].selected) {
            switch (shapes[i].type) {
                case SHAPE_RECTANGLE:
                    shapes[i].data.rectangle.x = event->motion.x - shapes[i].data.rectangle.width / 2;
                    shapes[i].data.rectangle.y = event->motion.y - shapes[i].data.rectangle.height / 2;

                    cursor->x = shapes[i].data.rectangle.x + shapes[i].data.rectangle.width / 2;
                    cursor->y = shapes[i].data.rectangle.y + shapes[i].data.rectangle.height / 2;
                    break;

                case SHAPE_CIRCLE:
                    shapes[i].data.circle.x = event->motion.x;
                    shapes[i].data.circle.y = event->motion.y;

                    cursor->x = shapes[i].data.circle.x;
                    cursor->y = shapes[i].data.circle.y;
                    break;

                case SHAPE_ELLIPSE:
                    shapes[i].data.ellipse.x = event->motion.x;
                    shapes[i].data.ellipse.y = event->motion.y;

                    cursor->x = shapes[i].data.ellipse.x;
                    cursor->y = shapes[i].data.ellipse.y;
                    break;

                case SHAPE_LINE: {
                    int lineWidth = shapes[i].data.line.x2 - shapes[i].data.line.x1;
                    int lineHeight = shapes[i].data.line.y2 - shapes[i].data.line.y1;

                    shapes[i].data.line.x1 = event->motion.x - lineWidth / 2;
                    shapes[i].data.line.y1 = event->motion.y - lineHeight / 2;
                    shapes[i].data.line.x2 = event->motion.x + lineWidth / 2;
                    shapes[i].data.line.y2 = event->motion.y + lineHeight / 2;

                    cursor->x = event->motion.x;
                    cursor->y = event->motion.y;
                    break;
                }

                case SHAPE_ROUNDED_RECTANGLE:
                    shapes[i].data.rounded_rectangle.x1 = event->motion.x - 50;
                    shapes[i].data.rounded_rectangle.y1 = event->motion.y - 50;
                    shapes[i].data.rounded_rectangle.x2 = event->motion.x + 50;
                    shapes[i].data.rounded_rectangle.y2 = event->motion.y + 50;

                    cursor->x = (shapes[i].data.rounded_rectangle.x1 + shapes[i].data.rounded_rectangle.x2) / 2;
                    cursor->y = (shapes[i].data.rounded_rectangle.y1 + shapes[i].data.rounded_rectangle.y2) / 2;
                    break;
            }
        }
    }
}

void moveSelectedShapes(Shape *shapes, int shapeCount, int dx, int dy) {
    for (int i = 0; i < shapeCount; i++) {
        if (shapes[i].selected) {
            switch (shapes[i].type) {
                case SHAPE_RECTANGLE:
                    shapes[i].data.rectangle.x += dx;
                    shapes[i].data.rectangle.y += dy;
                    break;

                case SHAPE_CIRCLE:
                    shapes[i].data.circle.x += dx;
                    shapes[i].data.circle.y += dy;
                    break;

                case SHAPE_ELLIPSE:
                    shapes[i].data.ellipse.x += dx;
                    shapes[i].data.ellipse.y += dy;
                    break;

                case SHAPE_LINE:
                    shapes[i].data.line.x1 += dx;
                    shapes[i].data.line.y1 += dy;
                    shapes[i].data.line.x2 += dx;
                    shapes[i].data.line.y2 += dy;
                    break;

                case SHAPE_ROUNDED_RECTANGLE:
                    shapes[i].data.rounded_rectangle.x1 += dx;
                    shapes[i].data.rounded_rectangle.y1 += dy;
                    shapes[i].data.rounded_rectangle.x2 += dx;
                    shapes[i].data.rounded_rectangle.y2 += dy;
                    break;

                default:
                    break;
            }
        }
    }
}




void rotateShape(Shape *shape, int angle) {
    if (!shape->selected) return;

    shape->rotation += angle;
    if (shape->rotation >= 360) {
        shape->rotation -= 360;
    } else if (shape->rotation < 0) {
        shape->rotation += 360;
    }
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

int isPointInRoundedRectangle(Sint16 px, Sint16 py, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 radius) {
    // Vérification si le point est dans la partie rectangulaire centrale (sans les coins arrondis)
    if (px >= x1 + radius && px <= x2 - radius && py >= y1 && py <= y2) {
        return 1; // Le point est dans la zone rectangulaire centrale
    }

    if (px >= x1 && px <= x2 && py >= y1 + radius && py <= y2 - radius) {
        return 1; // Le point est dans les zones rectangulaires gauche/droite
    }


    // Vérification des coins arrondis (par rapport à leurs centres)
    // Coin supérieur gauche
    if (isPointInCircle(px, py, x1 + radius, y1 + radius, radius)) return 1;

    // Coin supérieur droit
    if (isPointInCircle(px, py, x2 - radius, y1 + radius, radius)) return 1;

    // Coin inférieur gauche
    if (isPointInCircle(px, py, x1 + radius, y2 - radius, radius)) return 1;

    // Coin inférieur droit
    if (isPointInCircle(px, py, x2 - radius, y2 - radius, radius)) return 1;

    return 0; // Le point n'est pas dans le RoundedRectangle
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

