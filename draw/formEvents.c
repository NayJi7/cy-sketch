#include "../files.h/formEvents.h"
#include "../files.h/cursorEvents.h"

#include <math.h>

// Global table to storage the forms
Shape shapes[MAX_SHAPES];
int shapeCount = 0;


/**
 * @brief Renders the content of a texture onto the screen and handles optional delays.
 * 
 * @param renderer Renderer used for drawing.
 * @param texture Texture to be rendered.
 * @param time Delay in milliseconds after rendering (set to 0 for no delay).
 */
void renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, int time) {
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    if(time != 0) SDL_Delay(time);
    SDL_RenderPresent(renderer);
    SDL_SetRenderTarget(renderer, texture);
}

/**
 * @brief Renders a shape on the screen based on its type and properties.
 * 
 * @param shape Pointer to the shape structure containing its type, dimensions, color, etc.
 */
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
                circleColor(renderer, shape->data.circle.x, shape->data.circle.y, shape->data.circle.radius + 5, blue);
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

            SDL_Point pointsR[5];
            SDL_Point selectionPoints[5];
            int offset = 25; // Espace encore plus large autour pour le contour

            // Calcul des points du rectangle
            pointsR[0].x = cos(angle) * (shape->data.rectangle.x - cx) - sin(angle) * (shape->data.rectangle.y - cy) + cx;
            pointsR[0].y = sin(angle) * (shape->data.rectangle.x - cx) + cos(angle) * (shape->data.rectangle.y - cy) + cy;

            pointsR[1].x = cos(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) - sin(angle) * (shape->data.rectangle.y - cy) + cx;
            pointsR[1].y = sin(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) + cos(angle) * (shape->data.rectangle.y - cy) + cy;

            pointsR[2].x = cos(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) - sin(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cx;
            pointsR[2].y = sin(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) + cos(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cy;

            pointsR[3].x = cos(angle) * (shape->data.rectangle.x - cx) - sin(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cx;
            pointsR[3].y = sin(angle) * (shape->data.rectangle.x - cx) + cos(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cy;

            pointsR[4] = pointsR[0]; // Fermer le rectangle

            // Calcul des points pour le contour jaune en prolongeant les vecteurs
            for (int i = 0; i < 4; i++) {
                double dx = pointsR[i].x - cx;
                double dy = pointsR[i].y - cy;
                double length = sqrt(dx * dx + dy * dy);

                selectionPoints[i].x = cx + (dx / length) * (length + offset);
                selectionPoints[i].y = cy + (dy / length) * (length + offset);
            }
            selectionPoints[4] = selectionPoints[0]; // Fermer le contour

            // Dessiner le rectangle
            if (strcmp(shape->typeForm, "filled") == 0) {
                filledPolygonColor(renderer, 
                                    (Sint16[]){pointsR[0].x, pointsR[1].x, pointsR[2].x, pointsR[3].x}, 
                                    (Sint16[]){pointsR[0].y, pointsR[1].y, pointsR[2].y, pointsR[3].y}, 
                                    4, shape->color);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                SDL_RenderDrawLines(renderer, pointsR, 5);
            }

            // Dessiner le contour jaune si sélectionné
            if (shape->selected) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 139, 255);
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

                SDL_SetRenderDrawColor(renderer, 0, 0, 139, 255); // Jaune
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
                SDL_SetRenderDrawColor(renderer, 0, 0, 139, 255); // Jaune
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            }
            break;



        case SHAPE_ROUNDED_RECTANGLE:

            SDL_SetRenderDrawColor(renderer, 0, 0, 139, 255);

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
                // Déterminer une marge cohérente pour le contour jaune
                int margin = -10; // Ajuste cette valeur selon tes besoins

                // Calculer les nouvelles dimensions du rectangle
                int new_x1 = shape->data.rounded_rectangle.x1 - margin;
                int new_y1 = shape->data.rounded_rectangle.y1 - margin;
                int new_x2 = shape->data.rounded_rectangle.x2 + margin;
                int new_y2 = shape->data.rounded_rectangle.y2 + margin;

                // Calculer un rayon ajusté
                int new_radius = shape->data.rounded_rectangle.radius + margin;

                // Appliquer la couleur jaune
                SDL_SetRenderDrawColor(renderer, 0, 0, 139, 255); // Jaune

                // Dessiner le contour jaune
                roundedRectangleColor(renderer,
                                    new_x1, new_y1,
                                    new_x2, new_y2,
                                    new_radius,
                                    black); // Couleur jaune
            }   
            break;
        
        case SHAPE_POLYGON:
            Uint8 rP = (shape->color >> 24) & 0xFF; // Rouge
            Uint8 gP = (shape->color >> 16) & 0xFF; // Vert
            Uint8 bP = (shape->color >> 8) & 0xFF;  // Bleu
            Uint8 aP = shape->color & 0xFF;         // Alpha

            SDL_SetRenderDrawColor(renderer, rP, gP, bP, aP);

            int sides = shape->data.polygon.sides;
            if (sides < 3) return; // Ne pas dessiner si moins de 3 côtés

            double angleStep = 2 * M_PI / sides; // Angle entre chaque sommet
            SDL_Point pointsP[13]; // Points pour le contour fermé

            // Calcul des sommets
            for (int i = 0; i < sides; i++) {
                double angle = i * angleStep + (shape->rotation * M_PI / 180.0);
                pointsP[i].x = shape->data.polygon.cx + cos(angle) * shape->data.polygon.radius;
                pointsP[i].y = shape->data.polygon.cy + sin(angle) * shape->data.polygon.radius;
            }
            pointsP[sides] = pointsP[0]; // Boucler pour fermer le polygone

            // Dessiner le polygone
            if (strcmp(shape->typeForm, "filled") == 0) {
                Sint16 xCoords[sides];
                Sint16 yCoords[sides];
                for (int i = 0; i < sides; i++) {
                    xCoords[i] = pointsP[i].x;
                    yCoords[i] = pointsP[i].y;
                }
                filledPolygonColor(renderer, xCoords, yCoords, sides, shape->color);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                SDL_RenderDrawLines(renderer, pointsP, sides + 1);
            }

            // Ajouter un contour jaune si sélectionné
            if (shape->selected) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 139, 255); // Couleur jaune
                SDL_RenderDrawLines(renderer, pointsP, sides + 1);
            }
            break;
        
        case SHAPE_ARC:
            Uint8 rA = (shape->color >> 24) & 0xFF; // Rouge
            Uint8 gA = (shape->color >> 16) & 0xFF; // Vert
            Uint8 bA = (shape->color >> 8) & 0xFF;  // Bleu
            Uint8 aA = shape->color & 0xFF;         // Alpha

            SDL_SetRenderDrawColor(renderer, rA, gA, bA, aA);

            // Vérifier les angles pour éviter l'inversion
            int startAngle = shape->data.arc.start_angle % 360;
            int endAngle = shape->data.arc.end_angle % 360;
            if (startAngle < 0) startAngle += 360;
            if (endAngle < 0) endAngle += 360;

            // Appliquer la rotation à l'arc
            int centerX = shape->data.arc.x;
            int centerY = shape->data.arc.y;
            double rotationRad = shape->rotation * M_PI / 180.0; // Convertir la rotation en radians

            // Déplacer le centre de rotation si nécessaire (ajuster avec d'autres transformations si besoin)
            int rotatedX = (int)(cos(rotationRad) * (centerX - centerX) - sin(rotationRad) * (centerY - centerY)) + centerX;
            int rotatedY = (int)(sin(rotationRad) * (centerX - centerX) + cos(rotationRad) * (centerY - centerY)) + centerY;

            // Ajuster les angles de départ et de fin selon la rotation
            startAngle = (startAngle + (int)shape->rotation) % 360;
            endAngle = (endAngle + (int)shape->rotation) % 360;

            // Dessiner l'arc
            if (strcmp(shape->typeForm, "empty") == 0) {
                arcColor(renderer, rotatedX, rotatedY, shape->data.arc.radius,
                        startAngle, endAngle, shape->color);
            } else if (strcmp(shape->typeForm, "filled") == 0) {
                filledPieColor(renderer, rotatedX, rotatedY, shape->data.arc.radius,
                            startAngle, endAngle, shape->color);
            }

            // Dessiner un contour jaune si sélectionné
            if (shape->selected) {
                arcColor(renderer, rotatedX, rotatedY, shape->data.arc.radius + 5,
                        startAngle, endAngle, blue);
            }
            break;

   
    }
}

