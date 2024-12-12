#include "../files.h/clickEvents.h"

#include <math.h>

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




bool selectShape(int mouseX, int mouseY) {
    bool shapeSelected = false;  // Une variable pour suivre si une forme a été sélectionnée

    for (int i = 0; i < shapeCount; i++) {
        Shape *s = &shapes[i];

        switch (s->type) {
            case SHAPE_CIRCLE:
                if (isPointInCircle(mouseX, mouseY, s->data.circle.x, s->data.circle.y, s->data.circle.radius)) {
                    s->selected = true;
                    shapeSelected = true;  // Indiquer qu'une forme a été sélectionnée
                } else {
                    s->selected = false;
                }
                break;
            case SHAPE_ELLIPSE:
                if (isPointInEllipse(mouseX, mouseY, s->data.ellipse.x, s->data.ellipse.y, s->data.ellipse.rx, s->data.ellipse.ry)) {
                    s->selected = true;
                    shapeSelected = true;
                } else {
                    s->selected = false;
                }
                break;
            case SHAPE_RECTANGLE:
                if (isPointInRectangle(mouseX, mouseY, s->data.rectangle.x, s->data.rectangle.y, s->data.rectangle.w, s->data.rectangle.h)) {
                    s->selected = true;
                    shapeSelected = true;
                } else {
                    s->selected = false;
                }
                break;
            case SHAPE_LINE:
                if (isPointInLine(mouseX, mouseY, s->data.line.x1, s->data.line.y1, s->data.line.x2, s->data.line.y2, s->data.line.thickness)) {
                    s->selected = true;
                    shapeSelected = true;
                } else {
                    s->selected = false;
                }
                break;
            case SHAPE_POLYGON:
                if (isPointInPolygon(mouseX, mouseY, s->data.polygon.cx, s->data.polygon.cy, s->data.polygon.radius, s->data.polygon.sides)) {
                    s->selected = true;
                    shapeSelected = true;
                } else {
                    s->selected = false;
                }
                break;
            case SHAPE_ARC:
                if (isPointInArc(mouseX, mouseY, s->data.arc.x, s->data.arc.y, s->data.arc.radius, s->data.arc.start_angle, s->data.arc.end_angle)) {
                    s->selected = true;
                    shapeSelected = true;
                } else {
                    s->selected = false;
                }
                break;
            case SHAPE_ROUNDEDRECTANGLE:
                if (isPointInRoundedRectangle(mouseX, mouseY, s->data.roundedrectangle.x1, s->data.roundedrectangle.y1, s->data.roundedrectangle.x2, s->data.roundedrectangle.y2, s->data.roundedrectangle.rad)) {
                    s->selected = true;
                    shapeSelected = true;
                } else {
                    s->selected = false;
                }
                break;
            default:
                break;
        }
    }

    return shapeSelected;  // Retourner true si une forme a été sélectionnée, sinon false
}


void renderShapes(SDL_Renderer* renderer) {
    for (int i = 0; i < shapeCount; i++) {
        Shape *s = &shapes[i];

        switch (s->type) {
            case SHAPE_CIRCLE:
                if (s->selected) {
                    // Dessiner un contour rouge si sélectionné
                    drawCircle(renderer, s->data.circle.x, s->data.circle.y, s->data.circle.radius + 2, 0xFF0000FF, s->typeForm);
                }
                drawCircle(renderer, s->data.circle.x, s->data.circle.y, s->data.circle.radius, s->color, s->typeForm);
                break;
            case SHAPE_ELLIPSE:
                if (s->selected) {
                    // Dessiner un contour rouge si sélectionné
                    drawEllipse(renderer, s->data.ellipse.x, s->data.ellipse.y, s->data.ellipse.rx + 2, s->data.ellipse.ry + 2, 0xFF0000FF, s->typeForm);
                }
                drawEllipse(renderer, s->data.ellipse.x, s->data.ellipse.y, s->data.ellipse.rx, s->data.ellipse.ry, s->color, s->typeForm);
                break;
            case SHAPE_RECTANGLE:
                if (s->selected) {
                    // Dessiner un contour rouge si sélectionné
                    drawRectangle(renderer, s->data.rectangle.x, s->data.rectangle.y, s->data.rectangle.w + 2, s->data.rectangle.h + 2, 0xFF0000FF, s->typeForm);
                }
                drawRectangle(renderer, s->data.rectangle.x, s->data.rectangle.y, s->data.rectangle.w, s->data.rectangle.h, s->color, s->typeForm);
                break;
            case SHAPE_LINE:
                if (s->selected) {
                    // Dessiner un contour rouge si sélectionné
                    drawLine(renderer, s->data.line.x1, s->data.line.y1, s->data.line.x2, s->data.line.y2, 0xFF0000FF, s->data.line.thickness, s->typeForm);
                }
                // Dessiner la ligne
                drawLine(renderer, s->data.line.x1, s->data.line.y1, s->data.line.x2, s->data.line.y2, s->color, s->data.line.thickness, s->typeForm);
                break;
            case SHAPE_POLYGON:
                if (s->selected) {
                    // Dessiner un contour rouge si sélectionné
                    drawCustomPolygon(renderer, s->data.polygon.cx, s->data.polygon.cy, s->data.polygon.radius, s->data.polygon.sides, 0xFF0000FF, "filled");
                }
                // Dessiner le polygone
                drawCustomPolygon(renderer, s->data.polygon.cx, s->data.polygon.cy, s->data.polygon.radius, s->data.polygon.sides, s->color, s->typeForm);
                break;
            case SHAPE_ARC:
                if (s->selected) {
                    // Dessiner un contour rouge si sélectionné
                   drawArc(renderer, s->data.arc.x, s->data.arc.y, s->data.arc.radius, s->data.arc.start_angle, s->data.arc.end_angle, 0xFF0000FF);
                }
                // Dessiner l'arc
                drawArc(renderer, s->data.arc.x, s->data.arc.y, s->data.arc.radius, s->data.arc.start_angle, s->data.arc.end_angle, s->color);
                break;
            case SHAPE_ROUNDEDRECTANGLE:
                if (s->selected) {
                    // Dessiner un contour rouge si sélectionné
                    drawRoundedRectangle(renderer, s->data.roundedrectangle.x1, s->data.roundedrectangle.y1, s->data.roundedrectangle.x2, s->data.roundedrectangle.y2, s->data.roundedrectangle.rad, 0xFF0000FF, s->typeForm);
                }
                // Dessiner le rectangle arrondi
                drawRoundedRectangle(renderer, s->data.roundedrectangle.x1, s->data.roundedrectangle.y1, s->data.roundedrectangle.x2, s->data.roundedrectangle.y2, s->data.roundedrectangle.rad, s->color, s->typeForm);
                break;
            default:
                break;
        }
    }

    SDL_RenderPresent(renderer);
}
