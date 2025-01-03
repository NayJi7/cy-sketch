#include "../files.h/formEvents.h"
#include "../files.h/cursorEvents.h"

#include <math.h>

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
void setRenderColor(SDL_Renderer* renderer, Uint32 color) {
    static Uint32 currentColor = 0xFFFFFFFF; // Default color is white
    if (currentColor != color) {
        currentColor = color;

        // Extract the RGBA components from the 32-bit color value
        Uint8 r = (color >> 24) & 0xFF;
        Uint8 g = (color >> 16) & 0xFF;
        Uint8 b = (color >> 8) & 0xFF;
        Uint8 a = color & 0xFF;

        // Set the draw color in the renderer
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
    }
}

/**
 * @brief Adjusts the brightness of a given color.
 *
 * This function modifies the RGB components of the input color by multiplying them
 * by a brightness factor. The alpha component remains unchanged.
 * 
 * @param color The original 32-bit RGBA color.
 * @param factor A scaling factor for the brightness (e.g., 0.5 for 50% darker, 1.5 for 50% brighter).
 * @return The adjusted color as a 32-bit RGBA value.
 */
Uint32 adjustBrightness(Uint32 color, float factor) {
    // Extract RGBA components from the color
    Uint8 r = (color >> 24) & 0xFF;
    Uint8 g = (color >> 16) & 0xFF;
    Uint8 b = (color >> 8) & 0xFF;
    Uint8 a = color & 0xFF;

    // Adjust the RGB components using the factor, clamping to the range [0, 255]
    r = (Uint8)fminf(255, fmaxf(0, r * factor));
    g = (Uint8)fminf(255, fmaxf(0, g * factor));
    b = (Uint8)fminf(255, fmaxf(0, b * factor));

    // Combine the adjusted components back into a 32-bit RGBA color
    return (r << 24) | (g << 16) | (b << 8) | a;
}


/**
 * @brief Renders the content of a texture onto the screen and handles optional delays.
 * 
 * @param renderer Renderer used for drawing.
 * @param texture Texture to be rendered.
 * @param time Delay in milliseconds after rendering (set to 0 for no delay).
 */
void renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, int time) {
    SDL_SetRenderTarget(renderer, NULL); // Reset the render target to the screen
    SDL_RenderCopy(renderer, texture, NULL, NULL); // Copy the texture to the renderer
    if (time != 0) SDL_Delay(time); // Optional delay
    SDL_RenderPresent(renderer); // Present the rendered content to the screen
    SDL_SetRenderTarget(renderer, texture); // Restore the texture as the render target
}