/**
 * @brief Adds a new shape to the shape list.
 * 
 * @param shape The shape to be added.
 */
void addShape(Shape shape) {
    if (shapeCount >= MAX_SHAPES) {
        printf("Erreur : Nombre maximal de formes atteint.\n");
        return;
    }
    shapes[shapeCount++] = shape;
}

/**
 * @brief Deletes a shape from the shape list at the specified index.
 * 
 * @param index The index of the shape to delete.
 */
void deleteShape(int index) {
    if (index < 0 || index >= shapeCount) return;
    for (int i = index; i < shapeCount - 1; i++) {
        shapes[i] = shapes[i + 1];
    }
    shapeCount--;
}

/**
 * @brief Adjusts the size of a shape based on a zoom factor.
 * 
 * @param shape The shape to zoom in or out.
 * @param zoomFactor The zoom multiplier (positive for zoom in, negative for zoom out).
 */
void zoomShape(Shape *shape, float zoomFactor) {
    switch (shape->type) {
        case SHAPE_RECTANGLE:

        // Calculer le rapport d'aspect initial du rectangle
        float aspectRatio = (float)shape->data.rectangle.width / shape->data.rectangle.height;

        // Appliquer le zoom
        shape->data.rectangle.width += (int)(zoomFactor * 10);
        shape->data.rectangle.height = (int)(shape->data.rectangle.width / aspectRatio);

        // Définir une taille minimale pour la largeur
        const int minWidth = 10;
        // on peut rajouter max aussi stv 

        // Vérifier et ajuster la taille si elle tombe en dessous de la largeur minimale
        if (shape->data.rectangle.width < minWidth) {
            shape->data.rectangle.width = minWidth;
            shape->data.rectangle.height = (int)(minWidth / aspectRatio); // Ajuster en fonction du rapport
        }

        // Optionnel : Limiter aussi une hauteur minimale
        const int minHeight = 10;
        if (shape->data.rectangle.height < minHeight) {
            shape->data.rectangle.height = minHeight;
            shape->data.rectangle.width = (int)(minHeight * aspectRatio); // Ajuster en fonction du rapport
        }
        break;


        case SHAPE_CIRCLE:
            shape->data.circle.radius += (int)(zoomFactor * 5);

            // Rayon minimum pour éviter des bugs
            if (shape->data.circle.radius < 20) shape->data.circle.radius = 20;
            break;

        case SHAPE_ELLIPSE:
            shape->data.ellipse.rx += (int)(zoomFactor * 5);
            shape->data.ellipse.ry += (int)(zoomFactor * 5);

            // Valeurs minimum pour éviter un aplatissement complet
            // Empêcher l'ellipse de devenir un cercle parfait en préservant le rapport d'aspect
            if (shape->data.ellipse.rx < 20){
                shape->data.ellipse.rx = 20;
                shape->data.ellipse.ry = 20 * 2; // Exemple : conserver un rapport 1:2
            } else if (shape->data.ellipse.ry < 20) {
                shape->data.ellipse.ry = 20;
                shape->data.ellipse.rx = 20 * 2; // Exemple : conserver un rapport 2:1
            }
            break;

        case SHAPE_LINE:
            // Calcul du centre de la ligne
            int cx = (shape->data.line.x1 + shape->data.line.x2) / 2;
            int cy = (shape->data.line.y1 + shape->data.line.y2) / 2;

            // Ajuster les points pour zoomer/dézoomer
            shape->data.line.x1 += (int)((shape->data.line.x1 - cx) * zoomFactor * 0.1);
            shape->data.line.y1 += (int)((shape->data.line.y1 - cy) * zoomFactor * 0.1);
            shape->data.line.x2 += (int)((shape->data.line.x2 - cx) * zoomFactor * 0.1);
            shape->data.line.y2 += (int)((shape->data.line.y2 - cy) * zoomFactor * 0.1);

            // Calcul de la longueur de la ligne
            int dx = shape->data.line.x2 - shape->data.line.x1;
            int dy = shape->data.line.y2 - shape->data.line.y1;
            double length = sqrt(dx * dx + dy * dy);

            // Assurer une longueur minimale
            const double minLength = 50.0; // Minimum raisonnable pour une ligne
            if (length < minLength) {
                double scale = minLength / length;
                shape->data.line.x1 = cx + (int)((shape->data.line.x1 - cx) * scale);
                shape->data.line.y1 = cy + (int)((shape->data.line.y1 - cy) * scale);
                shape->data.line.x2 = cx + (int)((shape->data.line.x2 - cx) * scale);
                shape->data.line.y2 = cy + (int)((shape->data.line.y2 - cy) * scale);
            }
            break;

        case SHAPE_ROUNDED_RECTANGLE: {
            // Calculer la largeur, la hauteur et le centre actuels
            int width = shape->data.rounded_rectangle.x2 - shape->data.rounded_rectangle.x1;
            int height = shape->data.rounded_rectangle.y2 - shape->data.rounded_rectangle.y1;
            int centerX = shape->data.rounded_rectangle.x1 + width / 2;
            int centerY = shape->data.rounded_rectangle.y1 + height / 2;

            // Définir le pourcentage de zoom (5% par défaut)
            float zoomPercentage = 0.05f * zoomFactor; // Positif ou négatif selon zoom

            // Calculer les nouvelles dimensions
            int newWidth = width + (int)(width * zoomPercentage);
            int newHeight = height + (int)(height * zoomPercentage);

            // Recalculer les coordonnées pour conserver le centre
            shape->data.rounded_rectangle.x1 = centerX - newWidth / 2;
            shape->data.rounded_rectangle.y1 = centerY - newHeight / 2;
            shape->data.rounded_rectangle.x2 = centerX + newWidth / 2;
            shape->data.rounded_rectangle.y2 = centerY + newHeight / 2;

            // Ajuster le rayon des coins proportionnellement à la hauteur
            shape->data.rounded_rectangle.radius = (int)(newHeight * 0.2); // Exemple : 20% de la hauteur
            if (shape->data.rounded_rectangle.radius < 5) {
                shape->data.rounded_rectangle.radius = 5; // Rayon minimal pour éviter des erreurs
            }
        }
            break;


        case SHAPE_POLYGON:
            shape->data.polygon.radius += (int)(zoomFactor * 5);

            // Empêcher un rayon trop petit
            if (shape->data.polygon.radius < 35) shape->data.polygon.radius = 35;
            break;

        case SHAPE_ARC:
            shape->data.arc.radius += (int)(zoomFactor * 5);

            // Empêcher un rayon trop petit
            if (shape->data.arc.radius < 30) {
                shape->data.arc.radius = 30;
            }
            break;
    }
}

