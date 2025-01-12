#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "../files.h/formEvents.h"
#include "../files.h/cursorEvents.h"
#include "../files.h/colors.h"

#include <math.h>
#include <limits.h>

// ANSI escape codes for colors
#define RED_COLOR "-#red "

// Global table to storage the forms
Shape shapes[MAX_SHAPES];
int shapeCount = 0;

/**
 * @brief Sets the rendering color for the SDL renderer, optimizing redundant calls.
 *
 * This function changes the render draw color of the SDL renderer only when the requested
 * color is different from the current one. The `color` is provided as a 32-bit unsigned integer,
 * where each byte represents a color component in RGBA order.
 *
 * @param renderer The SDL renderer whose draw color is to be set.
 * @param color A 32-bit RGBA color value (e.g., 0xRRGGBBAA).
 */
void setRenderColor(SDL_Renderer* renderer, SDL_Color color) {
    static SDL_Color currentColor = {255, 255, 255, 255}; // Default color is white
    if (currentColor.r != color.r || currentColor.g != color.g || currentColor.b != color.b || currentColor.a != color.a) {
        currentColor = color;

        // Set the draw color in the renderer
        SDL_SetRenderDrawColor(renderer, currentColor.r, currentColor.g, currentColor.b, currentColor.a);
    }
}

/**
 * @brief Adjusts the brightness of a given color.
 *
 * This function modifies the RGB components of the input color by multiplying them
 * by a brightness factor. The alpha component remains unchanged.
 * 
 * @param color The original 32-bit RGBA color.
 * @return The selected color as a 32-bit RGBA value.
 */
SDL_Color selectColor(SDL_Color color) {
    // Brighten the original color for selection by changing the RGB values
    SDL_Color selectedColor = color;
    selectedColor.r = (Uint8)fminf(255, color.r * 0.8);
    selectedColor.g = (Uint8)fminf(255, color.g * 0.8);
    selectedColor.b = (Uint8)fminf(255, color.b * 0.8);
    return selectedColor;
}

/**
 * @brief Renders the content of a texture onto the screen and handles optional delays.
 *
 * @param renderer Renderer used for drawing.
 * @param texture Texture to be rendered.
 * @param time Delay in milliseconds after rendering (set to 0 for no delay).
 * @return int Returns 0 on success, -1 on failure
 */
int renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, int time) {
    // Validate parameters
    if (!renderer || !texture) {
        printf("%sExecutionError: Invalid renderer or texture provided\n", 
               RED_COLOR);
        return -1;
    }

    // Reset render target to screen
    if (SDL_SetRenderTarget(renderer, NULL) != 0) {
        printf("%sExecutionError: Failed to reset render target.\n", 
               RED_COLOR);
        return -1;
    }

    // Copy texture to renderer
    if (SDL_RenderCopy(renderer, texture, NULL, NULL) != 0) {
        printf("%sExecutionError: Failed to copy texture to renderer.\n", 
               RED_COLOR);
        return -1;
    }

    // Add optional delay
    if (time != 0) SDL_Delay(time);

    // Present the rendered content
    SDL_RenderPresent(renderer);

    // Restore texture as render target
    if (SDL_SetRenderTarget(renderer, texture) != 0) {
        char* error = SDL_GetError();
        printf("%sExecutionError: Failed to restore render target.\n", 
               RED_COLOR);
        return -1;
    }

    return 0;
}

/**
 * @brief Renders a shape on the screen based on its type and properties.
 * 
 * @param renderer The SDL renderer to use for drawing
 * @param shape Pointer to the shape structure containing its type, dimensions, color, etc.
 */