/**
 * @brief Renders a shape on the screen based on its type and properties.
 * 
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
                filledCircleColor(renderer, shape->data.circle.x, shape->data.circle.y, shape->data.circle.radius, shape->color);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                circleColor(renderer, shape->data.circle.x, shape->data.circle.y, shape->data.circle.radius, shape->color);
            }

            // Highlight the circle if selected
            if (shape->selected) {
                Uint32 highlightColor = adjustBrightness(shape->color, 0.8); // Darken the color by 20%.
                circleColor(renderer, shape->data.circle.x, shape->data.circle.y, shape->data.circle.radius + 5, highlightColor);
            }

            // Draw an indicator line for rotation
            double angle1 = shape->rotation * M_PI / 180.0; // Convert angle to radians.
            int endX = shape->data.circle.x + cos(angle1) * shape->data.circle.radius;
            int endY = shape->data.circle.y + sin(angle1) * shape->data.circle.radius;

            setRenderColor(renderer, red); // Use red for the indicator line.
            SDL_RenderDrawLine(renderer, shape->data.circle.x, shape->data.circle.y, endX, endY);
            break;
        }
        
        case SHAPE_RECTANGLE: {
            setRenderColor(renderer, shape->color);

            // Calculate the center of the rectangle
            int cx = shape->data.rectangle.x + shape->data.rectangle.width / 2;
            int cy = shape->data.rectangle.y + shape->data.rectangle.height / 2;
            double angle = shape->rotation * M_PI / 180.0;

            SDL_Point pointsR[5];  // Array to store rotated rectangle vertices
            SDL_Point selectionPoints[5];  // Array for the selection contour
            int offset = 25;  // Extra space around the rectangle for the contour

            // Calculate rotated rectangle vertices
            pointsR[0].x = cos(angle) * (shape->data.rectangle.x - cx) - sin(angle) * (shape->data.rectangle.y - cy) + cx;
            pointsR[0].y = sin(angle) * (shape->data.rectangle.x - cx) + cos(angle) * (shape->data.rectangle.y - cy) + cy;

            pointsR[1].x = cos(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) - sin(angle) * (shape->data.rectangle.y - cy) + cx;
            pointsR[1].y = sin(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) + cos(angle) * (shape->data.rectangle.y - cy) + cy;

            pointsR[2].x = cos(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) - sin(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cx;
            pointsR[2].y = sin(angle) * (shape->data.rectangle.x + shape->data.rectangle.width - cx) + cos(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cy;

            pointsR[3].x = cos(angle) * (shape->data.rectangle.x - cx) - sin(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cx;
            pointsR[3].y = sin(angle) * (shape->data.rectangle.x - cx) + cos(angle) * (shape->data.rectangle.y + shape->data.rectangle.height - cy) + cy;

            pointsR[4] = pointsR[0];  // Close the rectangle by repeating the first point.

            // Calculate selection contour points
            for (int i = 0; i < 4; i++) {
                double dx = pointsR[i].x - cx;
                double dy = pointsR[i].y - cy;
                double length = sqrt(dx * dx + dy * dy);

                selectionPoints[i].x = cx + (dx / length) * (length + offset);
                selectionPoints[i].y = cy + (dy / length) * (length + offset);
            }
            selectionPoints[4] = selectionPoints[0];  // Close the contour.

            // Render the rectangle
            if (strcmp(shape->typeForm, "filled") == 0) {
                filledPolygonColor(renderer, 
                                (Sint16[]){pointsR[0].x, pointsR[1].x, pointsR[2].x, pointsR[3].x}, 
                                (Sint16[]){pointsR[0].y, pointsR[1].y, pointsR[2].y, pointsR[3].y}, 
                                4, shape->color);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                SDL_RenderDrawLines(renderer, pointsR, 5);
            }

            // Render selection highlight if selected
            if (shape->selected) {
                if (strcmp(shape->typeForm, "filled") == 0) {
                    filledPolygonColor(renderer, 
                                    (Sint16[]){pointsR[0].x, pointsR[1].x, pointsR[2].x, pointsR[3].x}, 
                                    (Sint16[]){pointsR[0].y, pointsR[1].y, pointsR[2].y, pointsR[3].y}, 
                                    4, yellow);
                } else if (strcmp(shape->typeForm, "empty") == 0) {
                    polygonColor(renderer, 
                                (Sint16[]){pointsR[0].x, pointsR[1].x, pointsR[2].x, pointsR[3].x}, 
                                (Sint16[]){pointsR[0].y, pointsR[1].y, pointsR[2].y, pointsR[3].y}, 
                                4, yellow);
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
                filledPolygonColor(renderer, xCoords, yCoords, pointCount, shape->color);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                SDL_RenderDrawLines(renderer, ellipsePoints, pointCount);
                SDL_RenderDrawLine(renderer, ellipsePoints[pointCount - 1].x, ellipsePoints[pointCount - 1].y, ellipsePoints[0].x, ellipsePoints[0].y);
            }

            // Render selection if the shape is selected.
            if (shape->selected) {
                SDL_Point selectionPoints[36];
                for (int i = 0; i < pointCount; i++) {
                    double theta = (2 * M_PI * i) / pointCount;
                    double dx = (shape->data.ellipse.rx + 5) * cos(theta);
                    double dy = (shape->data.ellipse.ry + 5) * sin(theta);
                    selectionPoints[i].x = cos(angleE) * dx - sin(angleE) * dy + cxE;
                    selectionPoints[i].y = sin(angleE) * dx + cos(angleE) * dy + cyE;
                }
                setRenderColor(renderer, blue);
                SDL_RenderDrawLines(renderer, selectionPoints, pointCount);
                SDL_RenderDrawLine(renderer, selectionPoints[pointCount - 1].x, selectionPoints[pointCount - 1].y, selectionPoints[0].x, selectionPoints[0].y);
            }
            break;
        }

        case SHAPE_LINE: {
            setRenderColor(renderer, shape->color);

            int x1 = shape->data.line.x1;
            int y1 = shape->data.line.y1;
            int x2 = shape->data.line.x2;
            int y2 = shape->data.line.y2;
            int thickness = shape->data.line.thickness;

            // Apply rotation if necessary.
            if (shape->rotation != 0) {
                int cx = (shape->data.line.x1 + shape->data.line.x2) / 2; // Center of rotation.
                int cy = (shape->data.line.y1 + shape->data.line.y2) / 2;
                double angle = shape->rotation * M_PI / 180.0;

                // Calculate rotated points.
                int rotatedX1 = cos(angle) * (shape->data.line.x1 - cx) - sin(angle) * (shape->data.line.y1 - cy) + cx;
                int rotatedY1 = sin(angle) * (shape->data.line.x1 - cx) + cos(angle) * (shape->data.line.y1 - cy) + cy;
                int rotatedX2 = cos(angle) * (shape->data.line.x2 - cx) - sin(angle) * (shape->data.line.y2 - cy) + cx;
                int rotatedY2 = sin(angle) * (shape->data.line.x2 - cx) + cos(angle) * (shape->data.line.y2 - cy) + cy;

                x1 = rotatedX1;
                y1 = rotatedY1;
                x2 = rotatedX2;
                y2 = rotatedY2;
            }

            // Render the line.
            thickLineColor(renderer, x1, y1, x2, y2, thickness, shape->color);

            // Highlight if the line is selected.
            if (shape->selected) {
                SDL_SetRenderDrawColor(renderer, 0, 139, 0, 255); // Yellow.
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            }
            break;
        }


        case SHAPE_ROUNDED_RECTANGLE: {
            setRenderColor(renderer, shape->color);

            // Calculate the center of the rounded rectangle
            int centerX = (shape->data.rounded_rectangle.x1 + shape->data.rounded_rectangle.x2) / 2;
            int centerY = (shape->data.rounded_rectangle.y1 + shape->data.rounded_rectangle.y2) / 2;
            double angle = shape->rotation * M_PI / 180.0; // Convert rotation to radians

            // Calculate the dimensions
            int width = shape->data.rounded_rectangle.x2 - shape->data.rounded_rectangle.x1;
            int height = shape->data.rounded_rectangle.y2 - shape->data.rounded_rectangle.y1;
            int radius = shape->data.rounded_rectangle.radius;

            // Calculate the four corners after rotation
            SDL_Point corners[4];
            corners[0].x = cos(angle) * (-width / 2) - sin(angle) * (-height / 2) + centerX;
            corners[0].y = sin(angle) * (-width / 2) + cos(angle) * (-height / 2) + centerY;

            corners[1].x = cos(angle) * (width / 2) - sin(angle) * (-height / 2) + centerX;
            corners[1].y = sin(angle) * (width / 2) + cos(angle) * (-height / 2) + centerY;

            corners[2].x = cos(angle) * (width / 2) - sin(angle) * (height / 2) + centerX;
            corners[2].y = sin(angle) * (width / 2) + cos(angle) * (height / 2) + centerY;

            corners[3].x = cos(angle) * (-width / 2) - sin(angle) * (height / 2) + centerX;
            corners[3].y = sin(angle) * (-width / 2) + cos(angle) * (height / 2) + centerY;

            // Draw the rounded rectangle
            if (strcmp(shape->typeForm, "filled") == 0) {
                // Use `roundedBoxColor` to draw the filled rounded rectangle
                roundedBoxColor(renderer,
                                corners[0].x, corners[0].y,
                                corners[2].x, corners[2].y,
                                radius,
                                shape->color);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                // Use `roundedRectangleColor` to draw the outline
                roundedRectangleColor(renderer,
                                    corners[0].x, corners[0].y,
                                    corners[2].x, corners[2].y,
                                    radius,
                                    shape->color);
            }

            // Add a selection highlight if the shape is selected
            if (shape->selected) {
                SDL_SetRenderDrawColor(renderer, 240, 0, 130, 255); // Highlight color
                if (strcmp(shape->typeForm, "filled") == 0) {
                    roundedBoxColor(renderer,
                                    corners[0].x, corners[0].y,
                                    corners[2].x, corners[2].y,
                                    radius,
                                    yellow); // Use `yellow` for highlight
                } else if (strcmp(shape->typeForm, "empty") == 0) {
                    roundedRectangleColor(renderer,
                                        corners[0].x, corners[0].y,
                                        corners[2].x, corners[2].y,
                                        radius,
                                        yellow);
                }
            }

            break;
        }


        case SHAPE_POLYGON: {
            setRenderColor(renderer, shape->color);

            int sides = shape->data.polygon.sides;
            if (sides < 3) return; // Do not draw if the number of sides is less than 3.

            double angleStep = 2 * M_PI / sides; // Calculate the angle between each vertex.
            SDL_Point pointsP[13]; // Array to store the vertices of the polygon.

            // Calculate the vertices of the polygon with rotation applied.
            for (int i = 0; i < sides; i++) {
                double angle = i * angleStep + (shape->rotation * M_PI / 180.0); // Add rotation to each vertex.
                pointsP[i].x = shape->data.polygon.cx + cos(angle) * shape->data.polygon.radius; // X-coordinate.
                pointsP[i].y = shape->data.polygon.cy + sin(angle) * shape->data.polygon.radius; // Y-coordinate.
            }
            pointsP[sides] = pointsP[0]; // Close the polygon by connecting the last vertex to the first.

            // Render the polygon.
            if (strcmp(shape->typeForm, "filled") == 0) {
                Sint16 xCoords[sides];
                Sint16 yCoords[sides];
                for (int i = 0; i < sides; i++) {
                    xCoords[i] = pointsP[i].x;
                    yCoords[i] = pointsP[i].y;
                }
                filledPolygonColor(renderer, xCoords, yCoords, sides, shape->color); // Draw a filled polygon.
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                SDL_RenderDrawLines(renderer, pointsP, sides + 1); // Draw the outline of the polygon.
            }

            // Add a color highlight if the polygon is selected.
            if (shape->selected) {
                Sint16 xCoords[sides];
                Sint16 yCoords[sides];
                for (int i = 0; i < sides; i++) {
                    xCoords[i] = pointsP[i].x;
                    yCoords[i] = pointsP[i].y;
                }

                if (strcmp(shape->typeForm, "filled") == 0) {
                    filledPolygonColor(renderer, xCoords, yCoords, sides, yellow); // Highlight the filled polygon.
                } else if (strcmp(shape->typeForm, "empty") == 0) {
                    polygonColor(renderer, xCoords, yCoords, sides, yellow); // Highlight the outline.
                }
            }
            break;
        }

        case SHAPE_ARC: {
            setRenderColor(renderer, shape->color);

            // Ensure start and end angles are within valid bounds.
            int startAngle = shape->data.arc.start_angle % 360;
            int endAngle = shape->data.arc.end_angle % 360;
            if (startAngle < 0) startAngle += 360;
            if (endAngle < 0) endAngle += 360;

            // Apply rotation to the arc.
            int centerX = shape->data.arc.x; // X-coordinate of the arc's center.
            int centerY = shape->data.arc.y; // Y-coordinate of the arc's center.
            double rotationRad = shape->rotation * M_PI / 180.0; // Convert rotation to radians.

            // Adjust start and end angles based on the rotation.
            startAngle = (startAngle + (int)shape->rotation) % 360;
            endAngle = (endAngle + (int)shape->rotation) % 360;

            // Render the arc.
            if (strcmp(shape->typeForm, "empty") == 0) {
                arcColor(renderer, centerX, centerY, shape->data.arc.radius, startAngle, endAngle, shape->color); // Draw an arc outline.
            } else if (strcmp(shape->typeForm, "filled") == 0) {
                filledPieColor(renderer, centerX, centerY, shape->data.arc.radius, startAngle, endAngle, shape->color); // Draw a filled arc.
            }

            // Add a color highlight if the arc is selected.
            if (shape->selected) {
                if (strcmp(shape->typeForm, "empty") == 0) {
                    arcColor(renderer, centerX, centerY, shape->data.arc.radius, startAngle, endAngle, yellow); // Highlight the outline.
                } else if (strcmp(shape->typeForm, "filled") == 0) {
                    filledPieColor(renderer, centerX, centerY, shape->data.arc.radius, startAngle, endAngle, yellow); // Highlight the filled arc.
                }
            }
            break;
        }
    }
}

/**
 * @brief Adds a new shape to the shape list.
 * 
 * @param shape The shape to be added.
 */