/**
 * @brief Rotates a selected shape by the given angle.
 * 
 * @param shape The shape to rotate.
 * @param angle The angle in degrees (positive for clockwise, negative for counterclockwise).
 */
void rotateShape(Shape *shape, int angle) {
    if (!shape->selected) return;

    shape->rotation += angle;
    if (shape->rotation >= 360) {
        shape->rotation -= 360;
    } else if (shape->rotation < 0) {
        shape->rotation += 360;
    }

}

/**
 * @brief Moves shapes with the mouse during drag events.
 * 
 * @param shapes List of shapes in the scene.
 * @param shapeCount Number of shapes in the list.
 * @param event SDL_Event structure containing mouse motion data.
 * @param cursor Pointer to the cursor structure for updating its position.
 */
void moveShapesWithMouse(Shape *shapes, int shapeCount, SDL_Event *event, Cursor *cursor) {
    for (int i = 0; i < shapeCount; i++) {
        if (shapes[i].selected) {
            switch (shapes[i].type) {
                case SHAPE_RECTANGLE: {
                    shapes[i].data.rectangle.x = event->motion.x - shapes[i].data.rectangle.width / 2;
                    shapes[i].data.rectangle.y = event->motion.y - shapes[i].data.rectangle.height / 2;

                    cursor->x = shapes[i].data.rectangle.x + shapes[i].data.rectangle.width / 2;
                    cursor->y = shapes[i].data.rectangle.y + shapes[i].data.rectangle.height / 2;
                    break;
                }

                case SHAPE_CIRCLE: {
                    shapes[i].data.circle.x = event->motion.x;
                    shapes[i].data.circle.y = event->motion.y;

                    cursor->x = shapes[i].data.circle.x;
                    cursor->y = shapes[i].data.circle.y;
                    break;
                }

                case SHAPE_ELLIPSE: {
                    shapes[i].data.ellipse.x = event->motion.x;
                    shapes[i].data.ellipse.y = event->motion.y;

                    cursor->x = shapes[i].data.ellipse.x;
                    cursor->y = shapes[i].data.ellipse.y;
                    break;
                }

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

                case SHAPE_ROUNDED_RECTANGLE: {
                    int dx = event->motion.xrel;
                    int dy = event->motion.yrel;

                    // Déplacer les coordonnées
                    shapes[i].data.rounded_rectangle.x1 += dx;
                    shapes[i].data.rounded_rectangle.y1 += dy;
                    shapes[i].data.rounded_rectangle.x2 += dx;
                    shapes[i].data.rounded_rectangle.y2 += dy;

                    // Assurez-vous que le rayon reste inchangé
                    shapes[i].data.rounded_rectangle.radius = shapes[i].data.rounded_rectangle.radius;
                    break;
                }
                
                case SHAPE_POLYGON: {
                    shapes[i].data.polygon.cx = event->motion.x; // Déplacer le centre en x
                    shapes[i].data.polygon.cy = event->motion.y; // Déplacer le centre en y

                    // Mettre à jour la position du curseur
                    cursor->x = shapes[i].data.polygon.cx;
                    cursor->y = shapes[i].data.polygon.cy;
                    break;
                }

                case SHAPE_ARC: {
                    shapes[i].data.arc.x = event->motion.x; // Déplacer le centre en x
                    shapes[i].data.arc.y = event->motion.y; // Déplacer le centre en y

                    // Mettre à jour la position du curseur
                    cursor->x = shapes[i].data.arc.x;
                    cursor->y = shapes[i].data.arc.y;
                    break;
                }
            }
        }
    }
}