void renderShape(SDL_Renderer *renderer, Shape *shape) {
    if (shape->typeForm == NULL || (strcmp(shape->typeForm, "filled") != 0 && strcmp(shape->typeForm, "empty") != 0)) {
        return; // Skip rendering if the typeForm is invalid.
    }

    // Switch based on the shape type
    switch (shape->type) {  
        case SHAPE_CIRCLE: {
            setRenderColor(renderer, shape->color);

            // Render the filled or empty circle based on typeForm
            if (strcmp(shape->typeForm, "filled") == 0) {
                filledCircleRGBA(renderer, shape->data.circle.x, shape->data.circle.y, shape->data.circle.radius, 
                                shape->color.r, shape->color.g, shape->color.b, shape->color.a);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                circleRGBA(renderer, shape->data.circle.x, shape->data.circle.y, shape->data.circle.radius, 
                          shape->color.r, shape->color.g, shape->color.b, shape->color.a);
            }

            // Highlight the circle if selected
            if (shape->selected) {
                SDL_Color selectedColor = selectColor(shape->color);
                if (strcmp(shape->typeForm, "filled") == 0) {
                    filledCircleRGBA(renderer, shape->data.circle.x, shape->data.circle.y, shape->data.circle.radius + 5, 
                                   selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
                } else {
                    circleRGBA(renderer, shape->data.circle.x, shape->data.circle.y, shape->data.circle.radius + 5, 
                              selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
                }
            }
            break;
        }

        case SHAPE_RECTANGLE: {
            setRenderColor(renderer, shape->color);

            // Calculate the center of the rectangle
            int cx = shape->data.rectangle.x + shape->data.rectangle.width / 2;
            int cy = shape->data.rectangle.y + shape->data.rectangle.height / 2;
            double angle = shape->rotation * M_PI / 180.0;

            SDL_Point points[5];  // Array to store rotated rectangle vertices

            // Calculate rotated rectangle vertices
            points[0].x = cos(angle) * (shape->data.rectangle.x - cx) - sin(angle) * (shape->data.rectangle.y - cy) + cx;
            points[0].y = sin(angle) * (shape->data.rectangle.x - cx) + cos(angle) * (shape->data.rectangle.y - cy) + cy;

            points[1].x = cos(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) - sin(angle) * (shape->data.rectangle.y - cy) + cx;
            points[1].y = sin(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) + cos(angle) * (shape->data.rectangle.y - cy) + cy;

            points[2].x = cos(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) - sin(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cx;
            points[2].y = sin(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) + cos(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cy;

            points[3].x = cos(angle) * (shape->data.rectangle.x - cx) - sin(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cx;
            points[3].y = sin(angle) * (shape->data.rectangle.x - cx) + cos(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cy;

            points[4] = points[0];  // Close the rectangle by repeating the first point

            // Render the rectangle
            if (strcmp(shape->typeForm, "filled") == 0) {
                filledPolygonRGBA(renderer, 
                                (Sint16[]){points[0].x, points[1].x, points[2].x, points[3].x}, 
                                (Sint16[]){points[0].y, points[1].y, points[2].y, points[3].y}, 
                                4, shape->color.r, shape->color.g, shape->color.b, shape->color.a);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                SDL_RenderDrawLines(renderer, points, 5);
            }

            // Render selection highlight if selected
            if (shape->selected) {
                SDL_Color selectedColor = selectColor(shape->color);
                // Calculate enlarged rectangle vertices (5 pixels larger)
                int enlargement = 5;
                SDL_Point enlargedPoints[5];
                
                enlargedPoints[0].x = cos(angle) * (shape->data.rectangle.x - enlargement - cx) - sin(angle) * (shape->data.rectangle.y - enlargement - cy) + cx;
                enlargedPoints[0].y = sin(angle) * (shape->data.rectangle.x - enlargement - cx) + cos(angle) * (shape->data.rectangle.y - enlargement - cy) + cy;

                enlargedPoints[1].x = cos(angle) * (shape->data.rectangle.x + shape->data.rectangle.width + enlargement - cx) - sin(angle) * (shape->data.rectangle.y - enlargement - cy) + cx;
                enlargedPoints[1].y = sin(angle) * (shape->data.rectangle.x + shape->data.rectangle.width + enlargement - cx) + cos(angle) * (shape->data.rectangle.y - enlargement - cy) + cy;

                enlargedPoints[2].x = cos(angle) * (shape->data.rectangle.x + shape->data.rectangle.width + enlargement - cx) - sin(angle) * (shape->data.rectangle.y + shape->data.rectangle.height + enlargement - cy) + cx;
                enlargedPoints[2].y = sin(angle) * (shape->data.rectangle.x + shape->data.rectangle.width + enlargement - cx) + cos(angle) * (shape->data.rectangle.y + shape->data.rectangle.height + enlargement - cy) + cy;

                enlargedPoints[3].x = cos(angle) * (shape->data.rectangle.x - enlargement - cx) - sin(angle) * (shape->data.rectangle.y + shape->data.rectangle.height + enlargement - cy) + cx;
                enlargedPoints[3].y = sin(angle) * (shape->data.rectangle.x - enlargement - cx) + cos(angle) * (shape->data.rectangle.y + shape->data.rectangle.height + enlargement - cy) + cy;

                enlargedPoints[4] = enlargedPoints[0];

                if (strcmp(shape->typeForm, "filled") == 0) {
                    filledPolygonRGBA(renderer, 
                                    (Sint16[]){enlargedPoints[0].x, enlargedPoints[1].x, enlargedPoints[2].x, enlargedPoints[3].x}, 
                                    (Sint16[]){enlargedPoints[0].y, enlargedPoints[1].y, enlargedPoints[2].y, enlargedPoints[3].y}, 
                                    4, selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
                } else {
                    SDL_RenderDrawLines(renderer, enlargedPoints, 5);
                }
            }
            break;
        }

        case SHAPE_SQUARE: {
            setRenderColor(renderer, shape->color);

            // Calculate the center of the rectangle
            int cx = shape->data.square.x + shape->data.square.c / 2;
            int cy = shape->data.square.y + shape->data.square.c / 2;
            double angle = shape->rotation * M_PI / 180.0;

            SDL_Point points[5];  // Array to store rotated rectangle vertices

            // Calculate rotated rectangle vertices
            points[0].x = cos(angle) * (shape->data.square.x - cx) - sin(angle) * (shape->data.square.y - cy) + cx;
            points[0].y = sin(angle) * (shape->data.square.x - cx) + cos(angle) * (shape->data.square.y - cy) + cy;

            points[1].x = cos(angle) * (shape->data.square.x + shape->data.square.c - cx) - sin(angle) * (shape->data.square.y - cy) + cx;
            points[1].y = sin(angle) * (shape->data.square.x + shape->data.square.c - cx) + cos(angle) * (shape->data.square.y - cy) + cy;

            points[2].x = cos(angle) * (shape->data.square.x + shape->data.square.c - cx) - sin(angle) * (shape->data.square.y + shape->data.square.c - cy) + cx;
            points[2].y = sin(angle) * (shape->data.square.x + shape->data.square.c - cx) + cos(angle) * (shape->data.square.y + shape->data.square.c - cy) + cy;

            points[3].x = cos(angle) * (shape->data.square.x - cx) - sin(angle) * (shape->data.square.y + shape->data.square.c - cy) + cx;
            points[3].y = sin(angle) * (shape->data.square.x - cx) + cos(angle) * (shape->data.square.y + shape->data.square.c - cy) + cy;

            points[4] = points[0];  // Close the rectangle by repeating the first point

            // Render the rectangle
            if (strcmp(shape->typeForm, "filled") == 0) {
                filledPolygonRGBA(renderer, 
                                (Sint16[]){points[0].x, points[1].x, points[2].x, points[3].x}, 
                                (Sint16[]){points[0].y, points[1].y, points[2].y, points[3].y}, 
                                4, shape->color.r, shape->color.g, shape->color.b, shape->color.a);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                SDL_RenderDrawLines(renderer, points, 5);
            }

            // Render selection highlight if selected
            if (shape->selected) {
                SDL_Color selectedColor = selectColor(shape->color);
                // Calculate enlarged rectangle vertices (5 pixels larger)
                int enlargement = 5;
                SDL_Point enlargedPoints[5];
                
                enlargedPoints[0].x = cos(angle) * (shape->data.square.x - enlargement - cx) - sin(angle) * (shape->data.square.y - enlargement - cy) + cx;
                enlargedPoints[0].y = sin(angle) * (shape->data.square.x - enlargement - cx) + cos(angle) * (shape->data.square.y - enlargement - cy) + cy;

                enlargedPoints[1].x = cos(angle) * (shape->data.square.x + shape->data.square.c + enlargement - cx) - sin(angle) * (shape->data.square.y - enlargement - cy) + cx;
                enlargedPoints[1].y = sin(angle) * (shape->data.square.x + shape->data.square.c + enlargement - cx) + cos(angle) * (shape->data.square.y - enlargement - cy) + cy;

                enlargedPoints[2].x = cos(angle) * (shape->data.square.x + shape->data.square.c + enlargement - cx) - sin(angle) * (shape->data.square.y + shape->data.square.c + enlargement - cy) + cx;
                enlargedPoints[2].y = sin(angle) * (shape->data.square.x + shape->data.square.c + enlargement - cx) + cos(angle) * (shape->data.square.y + shape->data.square.c + enlargement - cy) + cy;

                enlargedPoints[3].x = cos(angle) * (shape->data.square.x - enlargement - cx) - sin(angle) * (shape->data.square.y + shape->data.square.c + enlargement - cy) + cx;
                enlargedPoints[3].y = sin(angle) * (shape->data.square.x - enlargement - cx) + cos(angle) * (shape->data.square.y + shape->data.square.c + enlargement - cy) + cy;

                enlargedPoints[4] = enlargedPoints[0];

                if (strcmp(shape->typeForm, "filled") == 0) {
                    filledPolygonRGBA(renderer, 
                                    (Sint16[]){enlargedPoints[0].x, enlargedPoints[1].x, enlargedPoints[2].x, enlargedPoints[3].x}, 
                                    (Sint16[]){enlargedPoints[0].y, enlargedPoints[1].y, enlargedPoints[2].y, enlargedPoints[3].y}, 
                                    4, selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
                } else {
                    SDL_RenderDrawLines(renderer, enlargedPoints, 5);
                }
            }
            break;
        }

        case SHAPE_ELLIPSE: {
            setRenderColor(renderer, shape->color);

            int cxE = shape->data.ellipse.x;
            int cyE = shape->data.ellipse.y;
            double angleE = shape->rotation * M_PI / 180.0; // Convert rotation to radians.

            // Array to store the points for the rotated ellipse.
            SDL_Point ellipsePoints[36]; 
            int pointCount = 36; // Number of points to approximate the ellipse.

            // Calculate the rotated ellipse points.
            for (int i = 0; i < pointCount; i++) {
                double theta = (2 * M_PI * i) / pointCount; // Divide the ellipse into equal angles.
                double dx = shape->data.ellipse.rx * cos(theta);
                double dy = shape->data.ellipse.ry * sin(theta);
                ellipsePoints[i].x = cos(angleE) * dx - sin(angleE) * dy + cxE; // Rotate and translate.
                ellipsePoints[i].y = sin(angleE) * dx + cos(angleE) * dy + cyE;
            }

            // Render the ellipse.
            if (strcmp(shape->typeForm, "filled") == 0) {
                Sint16 xCoords[36];
                Sint16 yCoords[36];
                for (int i = 0; i < pointCount; i++) {
                    xCoords[i] = ellipsePoints[i].x;
                    yCoords[i] = ellipsePoints[i].y;
                }
                filledPolygonRGBA(renderer, xCoords, yCoords, pointCount, shape->color.r, shape->color.g, shape->color.b, shape->color.a);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                SDL_RenderDrawLines(renderer, ellipsePoints, pointCount);
                SDL_RenderDrawLine(renderer, ellipsePoints[pointCount - 1].x, ellipsePoints[pointCount - 1].y, ellipsePoints[0].x, ellipsePoints[0].y);
            }

            // Render selection if the shape is selected.
            if (shape->selected) {
                SDL_Color selectedColor = selectColor(shape->color);
                SDL_Point selectionPoints[36];
                for (int i = 0; i < pointCount; i++) {
                    double theta = (2 * M_PI * i) / pointCount;
                    double dx = (shape->data.ellipse.rx + 5) * cos(theta);
                    double dy = (shape->data.ellipse.ry + 5) * sin(theta);
                    selectionPoints[i].x = cos(angleE) * dx - sin(angleE) * dy + cxE;
                    selectionPoints[i].y = sin(angleE) * dx + cos(angleE) * dy + cyE;
                }
                
                if (strcmp(shape->typeForm, "filled") == 0) {
                    Sint16 xCoords[36];
                    Sint16 yCoords[36];
                    for (int i = 0; i < pointCount; i++) {
                        xCoords[i] = selectionPoints[i].x;
                        yCoords[i] = selectionPoints[i].y;
                    }
                    filledPolygonRGBA(renderer, xCoords, yCoords, pointCount, selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
                } else if (strcmp(shape->typeForm, "empty") == 0) {
                    setRenderColor(renderer, selectedColor);
                    SDL_RenderDrawLines(renderer, selectionPoints, pointCount);
                    SDL_RenderDrawLine(renderer, selectionPoints[pointCount - 1].x, selectionPoints[pointCount - 1].y, selectionPoints[0].x, selectionPoints[0].y);
                }
            }
            break;
        }

        case SHAPE_LINE: {
            setRenderColor(renderer, shape->color);
            
            // Update the actual thickness in the shape structure
            if (strcmp(shape->typeForm, "empty") == 0) {
                shape->data.line.thickness = 1;
            }
            
            // Calculate rotation
            int x1 = shape->data.line.x1;
            int y1 = shape->data.line.y1;
            int x2 = shape->data.line.x2;
            int y2 = shape->data.line.y2;
            
            if (shape->rotation != 0) {
                // Calculate center point
                int cx = (x1 + x2) / 2;
                int cy = (y1 + y2) / 2;
                
                // Convert rotation to radians
                double angle = shape->rotation * M_PI / 180.0;
                
                // Rotate points around center
                int rx1 = cx + (int)((x1 - cx) * cos(angle) - (y1 - cy) * sin(angle));
                int ry1 = cy + (int)((x1 - cx) * sin(angle) + (y1 - cy) * cos(angle));
                int rx2 = cx + (int)((x2 - cx) * cos(angle) - (y2 - cy) * sin(angle));
                int ry2 = cy + (int)((x2 - cx) * sin(angle) + (y2 - cy) * cos(angle));
                
                x1 = rx1;
                y1 = ry1;
                x2 = rx2;
                y2 = ry2;
            }
            
            // Draw the line with appropriate thickness
            if (shape->data.line.thickness > 1) {
                thickLineRGBA(renderer, x1, y1, x2, y2,
                    shape->data.line.thickness,
                    shape->color.r, shape->color.g, shape->color.b, shape->color.a);
            } else {
                lineRGBA(renderer, x1, y1, x2, y2,
                    shape->color.r, shape->color.g, shape->color.b, shape->color.a);
            }
            
            // Draw selection indicator if selected
            if (shape->selected) {
                SDL_Color selectedColor = selectColor(shape->color);
                thickLineRGBA(renderer, x1, y1, x2, y2,
                    shape->data.line.thickness + 4,
                    selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
            }
            break;
        }

        case SHAPE_POLYGON: {
            setRenderColor(renderer, shape->color);

            int sides = shape->data.polygon.sides;
            if (sides < 3) return;

            double angleStep = 2 * M_PI / sides;
            SDL_Point pointsP[13];

            // Calculate the vertices of the polygon with rotation applied
            for (int i = 0; i < sides; i++) {
                double angle = i * angleStep + (shape->rotation * M_PI / 180.0);
                pointsP[i].x = shape->data.polygon.cx + cos(angle) * shape->data.polygon.radius;
                pointsP[i].y = shape->data.polygon.cy + sin(angle) * shape->data.polygon.radius;
            }
            pointsP[sides] = pointsP[0];

            // Render the polygon
            Sint16 xCoords[sides];
            Sint16 yCoords[sides];
            for (int i = 0; i < sides; i++) {
                xCoords[i] = pointsP[i].x;
                yCoords[i] = pointsP[i].y;
            }

            if (strcmp(shape->typeForm, "filled") == 0) {
                filledPolygonRGBA(renderer, xCoords, yCoords, sides, 
                                shape->color.r, shape->color.g, shape->color.b, shape->color.a);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                polygonRGBA(renderer, xCoords, yCoords, sides,
                        shape->color.r, shape->color.g, shape->color.b, shape->color.a);
            }

            // Add a selection highlight if the shape is selected
            if (shape->selected) {
                SDL_Color selectedColor = selectColor(shape->color);
                SDL_Point enlargedPoints[13];
                int enlargement = 5;

                // Calculate enlarged polygon vertices
                for (int i = 0; i < sides; i++) {
                    double angle = i * angleStep + (shape->rotation * M_PI / 180.0);
                    enlargedPoints[i].x = shape->data.polygon.cx + cos(angle) * (shape->data.polygon.radius + enlargement);
                    enlargedPoints[i].y = shape->data.polygon.cy + sin(angle) * (shape->data.polygon.radius + enlargement);
                }
                enlargedPoints[sides] = enlargedPoints[0];

                Sint16 enlargedX[sides];
                Sint16 enlargedY[sides];
                for (int i = 0; i < sides; i++) {
                    enlargedX[i] = enlargedPoints[i].x;
                    enlargedY[i] = enlargedPoints[i].y;
                }

                if (strcmp(shape->typeForm, "filled") == 0) {
                    filledPolygonRGBA(renderer, enlargedX, enlargedY, sides,
                                    selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
                } else {
                    polygonRGBA(renderer, enlargedX, enlargedY, sides,
                            selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
                }
            }
            break;
        }

        case SHAPE_TRIANGLE: {
            setRenderColor(renderer, shape->color);

            int sides = 3;
            double angleStep = 2 * M_PI / sides;
            SDL_Point pointsP[13];

            // Calculate the vertices of the triangle with rotation applied
            for (int i = 0; i < sides; i++) {
                double angle = i * angleStep + (shape->rotation * M_PI / 180.0) + (30 * M_PI / 180.0);
                pointsP[i].x = shape->data.triangle.cx + cos(angle) * shape->data.triangle.radius;
                pointsP[i].y = shape->data.triangle.cy + sin(angle) * shape->data.triangle.radius;
            }
            pointsP[sides] = pointsP[0];

            // Render the triangle
            Sint16 xCoords[sides];
            Sint16 yCoords[sides];
            for (int i = 0; i < sides; i++) {
                xCoords[i] = pointsP[i].x;
                yCoords[i] = pointsP[i].y;
            }

            if (strcmp(shape->typeForm, "filled") == 0) {
                filledPolygonRGBA(renderer, xCoords, yCoords, sides, 
                                shape->color.r, shape->color.g, shape->color.b, shape->color.a);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                polygonRGBA(renderer, xCoords, yCoords, sides,
                        shape->color.r, shape->color.g, shape->color.b, shape->color.a);
            }

            // Add a selection highlight if the shape is selected
            if (shape->selected) {
                SDL_Color selectedColor = selectColor(shape->color);
                SDL_Point enlargedPoints[13];
                int enlargement = 5;

                // Calculate enlarged triangle vertices
                for (int i = 0; i < sides; i++) {
                    double angle = i * angleStep + (shape->rotation * M_PI / 180.0) + (30 * M_PI / 180.0);
                    enlargedPoints[i].x = shape->data.triangle.cx + cos(angle) * (shape->data.triangle.radius + enlargement);
                    enlargedPoints[i].y = shape->data.triangle.cy + sin(angle) * (shape->data.triangle.radius + enlargement);
                }
                enlargedPoints[sides] = enlargedPoints[0];

                Sint16 enlargedX[sides];
                Sint16 enlargedY[sides];
                for (int i = 0; i < sides; i++) {
                    enlargedX[i] = enlargedPoints[i].x;
                    enlargedY[i] = enlargedPoints[i].y;
                }

                if (strcmp(shape->typeForm, "filled") == 0) {
                    filledPolygonRGBA(renderer, enlargedX, enlargedY, sides,
                                    selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
                } else {
                    polygonRGBA(renderer, enlargedX, enlargedY, sides,
                            selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
                }
            }
            break;
        }

        case SHAPE_ARC: {
            setRenderColor(renderer, shape->color);

            int startAngle = shape->data.arc.start_angle % 360;
            int endAngle = shape->data.arc.end_angle % 360;
            if (startAngle < 0) startAngle += 360;
            if (endAngle < 0) endAngle += 360;

            int centerX = shape->data.arc.x;
            int centerY = shape->data.arc.y;
            startAngle = (startAngle + (int)shape->rotation) % 360;
            endAngle = (endAngle + (int)shape->rotation) % 360;

            // Render the arc
            if (strcmp(shape->typeForm, "empty") == 0) {
                arcRGBA(renderer, centerX, centerY, shape->data.arc.radius, 
                        startAngle, endAngle, 
                        shape->color.r, shape->color.g, shape->color.b, shape->color.a);
            } else if (strcmp(shape->typeForm, "filled") == 0) {
                filledPieRGBA(renderer, centerX, centerY, shape->data.arc.radius,
                            startAngle, endAngle,
                            shape->color.r, shape->color.g, shape->color.b, shape->color.a);
            }

            // Add a selection highlight if the shape is selected
            if (shape->selected) {
                SDL_Color selectedColor = selectColor(shape->color);
                int enlargement = 5;
                if (strcmp(shape->typeForm, "empty") == 0) {
                    arcRGBA(renderer, centerX, centerY, shape->data.arc.radius + enlargement,
                            startAngle, endAngle,
                            selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
                } else {
                    filledPieRGBA(renderer, centerX, centerY, shape->data.arc.radius + enlargement,
                                startAngle, endAngle,
                                selectedColor.r, selectedColor.g, selectedColor.b, selectedColor.a);
                }
            }
            break;
        }
    }
}

/**
 * @brief Renders all shapes in order of their z-index
 * 
 * @param renderer The SDL renderer to use for drawing
 */
void renderAllShapes(SDL_Renderer *renderer) {
    // Create a temporary array for sorting
    Shape** sortedShapes = malloc(shapeCount * sizeof(Shape*));
    if (!sortedShapes) {
        printf("%sExecutionError: Failed to allocate memory for sorted shapes\n", RED_COLOR);
        return;
    }

    // Copy pointers to shapes to temporary array
    for (int i = 0; i < shapeCount; i++) {
        sortedShapes[i] = &shapes[i];
    }

    // Sort shapes by z-index
    for (int i = 0; i < shapeCount - 1; i++) {
        for (int j = 0; j < shapeCount - i - 1; j++) {
            if (sortedShapes[j]->zIndex > sortedShapes[j + 1]->zIndex) {
                Shape* temp = sortedShapes[j];
                sortedShapes[j] = sortedShapes[j + 1];
                sortedShapes[j + 1] = temp;
            }
        }
    }

    // Render shapes in order
    for (int i = 0; i < shapeCount; i++) {
        renderShape(renderer, sortedShapes[i]);
    }

    // Free temporary array
    free(sortedShapes);
}

/**
 * @brief Adds a new shape to the shape list.
 * 
 * @param shape The shape to be added.
 */
void addShape(Shape shape) {
    // Check if the maximum number of shapes has been reached
    if (shapeCount >= MAX_SHAPES) {
        printf("%sExecutionError: Cannot add more shapes. Maximum limit of %d shapes reached.\n", 
               RED_COLOR, MAX_SHAPES);
        return;
    }

    // Set initial z-index to the current shape count (new shapes appear on top)
    shape.zIndex = shapeCount;

    // Initialize animation state
    shape.isAnimating = false;
    shape.zoom = 1.0f;  // Initialize zoom to 1.0 (normal size)
    shape.zoom_direction = 1.0f;  // Start with growing direction

    // Store initial values
    shape.initial_color = shape.color;
    shape.initial_rotation = shape.rotation;

    // Store initial values based on shape type (excluding position)
    switch (shape.type) {
        case SHAPE_CIRCLE:
            shape.data.circle.initial_radius = shape.data.circle.radius;
            break;
        case SHAPE_RECTANGLE:
            shape.data.rectangle.initial_width = shape.data.rectangle.width;
            shape.data.rectangle.initial_height = shape.data.rectangle.height;
            break;
        case SHAPE_SQUARE:
            shape.data.square.initial_c = shape.data.square.c;
            break;
        case SHAPE_ELLIPSE:
            shape.data.ellipse.initial_rx = shape.data.ellipse.rx;
            shape.data.ellipse.initial_ry = shape.data.ellipse.ry;
            break;
        case SHAPE_LINE:
            shape.data.line.initial_thickness = shape.data.line.thickness;
            break;
        case SHAPE_POLYGON:
            shape.data.polygon.initial_radius = shape.data.polygon.radius;
            shape.data.polygon.initial_sides = shape.data.polygon.sides;
            break;
        case SHAPE_TRIANGLE:
            shape.data.triangle.initial_radius = shape.data.triangle.radius;
            break;
        case SHAPE_ARC:
            shape.data.arc.initial_radius = shape.data.arc.radius;
            shape.data.arc.initial_start_angle = shape.data.arc.start_angle;
            shape.data.arc.initial_end_angle = shape.data.arc.end_angle;
            break;
    }

    // Add the new shape to the array and increment the shape count
    shapes[shapeCount++] = shape;
}

/**
 * @brief Deletes a shape from the shape list at the specified index.
 * 
 * @param index The index of the shape to delete.
 */
void deleteShape(int index) {
    // Check if the index is valid
    if (index < 0 || index >= shapeCount) return;

    // Shift all shapes after the specified index one position to the left
    for (int i = index; i < shapeCount - 1; i++) {
        shapes[i] = shapes[i + 1];
    }

    // Decrement the count of shapes
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
        case SHAPE_RECTANGLE: {
            // Calculate the initial aspect ratio of the rectangle
            float aspectRatio = (float)shape->data.rectangle.width / shape->data.rectangle.height;

            // Apply the zoom factor
            shape->data.rectangle.width += (int)(zoomFactor * 10);
            shape->data.rectangle.height = (int)(shape->data.rectangle.width / aspectRatio);

            // Set a minimum width to prevent the rectangle from disappearing
            const int minWidth = 10;
            const int maxWidth = 1500;
            // Check and adjust if the width falls below the minimum
            if (shape->data.rectangle.width < minWidth) {
                shape->data.rectangle.width = minWidth;
                shape->data.rectangle.height = (int)(minWidth / aspectRatio); // Adjust height proportionally
            } else if (shape->data.rectangle.width > maxWidth) {
                shape->data.rectangle.width = maxWidth;
                shape->data.rectangle.height = (int)(maxWidth / aspectRatio);
            }
            break;
        }

        case SHAPE_SQUARE: {

            // Apply the zoom factor
            shape->data.square.c += (int)(zoomFactor * 10);

            // Set a minimum to prevent the square from disappearing
            const int min = 10;
            const int max = 1500;

            // Check and adjust if the side falls below the minimum
            if (shape->data.square.c < min) {
                shape->data.square.c = min;
            } else if (shape->data.square.c > max) {
                shape->data.square.c = max;
            }
            break;
        }

        case SHAPE_CIRCLE: {
            // Adjust the radius of the circle
            shape->data.circle.radius += (int)(zoomFactor * 5);

            // Set a minimum and maximum radius
            const int min = 2;
            const int max = 1000;

            // Ensure a minimum radius to keep the circle visible
            if (shape->data.circle.radius < min) {
                shape->data.circle.radius = min;
            } else if (shape->data.circle.radius > max) {
                shape->data.circle.radius = max;
            }   
            break;
        }

        case SHAPE_ELLIPSE: {
            // Calculate the center point of the ellipse            
            int centerX = shape->data.ellipse.x;
            int centerY = shape->data.ellipse.y;

            // Store previous radii in case we need to revert
            int prevRx = shape->data.ellipse.rx;
            int prevRy = shape->data.ellipse.ry;

            // Store aspect ratio
            float aspectRatio = (float)prevRy / prevRx;

            // Apply zoom factor with smoother scaling
            float zoomMultiplier;
            if (zoomFactor > 0) {
                zoomMultiplier = 1.0f + (zoomFactor * 0.05f);
            } else {
                zoomMultiplier = 1.0f / (1.0f - (zoomFactor * 0.05f));
            }

            // Calculate new dimensions while maintaining aspect ratio
            int newRx = (int)(prevRx * zoomMultiplier);
            int newRy = (int)(newRx * aspectRatio);  // Use aspect ratio to calculate ry

            // Set reasonable limits
            const int minRadius = 2;
            const int maxRadius = 3000;

            // Only apply the new values if they're within limits
            if (newRx >= minRadius && newRx <= maxRadius) {
                shape->data.ellipse.rx = newRx;
                shape->data.ellipse.ry = newRy;
            }

            // Keep the center position fixed
            shape->data.ellipse.x = centerX;
            shape->data.ellipse.y = centerY;
            break;
        }

        case SHAPE_POLYGON: {
            // Adjust the radius of the polygon
            shape->data.polygon.radius += (int)(zoomFactor * 5);

            // Ensure a minimum radius to keep the polygon visible
            if (shape->data.polygon.radius < 2) {
                shape->data.polygon.radius = 2;
            } else if (shape->data.polygon.radius > 1000) {
                shape->data.polygon.radius = 1000;
            }
            break;
        }

        case SHAPE_TRIANGLE: {
            // Adjust the radius of the triangle
            shape->data.triangle.radius += (int)(zoomFactor * 5);

            // Ensure a minimum radius to keep the triangle visible
            if (shape->data.triangle.radius < 2) {
                shape->data.triangle.radius = 2;
            } else if (shape->data.triangle.radius > 1000) {
                shape->data.triangle.radius = 1000;
            }
            break;
        }

        case SHAPE_LINE: {
            // Calculate the center of the line
            int cx = (shape->data.line.x1 + shape->data.line.x2) / 2;
            int cy = (shape->data.line.y1 + shape->data.line.y2) / 2;

            // Adjust the endpoints to zoom in or out while preserving the center
            shape->data.line.x1 += (int)((shape->data.line.x1 - cx) * zoomFactor * 0.1);
            shape->data.line.y1 += (int)((shape->data.line.y1 - cy) * zoomFactor * 0.1);
            shape->data.line.x2 += (int)((shape->data.line.x2 - cx) * zoomFactor * 0.1);
            shape->data.line.y2 += (int)((shape->data.line.y2 - cy) * zoomFactor * 0.1);

            // Calculate the new length of the line
            int dx = shape->data.line.x2 - shape->data.line.x1;
            int dy = shape->data.line.y2 - shape->data.line.y1;
            double length = sqrt(dx * dx + dy * dy);

            // Ensure length stays between min and max values
            const double minLength = 5.0;
            const double maxLength = 1500.0;
            
            if (length < minLength) {
                // Scale up to minimum length
                double scale = minLength / length;
                shape->data.line.x1 = cx + (int)((shape->data.line.x1 - cx) * scale);
                shape->data.line.y1 = cy + (int)((shape->data.line.y1 - cy) * scale);
                shape->data.line.x2 = cx + (int)((shape->data.line.x2 - cx) * scale);
                shape->data.line.y2 = cy + (int)((shape->data.line.y2 - cy) * scale);
            } else if (length > maxLength) {
                // Scale down to maximum length
                double scale = maxLength / length;
                shape->data.line.x1 = cx + (int)((shape->data.line.x1 - cx) * scale);
                shape->data.line.y1 = cy + (int)((shape->data.line.y1 - cy) * scale);
                shape->data.line.x2 = cx + (int)((shape->data.line.x2 - cx) * scale);
                shape->data.line.y2 = cy + (int)((shape->data.line.y2 - cy) * scale);
            }
            break;
        }

        case SHAPE_ARC: {
            // Adjust the radius of the arc
            shape->data.arc.radius += (int)(zoomFactor * 5);

            // Ensure a minimum radius to avoid rendering issues
            if (shape->data.arc.radius < 50) {
                shape->data.arc.radius = 50;
            }
            else if (shape->data.arc.radius > 100) {
                shape->data.arc.radius = 100;
            }
            break;
        }
    }
}

/**
 * @brief Rotates a selected shape by the given angle.
 * 
 * @param shape The shape to rotate.
 * @param angle The angle in degrees (positive for clockwise, negative for counterclockwise).
 */
void rotateShape(Shape *shape, float angle) {
    // Only rotate the shape if it is selected
    if (!shape->selected) return;

    // Update the rotation angle
    shape->rotation += angle;

    // Normalize the rotation to stay within [0, 360)
    while (shape->rotation >= 360.0f) {
        shape->rotation -= 360.0f; // Wrap around for overflow
    }
    while (shape->rotation < 0.0f) {
        shape->rotation += 360.0f; // Wrap around for underflow
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
    // Iterate through all shapes in the scene
    for (int i = 0; i < shapeCount; i++) {
        // Check if the current shape is selected
        if (shapes[i].selected) {
            // Handle shape movement based on its type
            switch (shapes[i].type) {
                case SHAPE_RECTANGLE: {
                    // Update rectangle's top-left corner based on cursor position
                    shapes[i].data.rectangle.x = event->motion.x - shapes[i].data.rectangle.width / 2;
                    shapes[i].data.rectangle.y = event->motion.y - shapes[i].data.rectangle.height / 2;

                    // Update cursor position to the rectangle's center
                    cursor->x = shapes[i].data.rectangle.x + shapes[i].data.rectangle.width / 2;
                    cursor->y = shapes[i].data.rectangle.y + shapes[i].data.rectangle.height / 2;
                    break;
                }

                case SHAPE_SQUARE: {
                    // Update rectangle's top-left corner based on cursor position
                    shapes[i].data.square.x = event->motion.x - shapes[i].data.square.c / 2;
                    shapes[i].data.square.y = event->motion.y - shapes[i].data.square.c / 2;

                    // Update cursor position to the rectangle's center
                    cursor->x = shapes[i].data.square.x + shapes[i].data.square.c / 2;
                    cursor->y = shapes[i].data.square.y + shapes[i].data.square.c / 2;
                    break;
                }

                case SHAPE_CIRCLE: {
                    // Update circle's center to match cursor position
                    shapes[i].data.circle.x = event->motion.x;
                    shapes[i].data.circle.y = event->motion.y;

                    // Update cursor position to the circle's center
                    cursor->x = shapes[i].data.circle.x;
                    cursor->y = shapes[i].data.circle.y;
                    break;
                }

                case SHAPE_ELLIPSE: {
                    // Update ellipse's center to match cursor position
                    shapes[i].data.ellipse.x = event->motion.x;
                    shapes[i].data.ellipse.y = event->motion.y;

                    // Update cursor position to the ellipse's center
                    cursor->x = shapes[i].data.ellipse.x;
                    cursor->y = shapes[i].data.ellipse.y;
                    break;
                }

                case SHAPE_LINE: {
                    // Calculate the line's width and height
                    int lineWidth = shapes[i].data.line.x2 - shapes[i].data.line.x1;
                    int lineHeight = shapes[i].data.line.y2 - shapes[i].data.line.y1;

                    // Update both endpoints of the line based on the cursor position
                    shapes[i].data.line.x1 = event->motion.x - lineWidth / 2;
                    shapes[i].data.line.y1 = event->motion.y - lineHeight / 2;
                    shapes[i].data.line.x2 = event->motion.x + lineWidth / 2;
                    shapes[i].data.line.y2 = event->motion.y + lineHeight / 2;

                    // Update cursor position to match the current mouse position
                    cursor->x = event->motion.x;
                    cursor->y = event->motion.y;
                    break;
                }
                
                case SHAPE_POLYGON: {
                    // Update the polygon's center to match the current cursor position
                    shapes[i].data.polygon.cx = event->motion.x;
                    shapes[i].data.polygon.cy = event->motion.y;

                    // Update cursor position to the polygon's center
                    cursor->x = shapes[i].data.polygon.cx;
                    cursor->y = shapes[i].data.polygon.cy;
                    break;
                }

                case SHAPE_TRIANGLE: {
                    // Update the triangle's center to match the current cursor position
                    shapes[i].data.triangle.cx = event->motion.x;
                    shapes[i].data.triangle.cy = event->motion.y;

                    // Update cursor position to the triangle's center
                    cursor->x = shapes[i].data.triangle.cx;
                    cursor->y = shapes[i].data.triangle.cy;
                    break;
                }

                case SHAPE_ARC: {
                    // Update the arc's center to match the current cursor position
                    shapes[i].data.arc.x = event->motion.x;
                    shapes[i].data.arc.y = event->motion.y;

                    // Update cursor position to the arc's center
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
    // Iterate through all shapes
    for (int i = 0; i < shapeCount; i++) {
        // Check if the shape is selected
        if (shapes[i].selected) {
            // Move the shape based on its type
            switch (shapes[i].type) {
                case SHAPE_RECTANGLE: {
                    // Update rectangle's top-left corner by the offset
                    shapes[i].data.rectangle.x += dx;
                    shapes[i].data.rectangle.y += dy;
                    break;
                }

                case SHAPE_SQUARE: {
                    // Update rectangle's top-left corner by the offset
                    shapes[i].data.square.x += dx;
                    shapes[i].data.square.y += dy;
                    break;
                }

                case SHAPE_CIRCLE: {
                    // Update circle's center by the offset
                    shapes[i].data.circle.x += dx;
                    shapes[i].data.circle.y += dy;
                    break;
                }

                case SHAPE_ELLIPSE: {
                    // Update ellipse's center by the offset
                    shapes[i].data.ellipse.x += dx;
                    shapes[i].data.ellipse.y += dy;
                    break;
                }

                case SHAPE_LINE: {
                    // Update both endpoints of the line by the offset
                    shapes[i].data.line.x1 += dx;
                    shapes[i].data.line.y1 += dy;
                    shapes[i].data.line.x2 += dx;
                    shapes[i].data.line.y2 += dy;
                    break;
                }

                case SHAPE_POLYGON: {
                    // Update the polygon's center by the offset
                    shapes[i].data.polygon.cx += dx;
                    shapes[i].data.polygon.cy += dy; 
                    break;
                }

                case SHAPE_TRIANGLE: {
                    // Update the triangle's center by the offset
                    shapes[i].data.triangle.cx += dx;
                    shapes[i].data.triangle.cy += dy; 
                    break;
                }

                case SHAPE_ARC: {
                    // Update the arc's center by the offset
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
    // Calculate the distance between the point (x, y) and the circle's center (cx, cy).
    int dx = x - cx;
    int dy = y - cy;
    // Return true if the squared distance is less than or equal to the squared radius.
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
    // Check if the point satisfies the ellipse equation: (x-cx)^2 / rx^2 + (y-cy)^2 / ry^2 <= 1.
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
    // Calculate the distance between the point and the center.
    int dx = x - cx;
    int dy = y - cy;
    int distanceSquared = dx * dx + dy * dy;

    // Check if the point lies within the arc's radius.
    if (distanceSquared > radius * radius) {
        return 0; // Outside the arc's radius.
    }

    // Calculate the angle between the point and the center in degrees.
    double angle = atan2(dy, dx) * 180.0 / M_PI;

    // Ensure the angle is positive by adding 360 if it's negative.
    if (angle < 0) {
        angle += 360;
    }

    // Handle cases where the arc wraps around the 0-degree mark.
    if (startAngle <= endAngle) {
        // Normal case: startAngle < endAngle.
        return (angle >= startAngle && angle <= endAngle);
    } else {
        // Wrapped case: startAngle > endAngle.
        return (angle >= startAngle || angle <= endAngle);
    }
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
    // Check if the point lies within the rectangle's boundaries.
    return (x >= rectX && x <= rectX + rectW && y >= rectY && y <= rectY + rectH);
}

int isPointInSquare(int x, int y, int squareX, int squareY, int squareC) {
    // Check if the point lies within the square's boundaries.
    return (x >= squareX && x <= squareX + squareC && y >= squareY && y <= squareY + squareC);
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
    int i, j, c = 0; // `c` keeps track of crossing count (odd = inside, even = outside).
    float angle, x1, y1, x2, y2;

    // Ensure the polygon has at least three sides (minimum for a valid polygon).
    if (sides < 3) return 0;

    // Loop through each edge of the polygon.
    for (i = 0, j = sides - 1; i < sides; j = i++) {
        // Calculate the coordinates of the i-th vertex.
        angle = (2 * M_PI * i) / sides; // Angle for vertex i.
        x1 = cx + radius * cos(angle);
        y1 = cy + radius * sin(angle);

        // Calculate the coordinates of the j-th vertex (previous vertex).
        angle = (2 * M_PI * j) / sides; // Angle for vertex j.
        x2 = cx + radius * cos(angle);
        y2 = cy + radius * sin(angle);

        // Check if the point lies between the y-coordinates of the current edge.
        // Then determine if a ray extending to the right crosses this edge.
        if (((y1 > y) != (y2 > y)) && // The point's y lies between the edge's y range.
            (x < (x2 - x1) * (y - y1) / (y2 - y1) + x1)) { // Ray intersects the edge.
            c = !c; // Toggle crossing count (odd/even).
        }
    }

    return c; // Return 1 if inside (odd crossings), 0 otherwise.
}

int isPointInTriangle(int x, int y, int cx, int cy, int radius) {
    int i, j, c = 0; // `c` keeps track of crossing count (odd = inside, even = outside).
    int sides = 3;
    float angle, x1, y1, x2, y2;

    // Loop through each edge of the polygon.
    for (i = 0, j = sides - 1; i < sides; j = i++) {
        // Calculate the coordinates of the i-th vertex.
        angle = (2 * M_PI * i) / sides; // Angle for vertex i.
        x1 = cx + radius * cos(angle);
        y1 = cy + radius * sin(angle);

        // Calculate the coordinates of the j-th vertex (previous vertex).
        angle = (2 * M_PI * j) / sides; // Angle for vertex j.
        x2 = cx + radius * cos(angle);
        y2 = cy + radius * sin(angle);

        // Check if the point lies between the y-coordinates of the current edge.
        // Then determine if a ray extending to the right crosses this edge.
        if (((y1 > y) != (y2 > y)) && // The point's y lies between the edge's y range.
            (x < (x2 - x1) * (y - y1) / (y2 - y1) + x1)) { // Ray intersects the edge.
            c = !c; // Toggle crossing count (odd/even).
        }
    }

    return c; // Return 1 if inside (odd crossings), 0 otherwise.
}

/**
 * @brief Checks if a point is within a line's hitbox
 * 
 * @param x,y The point coordinates to check
 * @param x1,y1 First endpoint of the line
 * @param x2,y2 Second endpoint of the line
 * @param tolerance Distance threshold for hit detection
 * @return 1 if point is within line's hitbox, 0 otherwise
 */
int isPointInLine(int x, int y, int x1, int y1, int x2, int y2, int tolerance, int rotation) {
    // Augmenter la tolérance pour une meilleure sélection
    tolerance = tolerance + 5;

    // Calculate the center of the line
    float centerX = (x1 + x2) / 2.0f;
    float centerY = (y1 + y2) / 2.0f;

    // Convert rotation to radians
    float angle = -rotation * M_PI / 180.0f;  // Negative because we're rotating backwards

    // Translate point to origin (relative to line center)
    float translatedX = x - centerX;
    float translatedY = y - centerY;

    // Rotate point
    float rotatedX = translatedX * cos(angle) - translatedY * sin(angle);
    float rotatedY = translatedX * sin(angle) + translatedY * cos(angle);

    // Translate back
    float finalX = rotatedX + centerX;
    float finalY = rotatedY + centerY;

    // Calculer la longueur de la ligne
    float lineLength = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    if (lineLength == 0) return 0;  // Éviter division par zéro

    // Calculer les vecteurs normalisés
    float dx = (x2 - x1) / lineLength;
    float dy = (y2 - y1) / lineLength;

    // Vecteur du point de début de la ligne au point de test
    float px = finalX - x1;
    float py = finalY - y1;

    // Projection du point sur la ligne
    float projection = px * dx + py * dy;

    // Distance perpendiculaire à la ligne
    float perpDist = fabs(px * (-dy) + py * dx);

    // Le point est valide s'il est :
    // 1. À une distance perpendiculaire inférieure à la tolérance
    // 2. La projection est entre le début et la fin de la ligne
    return (perpDist <= tolerance && projection >= -tolerance && projection <= lineLength + tolerance);
}


bool isPointInShape(Shape* shape, int x, int y) {
    switch (shape->type) {
        case SHAPE_CIRCLE:
            return isPointInCircle(x, y, 
                shape->data.circle.x, 
                shape->data.circle.y, 
                shape->data.circle.radius);
            
        case SHAPE_RECTANGLE:
            return isPointInRectangle(x, y,
                shape->data.rectangle.x,
                shape->data.rectangle.y,
                shape->data.rectangle.width,
                shape->data.rectangle.height);
            
        case SHAPE_SQUARE:
            return isPointInSquare(x, y,
                shape->data.square.x,
                shape->data.square.y,
                shape->data.square.c);
            
        case SHAPE_ELLIPSE:
            return isPointInEllipse(x, y,
                shape->data.ellipse.x,
                shape->data.ellipse.y,
                shape->data.ellipse.rx,
                shape->data.ellipse.ry);
            
        case SHAPE_POLYGON:
            return isPointInPolygon(x, y,
                shape->data.polygon.cx,
                shape->data.polygon.cy,
                shape->data.polygon.radius,
                shape->data.polygon.sides);
            
        case SHAPE_TRIANGLE:
            return isPointInTriangle(x, y,
                shape->data.triangle.cx,
                shape->data.triangle.cy,
                shape->data.triangle.radius);
            
        case SHAPE_ARC:
            return isPointInArc(x, y,
                shape->data.arc.x,
                shape->data.arc.y,
                shape->data.arc.radius,
                shape->data.arc.start_angle,
                shape->data.arc.end_angle);
            
        case SHAPE_LINE:
            return isPointInLine(x, y,
                shape->data.line.x1,
                shape->data.line.y1,
                shape->data.line.x2,
                shape->data.line.y2,
                5,  // 5 pixel tolerance for hit detection
                shape->rotation);
            
        default:
            return false;
    }
}

/**
 * @brief Moves a selected shape one layer up in the z-order
 */
void moveShapeUp() {
    for (int i = 0; i < shapeCount; i++) {
        if (shapes[i].selected) {
            // Find the shape with the next highest z-index
            int currentZ = shapes[i].zIndex;
            int swapIndex = -1;
            int nextHighestZ = INT_MAX;
            
            for (int j = 0; j < shapeCount; j++) {
                if (shapes[j].zIndex > currentZ && shapes[j].zIndex < nextHighestZ) {
                    nextHighestZ = shapes[j].zIndex;
                    swapIndex = j;
                }
            }
            
            // If we found a shape to swap with, swap their z-indices
            if (swapIndex != -1) {
                int temp = shapes[i].zIndex;
                shapes[i].zIndex = shapes[swapIndex].zIndex;
                shapes[swapIndex].zIndex = temp;
            }
            break;
        }
    }
}

/**
 * @brief Moves a selected shape one layer down in the z-order
 */
void moveShapeDown() {
    for (int i = 0; i < shapeCount; i++) {
        if (shapes[i].selected) {
            // Find the shape with the next lowest z-index
            int currentZ = shapes[i].zIndex;
            int swapIndex = -1;
            int nextLowestZ = -1;
            
            for (int j = 0; j < shapeCount; j++) {
                if (shapes[j].zIndex < currentZ && shapes[j].zIndex > nextLowestZ) {
                    nextLowestZ = shapes[j].zIndex;
                    swapIndex = j;
                }
            }
            
            // If we found a shape to swap with, swap their z-indices
            if (swapIndex != -1) {
                int temp = shapes[i].zIndex;
                shapes[i].zIndex = shapes[swapIndex].zIndex;
                shapes[swapIndex].zIndex = temp;
            }
            break;
        }
    }
}

/**
 * @brief Deletes the currently selected shape.
 */
void deleteSelectedShape() {
    for (int i = 0; i < shapeCount; i++) {
        if (shapes[i].selected) {
            deleteShape(i);
            break;
        }
    }
}

/**
 * @brief Toggles animation on the selected shape if it's a rotation animation
 */
void toggleAnimation() {
    for (int i = 0; i < shapeCount; i++) {
        if (shapes[i].selected) {
            shapes[i].selected = false;
            shapes[i].isAnimating = !shapes[i].isAnimating;
            break;
        }
    }
}

/**
 * @brief Resets a shape to its initial state
 * 
 * @param shape Pointer to the shape to reset
 */
void resetShape(Shape *shape) {
    // Reset common properties
    shape->color = shape->initial_color;
    shape->rotation = shape->initial_rotation;
    shape->isAnimating = false;
    shape->zoom = 1.0f;
    shape->zoom_direction = 1.0f;
    shape->color_phase = 0.0f;
    shape->bounce_velocity = 0.0f;

    // Reset shape-specific properties (excluding position)
    switch (shape->type) {
        case SHAPE_CIRCLE:
            shape->data.circle.radius = shape->data.circle.initial_radius;
            break;
        case SHAPE_RECTANGLE:
            shape->data.rectangle.width = shape->data.rectangle.initial_width;
            shape->data.rectangle.height = shape->data.rectangle.initial_height;
            break;
        case SHAPE_SQUARE:
            shape->data.square.c = shape->data.square.initial_c;
            break;
        case SHAPE_ELLIPSE:
            shape->data.ellipse.rx = shape->data.ellipse.initial_rx;
            shape->data.ellipse.ry = shape->data.ellipse.initial_ry;
            break;
        case SHAPE_LINE:
            shape->data.line.thickness = shape->data.line.initial_thickness;
            break;
        case SHAPE_POLYGON:
            shape->data.polygon.radius = shape->data.polygon.initial_radius;
            shape->data.polygon.sides = shape->data.polygon.initial_sides;
            break;
        case SHAPE_TRIANGLE:
            shape->data.triangle.radius = shape->data.triangle.initial_radius;
            break;
        case SHAPE_ARC:
            shape->data.arc.radius = shape->data.arc.initial_radius;
            shape->data.arc.start_angle = shape->data.arc.initial_start_angle;
            shape->data.arc.end_angle = shape->data.arc.initial_end_angle;
            break;
    }
}

/**
 * @brief Moves a shape by the specified delta x and y.
 * 
 * @param shape Pointer to the shape to move
 * @param dx Change in x position
 * @param dy Change in y position
 */
void moveShape(Shape *shape, int dx, int dy) {
    if (!shape) return;

    switch (shape->type) {
        case SHAPE_CIRCLE:
            shape->data.circle.x += dx;
            shape->data.circle.y += dy;
            break;
        case SHAPE_RECTANGLE:
            shape->data.rectangle.x += dx;
            shape->data.rectangle.y += dy;
            break;
        case SHAPE_SQUARE:
            shape->data.square.x += dx;
            shape->data.square.y += dy;
            break;
        case SHAPE_ELLIPSE:
            shape->data.ellipse.x += dx;
            shape->data.ellipse.y += dy;
            break;
        case SHAPE_LINE:
            shape->data.line.x1 += dx;
            shape->data.line.y1 += dy;
            shape->data.line.x2 += dx;
            shape->data.line.y2 += dy;
            break;
        case SHAPE_POLYGON:
            shape->data.polygon.cx += dx;
            shape->data.polygon.cy += dy;
            break;
        case SHAPE_TRIANGLE:
            shape->data.triangle.cx += dx;
            shape->data.triangle.cy += dy;
            break;
        case SHAPE_ARC:
            shape->data.arc.x += dx;
            shape->data.arc.y += dy;
            break;
    }
}