void addShape(Shape shape) {
    // Check if the maximum number of shapes has been reached
    if (shapeCount >= MAX_SHAPES) {
        printf("Error : Maximum number of shapes reached.\n");
        return;
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

            // Check and adjust if the width falls below the minimum
            if (shape->data.rectangle.width < minWidth) {
                shape->data.rectangle.width = minWidth;
                shape->data.rectangle.height = (int)(minWidth / aspectRatio); // Adjust height proportionally
            }

            // Optionally, also enforce a minimum height
            const int minHeight = 10;
            if (shape->data.rectangle.height < minHeight) {
                shape->data.rectangle.height = minHeight;
                shape->data.rectangle.width = (int)(minHeight * aspectRatio); // Adjust width proportionally
            }
            break;
        }

        case SHAPE_CIRCLE: {
            // Increase the radius based on the zoom factor
            shape->data.circle.radius += (int)(zoomFactor * 5);

            // Ensure a minimum radius to prevent rendering issues
            if (shape->data.circle.radius < 20) shape->data.circle.radius = 20;
            break;
        }

        case SHAPE_ELLIPSE: {
            // Increase the radii based on the zoom factor
            shape->data.ellipse.rx += (int)(zoomFactor * 5);
            shape->data.ellipse.ry += (int)(zoomFactor * 5);

            // Ensure minimum values for the radii to avoid flattening the ellipse
            // Preserve the aspect ratio to prevent the ellipse from becoming a perfect circle
            if (shape->data.ellipse.rx < 20) {
                shape->data.ellipse.rx = 20;
                shape->data.ellipse.ry = 20 * 2; // Example: maintain a 1:2 ratio
            } else if (shape->data.ellipse.ry < 20) {
                shape->data.ellipse.ry = 20;
                shape->data.ellipse.rx = 20 * 2; // Example: maintain a 2:1 ratio
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

            // Ensure a minimum length to keep the line visible
            const double minLength = 50.0;
            if (length < minLength) {
                double scale = minLength / length;
                shape->data.line.x1 = cx + (int)((shape->data.line.x1 - cx) * scale);
                shape->data.line.y1 = cy + (int)((shape->data.line.y1 - cy) * scale);
                shape->data.line.x2 = cx + (int)((shape->data.line.x2 - cx) * scale);
                shape->data.line.y2 = cy + (int)((shape->data.line.y2 - cy) * scale);
            }
            break;
        }

        case SHAPE_ROUNDED_RECTANGLE: {
            // Calculate the current dimensions
            int width = shape->data.rounded_rectangle.x2 - shape->data.rounded_rectangle.x1;
            int height = shape->data.rounded_rectangle.y2 - shape->data.rounded_rectangle.y1;

            // Define minimum allowed dimensions to avoid collapse
            const int minDimension = 40;
            
            if (zoomFactor < 0 && (width <= minDimension || height <= minDimension)) {
                break; // Skip zooming if dezoom would shrink dimensions below minimum
            }

            // Apply zoom, ensuring the dimensions stay above the minimum
            int newWidth = width + (int)(width * 0.05f * zoomFactor);
            int newHeight = height + (int)(height * 0.05f * zoomFactor);
            if (newWidth < minDimension) newWidth = minDimension;
            if (newHeight < minDimension) newHeight = minDimension;

            // Calculate the center
            int centerX = (shape->data.rounded_rectangle.x1 + shape->data.rounded_rectangle.x2) / 2;
            int centerY = (shape->data.rounded_rectangle.y1 + shape->data.rounded_rectangle.y2) / 2;
            
            
            // Update the coordinates to maintain the center
            shape->data.rounded_rectangle.x1 = centerX - newWidth / 2;
            shape->data.rounded_rectangle.y1 = centerY - newHeight / 2;
            shape->data.rounded_rectangle.x2 = centerX + newWidth / 2;
            shape->data.rounded_rectangle.y2 = centerY + newHeight / 2;

            // Adjust the corner radius proportionally
            shape->data.rounded_rectangle.radius = (int)(newHeight * 0.2f);
            if (shape->data.rounded_rectangle.radius < 10) {
                shape->data.rounded_rectangle.radius = 10; // Set a minimum radius
            }
            // Prevent further dezooming if dimensions are at their minimum
            
            break;
        }








        case SHAPE_POLYGON: {
            // Adjust the radius of the polygon
            shape->data.polygon.radius += (int)(zoomFactor * 5);

            // Ensure a minimum radius to keep the polygon visible
            if (shape->data.polygon.radius < 35) shape->data.polygon.radius = 35;
            break;
        }

        case SHAPE_ARC: {
            // Adjust the radius of the arc
            shape->data.arc.radius += (int)(zoomFactor * 5);

            // Ensure a minimum radius to avoid rendering issues
            if (shape->data.arc.radius < 30) {
                shape->data.arc.radius = 30;
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
void rotateShape(Shape *shape, int angle) {
    // Only rotate the shape if it is selected
    if (!shape->selected) return;

    // Update the rotation angle
    shape->rotation += angle;

    // Normalize the rotation to stay within [0, 360)
    if (shape->rotation >= 360) {
        shape->rotation -= 360; // Wrap around for overflow
    } else if (shape->rotation < 0) {
        shape->rotation += 360; // Wrap around for underflow
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

                case SHAPE_ROUNDED_RECTANGLE: {
                    // Get the relative movement of the mouse
                    int dx = event->motion.xrel;
                    int dy = event->motion.yrel;

                    // Update the rectangle's corners based on relative movement
                    shapes[i].data.rounded_rectangle.x1 += dx;
                    shapes[i].data.rounded_rectangle.y1 += dy;
                    shapes[i].data.rounded_rectangle.x2 += dx;
                    shapes[i].data.rounded_rectangle.y2 += dy;

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

                case SHAPE_ROUNDED_RECTANGLE: {
                    // Update the rounded rectangle's corners by the offset
                    shapes[i].data.rounded_rectangle.x1 += dx;
                    shapes[i].data.rounded_rectangle.y1 += dy;
                    shapes[i].data.rounded_rectangle.x2 += dx;
                    shapes[i].data.rounded_rectangle.y2 += dy;
                    break;
                }

                case SHAPE_POLYGON: {
                    // Update the polygon's center by the offset
                    shapes[i].data.polygon.cx += dx;
                    shapes[i].data.polygon.cy += dy; 
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
int isPointInRoundedRectangle(Sint16 px, Sint16 py, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 radius) {
    // Ensure that the coordinates of the rectangle are in the correct order.
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

    // Check if the point lies in the central rectangular zone.
    if (px >= x1 + radius && px <= x2 - radius && py >= y1 && py <= y2) {
        return 1;
    }

    // Check if the point lies in the vertical rectangular zones.
    if (px >= x1 && px <= x2 && py >= y1 + radius && py <= y2 - radius) {
        return 1;
    }

    // Check if the point lies inside the rounded corners.
    return (isPointInCircle(px, py, x1 + radius, y1 + radius, radius) ||
            isPointInCircle(px, py, x2 - radius, y1 + radius, radius) ||
            isPointInCircle(px, py, x1 + radius, y2 - radius, radius) ||
            isPointInCircle(px, py, x2 - radius, y2 - radius, radius));
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
    // Calculate the length of the line segment.
    float lineLength = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));

    // Calculate the perpendicular distance from the point to the line.
    float distance = fabs((float)(y2 - y1) * x - (float)(x2 - x1) * y + (float)(x2 * y1) - (float)(y2 * x1)) / lineLength;

    // Return true if the distance is within the specified tolerance.
    return (distance <= tolerance);
}