/**
 * @brief Moves selected shapes by a specified offset.
 * 
 * @param shapes List of shapes in the scene.
 * @param shapeCount Number of shapes in the list.
 * @param dx Horizontal movement offset.
 * @param dy Vertical movement offset.
 */
void moveSelectedShapes(Shape *shapes, int shapeCount, int dx, int dy) {
    
    for (int i = 0; i < shapeCount; i++) {
        if (shapes[i].selected) {
            switch (shapes[i].type) {
                case SHAPE_RECTANGLE: {
                    shapes[i].data.rectangle.x += dx;
                    shapes[i].data.rectangle.y += dy;
                    break;
                }

                case SHAPE_CIRCLE: {
                    shapes[i].data.circle.x += dx;
                    shapes[i].data.circle.y += dy;
                    break;
                }

                case SHAPE_ELLIPSE: {
                    shapes[i].data.ellipse.x += dx;
                    shapes[i].data.ellipse.y += dy;
                    break;
                }

                case SHAPE_LINE: {
                    shapes[i].data.line.x1 += dx;
                    shapes[i].data.line.y1 += dy;
                    shapes[i].data.line.x2 += dx;
                    shapes[i].data.line.y2 += dy;
                    break;
                }

                case SHAPE_ROUNDED_RECTANGLE: {
                    shapes[i].data.rounded_rectangle.x1 += dx;
                    shapes[i].data.rounded_rectangle.y1 += dy;
                    shapes[i].data.rounded_rectangle.x2 += dx;
                    shapes[i].data.rounded_rectangle.y2 += dy;
                    break;
                }

                case SHAPE_POLYGON: {
                    shapes[i].data.polygon.cx += dx;
                    shapes[i].data.polygon.cy += dy; 
                    break;
                }

                case SHAPE_ARC: {
                    shapes[i].data.arc.x += dx;
                    shapes[i].data.arc.y += dy; 
                    break;
                }
            }
        }
    }
}




/**
 * @brief Common parameters for functions.
 * 
 * @param x  X-coordinate of the point.
 * @param y  X-coordinate of the point.
 */


/**
 * @brief Checks if a point lies inside a circle.
 * 
 * @param cx X-coordinate of the circle's center.
 * @param cy Y-coordinate of the circle's center.
 * @param radius Radius of the circle.
 * 
 * @return 1 if the point is inside the circle, 0 otherwise.
 */
int isPointInCircle(int x, int y, int cx, int cy, int radius) {
    // Calculer la distance entre le point (x, y) et le centre du cercle (cx, cy)
    int dx = x - cx;
    int dy = y - cy;
    // Si la distance est inférieure ou égale au rayon, le point est à l'intérieur
    return (dx * dx + dy * dy <= radius * radius);
}

/**
 * @brief Checks if a point lies inside an ellipse.
 * 
 * @param cx X-coordinate of the ellipse's center.
 * @param cy Y-coordinate of the ellipse's center.
 * @param rx Radius of the ellipse along the X-axis.
 * @param ry Radius of the ellipse along the Y-axis.
 * 
 * @return 1 if the point is inside the ellipse, 0 otherwise.
 */
int isPointInEllipse(int x, int y, int cx, int cy, int rx, int ry) {
    // Vérifie si le point respecte l'équation de l'ellipse (x-cx)^2 / rx^2 + (y-cy)^2 / ry^2 <= 1
    return ((x - cx) * (x - cx)) / (rx * rx) + ((y - cy) * (y - cy)) / (ry * ry) <= 1;
}

/**
 * @brief Checks if a point lies inside an arc.
 * 
 * @param cx X-coordinate of the arc's center.
 * @param cy Y-coordinate of the arc's center.
 * @param radius Radius of the arc.
 * @param startAngle Starting angle of the arc in degrees.
 * @param endAngle Ending angle of the arc in degrees.
 * 
 * @return 1 if the point is inside the arc, 0 otherwise.
 */
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

/**
 * @brief Checks if a point lies inside a rectangle.
 * 
 * @param rectX X-coordinate of the rectangle's top-left corner.
 * @param rectY Y-coordinate of the rectangle's top-left corner.
 * @param rectW Width of the rectangle.
 * @param rectH Height of the rectangle.
 * 
 * @return 1 if the point is inside the rectangle, 0 otherwise.
 */
int isPointInRectangle(int x, int y, int rectX, int rectY, int rectW, int rectH) {
    // Vérifie si le point est dans les limites du rectangle
    return (x >= rectX && x <= rectX + rectW && y >= rectY && y <= rectY + rectH);
}

/**
 * @brief Checks if a point lies inside a rounded rectangle.
 * 
 * @param x1 X-coordinate of the top-left corner.
 * @param y1 Y-coordinate of the top-left corner.
 * @param x2 X-coordinate of the bottom-right corner.
 * @param y2 Y-coordinate of the bottom-right corner.
 * @param radius Radius of the rounded corners.
 * 
 * @return 1 if the point is inside the rounded rectangle, 0 otherwise.
 */
int isPointInRoundedRectangle(Sint16 x, Sint16 y, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 radius) {
    // Corriger les coordonnées si elles ne sont pas dans l'ordre correct
    if (x1 > x2) {
        Sint16 temp = x1;
        x1 = x2;
        x2 = temp;
    }

    if (y1 > y2) {
        Sint16 temp = y1;
        y1 = y2;
        y2 = temp;
    }

    printf("Corrected Rectangle: (%d, %d, %d, %d), Radius: %d\n", x1, y1, x2, y2, radius);

    // Vérification si le point est dans la partie rectangulaire centrale (sans les coins arrondis)
    if (x >= x1 + radius && x <= x2 - radius && y >= y1 && y <= y2) {
        return 1; // Le point est dans la zone rectangulaire centrale
    }

    if (x >= x1 && x <= x2 && y >= y1 + radius && y <= y2 - radius) {
        return 1; // Le point est dans les zones rectangulaires gauche/droite
    }

    // Vérification des coins arrondis (par rapport à leurs centres)
    if (isPointInCircle(x, y, x1 + radius, y1 + radius, radius)) return 1;
    if (isPointInCircle(x, y, x2 - radius, y1 + radius, radius)) return 1;
    if (isPointInCircle(x, y, x1 + radius, y2 - radius, radius)) return 1;
    if (isPointInCircle(x, y, x2 - radius, y2 - radius, radius)) return 1;

    return 0; // Le point n'est pas dans le RoundedRectangle
}

/**
 * @brief Checks if a point lies inside a polygon.
 * 
 * @param cx X-coordinate of the polygon's center.
 * @param cy Y-coordinate of the polygon's center.
 * @param radius Radius of the circumscribed circle.
 * @param sides Number of sides of the polygon.
 * 
 * @return 1 if the point is inside the polygon, 0 otherwise.
 */
int isPointInPolygon(int x, int y, int cx, int cy, int radius, int sides) {
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
        if (((y1 > y) != (y2 > y)) &&
            (x < (x2 - x1) * (y - y1) / (y2 - y1) + x1)) {
            c = !c;
        }
    }

    return c; // Retourne 1 si le point est à l'intérieur, sinon 0
}

/**
 * @brief Checks if a point lies near a line segment.
 * 
 * @param x1 X-coordinate of the line's starting point.
 * @param y1 Y-coordinate of the line's starting point.
 * @param x2 X-coordinate of the line's ending point.
 * @param y2 Y-coordinate of the line's ending point.
 * @param tolerance Distance threshold for proximity to the line.
 * 
 * @return 1 if the point is near the line, 0 otherwise.
 */
int isPointInLine(int x, int y, int x1, int y1, int x2, int y2, int tolerance) {
    // Calculer la distance du point à la ligne
    float lineLength = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    // Convertir les entiers en flottants pour utiliser fabs correctement
    float distance = fabs((float)(y2 - y1) * x - (float)(x2 - x1) * y + (float)(x2 * y1) - (float)(y2 * x1)) / lineLength;
    
    // Si la distance est inférieure à la tolérance, le point est proche de la ligne
    return (distance <= tolerance);
}
