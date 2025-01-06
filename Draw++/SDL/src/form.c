#include "../files.h/form.h"

// ANSI escape codes for colors
#define RED_COLOR "-#red "

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
int drawCircle(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int radius, SDL_Color color, char *type) {
    // Check if the type is valid ("filled" or "empty")
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("%sExecutionError: Invalid instantCircle type: \"%s\". Must be filled or empty.\n", 
               RED_COLOR, type);
        return -1;
    }

    // Set the drawing color
    setRenderColor(renderer, color);

    // Handle events to allow user interaction during rendering
    if (handleEvents(renderer, texture) == -1) return -1;

    // Draw the circle based on the type
    if (strcmp(type, "empty") == 0) {
        if (circleRGBA(renderer, x, y, radius, color.r, color.g, color.b, color.a) != 0) {
            printf("%sExecutionError: Failed to draw empty circle.\n", 
                   RED_COLOR);
            return -1;
        }
    } else if (strcmp(type, "filled") == 0) {
        if (filledCircleRGBA(renderer, x, y, radius, color.r, color.g, color.b, color.a) != 0) {
            printf("%sExecutionError: Failed to draw filled circle.\n", 
                   RED_COLOR);
            return -1;
        }
    }

    // Render the texture to the screen with a delay
    if (renderTexture(renderer, texture, 750) == -1) {
        printf("%sExecutionError: Failed to render circle to texture.\n", 
               RED_COLOR);
        return -1;
    }

    // Handle events again to check for user actions
    if (handleEvents(renderer, texture) == -1) return -1;

    // Reset the rendering target to default
    if (SDL_SetRenderTarget(renderer, NULL) != 0) {
        printf("%sExecutionError: Failed to reset render target.\n", 
               RED_COLOR);
        return -1;
    }

    return 0;
}


/**
 * @brief Draws an ellipse on the SDL renderer.
 * 
 * @param x The x-coordinate of the ellipse's center.
 * @param y The y-coordinate of the ellipse's center.
 * @param rx The horizontal radius of the ellipse.
 * @param ry The vertical radius of the ellipse.
 */
int drawEllipse(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int rx, int ry, SDL_Color color, char *type) {
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantEllipse type: \"%s.\". Must be filled or empty.\n", type);
        return -1;
    }
    else
    {

        if (handleEvents(renderer, texture) == -1) return -1;

        if (strcmp(type, "empty") == 0) {
            if (ellipseRGBA(renderer, x, y, rx, ry, color.r, color.g, color.b, color.a) != 0) {
                printf("%sExecutionError: Failed to draw empty ellipse.\n", RED_COLOR);
                return -1;
            }
        } else if (strcmp(type, "filled") == 0) {
            if (filledEllipseRGBA(renderer, x, y, rx, ry, color.r, color.g, color.b, color.a) != 0) {
                printf("%sExecutionError: Failed to draw filled ellipse.\n", RED_COLOR);
                return -1;
            }
        } 
        
        renderTexture(renderer, texture, 750);
        if (handleEvents(renderer, texture) == -1) return -1;
        SDL_SetRenderTarget(renderer, NULL); 
    }  
    return 0;
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
int drawArc(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int radius, int start_angle, int end_angle, SDL_Color color, char *type) {

    if (handleEvents(renderer, texture) == -1) return -1;

    if (strcmp(type, "filled") == 0) {
        // Convert angles to radians
        double startRad = start_angle * M_PI / 180.0;
        double endRad = end_angle * M_PI / 180.0;

        setRenderColor(renderer, color);

        // Draw the filled arc by iterating over concentric radius and angles
        for (int r = 0; r <= radius; ++r) { 
            for (double angle = startRad; angle <= endRad; angle += 0.01) { // Iterate on each angle
                int px = x + (int)(r * cos(angle));
                int py = y + (int)(r * sin(angle));
                SDL_RenderDrawLine(renderer, x, y, px, py);
            }
        }
    } else if (strcmp(type, "empty") == 0) {
        if (arcRGBA(renderer, x, y, radius, start_angle, end_angle, color.r, color.g, color.b, color.a) != 0) {
            printf("%sExecutionError: Failed to draw arc.\n", RED_COLOR);
            return -1;
        }
    }

    renderTexture(renderer, texture, 750);
    if (handleEvents(renderer, texture) == -1) return -1;
    SDL_SetRenderTarget(renderer, NULL); 
    return 0;
}


/**
 * @brief Draws a rectangle on the SDL renderer.
 * 
 * @param x The x-coordinate of the top-left corner.
 * @param y The y-coordinate of the top-left corner.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 */
int drawRectangle(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int w, int h, SDL_Color color, char *type) {
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantRectangle type: \"%s.\". Must be filled or empty.\n", type);
        return -1;
    }
    else
    {
        setRenderColor(renderer, color);

        // Create a SDL_Rect structure for the rectangle
        SDL_Rect rect = {x, y, w, h};

        if (handleEvents(renderer, texture) == -1) return -1;

        if (strcmp(type, "empty") == 0) {
            if (SDL_RenderDrawRect(renderer, &rect) != 0) {
                printf("%sExecutionError: Failed to draw empty rectangle.\n", RED_COLOR);
            }
        } else if (strcmp(type, "filled") == 0) {
            if (SDL_RenderFillRect(renderer, &rect) != 0) {
                printf("%sExecutionError: Failed to draw filled rectangle.\n", RED_COLOR);
            }
        } 
        renderTexture(renderer, texture, 750);
        if (handleEvents(renderer, texture) == -1) return -1;  
        SDL_SetRenderTarget(renderer, NULL);
    }
    return 0;
}

int drawSquare(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int c, SDL_Color color, char *type) {
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid instantSquare type: \"%s.\". Must be filled or empty.\n", type);
        return -1;
    }
    else
    {
        setRenderColor(renderer, color);

        // Create a SDL_Rect structure for the rectangle
        SDL_Rect rect = {x, y, c, c};

        if (handleEvents(renderer, texture) == -1) return -1;

        if (strcmp(type, "empty") == 0) {
            if (SDL_RenderDrawRect(renderer, &rect) != 0) {
                printf("%sExecutionError: Failed to draw empty square.\n", RED_COLOR);
            }
        } else if (strcmp(type, "filled") == 0) {
            if (SDL_RenderFillRect(renderer, &rect) != 0) {
                printf("%sExecutionError: Failed to draw filled square.\n", RED_COLOR);
            }
        } 
        renderTexture(renderer, texture, 750);
        if (handleEvents(renderer, texture) == -1) return -1;  
        SDL_SetRenderTarget(renderer, NULL);
    }
    return 0;
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
int drawRoundedRectangle(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, SDL_Color color, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("%sExecutionError: Invalid type for rounded rectangle '%s'. Must be 'filled' or 'empty'.\n", 
               RED_COLOR, type);
        return -1;
    }
    else
    {
       
        if (handleEvents(renderer, texture) == -1) return -1;

        if (strcmp(type, "empty") == 0) {
            if (roundedRectangleRGBA(renderer, x1, y1, x2, y2, rad, color.r, color.g, color.b, color.a) != 0) {
                printf("%sExecutionError: Failed to draw empty rounded rectangle.\n", RED_COLOR);
                return -1;
            }
        } else if (strcmp(type, "filled") == 0) {
            if (roundedBoxRGBA(renderer, x1, y1, x2, y2, rad, color.r, color.g, color.b, color.a) != 0) {
                printf("%sExecutionError: Failed to draw filled rounded rectangle.\n", RED_COLOR);
                return -1;
            }
        } 
        
        renderTexture(renderer, texture, 750);
        if (handleEvents(renderer, texture) == -1) return -1;
        SDL_SetRenderTarget(renderer, NULL); 
    } 
    return 0;
}

int drawTriangle(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 cx, Sint16 cy, int radius, SDL_Color color, char *type) 
{
    // Validate the number of sides
    int sides = 3;

    Sint16 vx[12]; // Array for x-coordinates of vertices
    Sint16 vy[12]; // Array for y-coordinates of vertices

    for (int i = 0; i < sides; i++) {
        if (handleEvents(renderer, texture) == -1) return -1;
        float angle = i * (2 * M_PI / sides) + (30 * M_PI / 180.0); // Calculate the angle for each vertex
        vx[i] = cx + radius * cos(angle); // x-coordinate
        vy[i] = cy + radius * sin(angle); // y-coordinate
    }

    // Call the function to draw the polygon
    drawPolygon(renderer, texture, vx, vy, sides, color, type);
    if (handleEvents(renderer, texture) == -1) return -1;
    SDL_SetRenderTarget(renderer, NULL); 

    return 0;
}


/**
 * @brief Draws a polygon on the SDL renderer.
 * 
 * @param vx Array of x-coordinates for the vertices of the polygon.
 * @param vy Array of y-coordinates for the vertices of the polygon.
 * @param n The number of vertices in the polygon.
 * @return int Returns 0 on success, -1 on failure.
 */
int drawPolygon(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 *vx, Sint16 *vy, int n, SDL_Color color, char *type)
{
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("%sExecutionError: Invalid type for polygon '%s'. Must be 'filled' or 'empty'.\n", 
               RED_COLOR, type);
        return -1;
    }

    if(strcmp(type, "empty") == 0)
    {
        if(polygonRGBA(renderer, vx, vy, n, color.r, color.g, color.b, color.a) != 0)
        {
            printf("%sExecutionError: Failed to draw empty polygon.\n", RED_COLOR);
            return -1;
        }
    }
    else if(strcmp(type, "filled") == 0)
    {
        if(filledPolygonRGBA(renderer, vx, vy, n, color.r, color.g, color.b, color.a) != 0)
        {
            printf("%sExecutionError: Failed to draw filled polygon.\n", RED_COLOR);
            return -1;
        }
    }
    renderTexture(renderer, texture, 750);
    return 0;
}


/**
 * @brief Draws a custom polygon with a specified number of sides, centered at (cx, cy).
 * 
 * For a 4-sided polygon, this function will generate a rhombus instead of a square.
 * 
 * @param cx The x-coordinate of the polygon's center.
 * @param cy The y-coordinate of the polygon's center.
 * @param radius The radius from the center to the vertices.
 * @param sides The number of sides of the polygon (must be between 3 and 12).
 * @param rhombusOffset Multiplier for elongating the diagonals (use <1 for flat rhombus, >1 for tall rhombus).
 * @param color The color of the polygon.
 * @param type The type of polygon ("filled" or "empty").
 */
int drawCustomPolygon(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 cx, Sint16 cy, int radius, int sides, SDL_Color color, char *type) 
{
    // Validate the number of sides
    if (sides < 3 || sides > 12) {
        printf("%sExecutionError: Invalid number of polygon sides %d. Must be between 3 and 12.\n", 
               RED_COLOR, sides);
        return -1;
    }

    Sint16 vx[12]; // Array for x-coordinates of vertices
    Sint16 vy[12]; // Array for y-coordinates of vertices

    for (int i = 0; i < sides; i++) {
        if (handleEvents(renderer, texture) == -1) return -1;
        float angle = i * (2 * M_PI / sides) + 30; // Calculate the angle for each vertex
        vx[i] = cx + radius * cos(angle); // x-coordinate
        vy[i] = cy + radius * sin(angle); // y-coordinate
    }

    // Call the function to draw the polygon
    drawPolygon(renderer, texture, vx, vy, sides, color, type);
    if (handleEvents(renderer, texture) == -1) return -1;
    SDL_SetRenderTarget(renderer, NULL); 

    return 0;
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
int drawLine(SDL_Renderer *renderer, SDL_Texture *texture, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 width, SDL_Color color, char *type) {
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("%sExecutionError: Invalid type for line '%s'. Must be 'filled' or 'empty'.\n", 
               RED_COLOR, type);
        return -1;
    }
    else
    {
        if (handleEvents(renderer, texture) == -1) return -1;

        // Always use thickLineRGBA for consistent thickness
        if (thickLineRGBA(renderer, x1, y1, x2, y2, width, color.r, color.g, color.b, color.a) != 0) {
            printf("%sExecutionError: Failed to draw line.\n", RED_COLOR);
            return -1;
        }
        
        renderTexture(renderer, texture, 750);
        if (handleEvents(renderer, texture) == -1) return -1;
        SDL_SetRenderTarget(renderer, NULL);
    }
    return 0;
}




/**
 * @brief Draws an animated circle progressively.
 * @param We using SDL_RenderDrawPoint to have this effect
 * 
 * @param x X-coordinate of the circle's center.
 * @param y Y-coordinate of the circle's center.
 * @param radius Radius of the circle.
 * @return -1 if an event interrupts the drawing, 0 otherwise.
 */
int drawAnimatedCircle(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int radius, SDL_Color color, char *type) { 
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)) {
        printf("%sExecutionError: Invalid type for animated circle '%s'. Must be 'filled' or 'empty'.\n", 
               RED_COLOR, type);
        return -1;
    }

    SDL_SetRenderTarget(renderer, texture); // Set the texture as the rendering target.
    setRenderColor(renderer, color);

    // Iterate through the vertical range of the circle.
    for (int dy = -radius; dy <= radius; ++dy) {
        if (handleEvents(renderer, texture) == -1) return -1; // Handle events less frequently.

        // Calculate the horizontal range for the current row.
        int dxLimit = (int)sqrt(radius * radius - dy * dy); // Limit of x for the current y.

        for (int dx = -dxLimit; dx <= dxLimit; ++dx) {
            int distanceSquared = dx * dx + dy * dy;

            if (strcmp(type, "empty") == 0) {
                // Check if the point lies on the border of the circle.
                if (distanceSquared >= (radius - 1) * (radius - 1) && distanceSquared <= radius * radius) {
                    SDL_RenderDrawPoint(renderer, x + dx, y + dy);
                }
            } else if (strcmp(type, "filled") == 0) {
                // Check if the point lies inside the circle.
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }

        // Render the texture after finishing a row.
        renderTexture(renderer, texture, 6);
    }

    SDL_SetRenderTarget(renderer, NULL); // Reset the rendering target to the default.
    return 0; // Return success.
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
int drawAnimatedRectangle(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int w, int h, SDL_Color color, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid animatedRectangle type: \"%s.\". Must be filled or empty.\n", type);
        return -1;
    }
    else
    {
        SDL_SetRenderTarget(renderer, texture);

        if(strcmp(type, "empty") == 0)
        {

            setRenderColor(renderer, color);

            // Draw the top edge of the rectangle.
            for (int i = 0; i < w; ++i) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x + i, y);
                renderTexture(renderer, texture, 3);
            }

            // Draw the right edge.
            for (int j = 0; j < h; ++j) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x + w - 1, y + j);
                renderTexture(renderer, texture, 3);
            }

            // Draw the bottom edge.
            for (int i = w - 1; i >= 0; --i) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x + i, y + h - 1);
                renderTexture(renderer, texture, 3);
            }

            // Draw the left edge.
            for (int j = h - 1; j >= 0; --j) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x, y + j);
                renderTexture(renderer, texture, 3);
            }

        }
        else if(strcmp(type, "filled") == 0)
        {
            setRenderColor(renderer, color);

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
            printf("Invalid animatedRectangle type: \"%s.\". Must be filled or empty.\n", type);
        }
        SDL_SetRenderTarget(renderer, NULL);
    }
    
    return 0;
}

int drawAnimatedSquare(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int c, SDL_Color color, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid animatedSquare type: \"%s.\". Must be filled or empty.\n", type);
        return -1;
    }
    else
    {
        SDL_SetRenderTarget(renderer, texture);

        if(strcmp(type, "empty") == 0)
        {

            setRenderColor(renderer, color);

            // Draw the top edge of the rectangle.
            for (int i = 0; i < c; ++i) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x + i, y);
                renderTexture(renderer, texture, 3);
            }

            // Draw the right edge.
            for (int j = 0; j < c; ++j) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x + c - 1, y + j);
                renderTexture(renderer, texture, 3);
            }

            // Draw the bottom edge.
            for (int i = c - 1; i >= 0; --i) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x + i, y + c - 1);
                renderTexture(renderer, texture, 3);
            }

            // Draw the left edge.
            for (int j = c - 1; j >= 0; --j) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x, y + j);
                renderTexture(renderer, texture, 3);
            }

        }
        else if(strcmp(type, "filled") == 0)
        {
            setRenderColor(renderer, color);

            for (int i = 0; i < c; ++i) 
            {
                if (handleEvents(renderer, texture) == -1) return -1;

                for (int j = 0; j < c; ++j) {
                    SDL_RenderDrawPoint(renderer, x + i, y + j);
                }
                renderTexture(renderer, texture, 3);
            }
        }
        else
        {
            printf("Invalid animatedRectangle type: \"%s.\". Must be filled or empty.\n", type);
        }
        SDL_SetRenderTarget(renderer, NULL);
    }
    
    return 0;
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
int drawAnimatedRoundedRectangle(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int w, int h, int radius, SDL_Color color, char *type) {
    
    // Validate that the radius fits within the dimensions.
    if (radius * 2 > w || radius * 2 > h) {
        printf("Invalid radius: too large for the rectangle dimensions.\n");
        return -1;
    }
    else if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)) {
        printf("Invalid animatedRoundedRectangle type: \"%s.\". Must be filled or empty.\n", type);
        return -1;
    }
    else
    {
        SDL_SetRenderTarget(renderer, texture);

        setRenderColor(renderer, color);

        if (strcmp(type, "empty") == 0) {
            // Draw rounded corners
            for (double theta = 0; theta <= M_PI / 2; theta += 0.01) {
                if (handleEvents(renderer, texture) == -1) return -1;

                // Top-Left corner 
                SDL_RenderDrawPoint(renderer, x + radius - (int)(radius * cos(theta)), y + radius - (int)(radius * sin(theta)));
                renderTexture(renderer, texture, 1);

                // Top-Right corner
                SDL_RenderDrawPoint(renderer, x + w - radius + (int)(radius * cos(theta)), y + radius - (int)(radius * sin(theta)));
                renderTexture(renderer, texture, 1);

                // Bottom-Left corner
                SDL_RenderDrawPoint(renderer, x + radius - (int)(radius * cos(theta)), y + h - radius + (int)(radius * sin(theta)));
                renderTexture(renderer, texture, 1);

                // Bottom-Right corner
                SDL_RenderDrawPoint(renderer, x + w - radius + (int)(radius * cos(theta)), y + h - radius + (int)(radius * sin(theta)));
                renderTexture(renderer, texture, 1);
            }

            // Draw the left and right edges 
            for (int j = radius; j < h - radius; ++j) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x, y + j); // left
                SDL_RenderDrawPoint(renderer, x + w - 1, y + j); // right
                renderTexture(renderer, texture, 2);
            }

            // Draw the top and bottom edges 
            for (int i = radius; i < w - radius; ++i) {
                if (handleEvents(renderer, texture) == -1) return -1;

                SDL_RenderDrawPoint(renderer, x + i, y); // top
                SDL_RenderDrawPoint(renderer, x + i, y + h - 1); // bottom
                renderTexture(renderer, texture, 4);
            }
        } 
        else if (strcmp(type, "filled") == 0) {
            // Fill the rectangle with rounded corners row by row.
            for (int j = 0; j < h; ++j) {
                for (int i = 0; i < w; ++i) {
                    // Check if the point is within rounded corners.
                    int dist_top_left = (i - radius) * (i - radius) + (j - radius) * (j - radius);
                    int dist_top_right = (i - (w - radius - 1)) * (i - (w - radius - 1)) + (j - radius) * (j - radius);
                    int dist_bottom_left = (i - radius) * (i - radius) + (j - (h - radius - 1)) * (j - (h - radius - 1));
                    int dist_bottom_right = (i - (w - radius - 1)) * (i - (w - radius - 1)) + (j - (h - radius - 1)) * (j - (h - radius - 1));

                    // Skip points outside the rounded corners.
                    if ((dist_top_left > radius * radius && j < radius && i < radius) ||
                        (dist_top_right > radius * radius && j < radius && i >= w - radius) ||
                        (dist_bottom_left > radius * radius && j >= h - radius && i < radius) ||
                        (dist_bottom_right > radius * radius && j >= h - radius && i >= w - radius)) {
                        continue;
                    }

                    SDL_RenderDrawPoint(renderer, x + i, y + j);
                    
                }
                if (handleEvents(renderer, texture) == -1) return -1;
                renderTexture(renderer, texture, 6);
            }
        }
        SDL_SetRenderTarget(renderer, NULL); 
    }
    return 0;
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
int drawAnimatedEllipse(SDL_Renderer* renderer, SDL_Texture *texture, int x, int y, int rx, int ry, SDL_Color color, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("Invalid animatedEllipse type: \"%s.\". Must be filled or empty.\n", type);
        return -1;
    }
    else
    {
        SDL_SetRenderTarget(renderer, texture);

        setRenderColor(renderer, color);

        if (strcmp(type, "empty") == 0) 
        {
            // Number of steps to approximate the ellipse
            const int steps = 360;  // More steps = More more precise, but more slow
            for (int i = 0; i < steps; i++) 
            {
                if (handleEvents(renderer, texture) == -1) return -1;

                // Calculate the angle and point on the ellipse's boundary
                float angle = (i * 2 * M_PI) / steps;
                int dx = (int)(rx * cos(angle));
                int dy = (int)(ry * sin(angle));

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

                float angle = (i * 2 * M_PI) / steps;
                int dx = (int)(rx * cos(angle));
                int dy = (int)(ry * sin(angle));
                

                // Fill the ellipse by drawing points inside its boundary
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
        SDL_SetRenderTarget(renderer, NULL);  
    }
    return 0;
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
int drawAnimatedArc(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, int radius, int start_angle, int end_angle, SDL_Color color, char *type) {
    
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("%sExecutionError: Invalid type for animated arc '%s'. Must be 'filled' or 'empty'.\n", 
               RED_COLOR, type);
        return -1;
    }
    else
    {
        SDL_SetRenderTarget(renderer, texture);

        setRenderColor(renderer, color);

        // Convert start and end angles to radians
        double startRad = start_angle * M_PI / 180.0;
        double endRad = end_angle * M_PI / 180.0;

        if (strcmp(type, "empty") == 0) {
            for (double theta = startRad; theta <= endRad; theta += 0.01) {
                int px = (int)(radius * cos(theta));
                int py = (int)(radius * sin(theta));
                SDL_RenderDrawPoint(renderer, x + px, y - py);
                renderTexture(renderer, texture, 5);
                if (handleEvents(renderer, texture) == -1) return -1;
            }
        }
        else if (strcmp(type, "filled") == 0) {
            for (int currentRadius = 0; currentRadius <= radius; ++currentRadius) {
                for (double angle = startRad; angle <= endRad; angle += 0.01) {
                    int px = x + (int)(currentRadius * cos(angle));
                    int py = y + (int)(currentRadius * sin(angle));
                    SDL_RenderDrawLine(renderer, x, y, px, py);
                }
                renderTexture(renderer, texture, 5);
                if (handleEvents(renderer, texture) == -1) return -1;
            }
        }
        SDL_SetRenderTarget(renderer, NULL); 
    }
    return 0;
}

int drawAnimatedTriangle(SDL_Renderer *renderer, SDL_Texture *texture, int cx, int cy, int radius, SDL_Color color, char *type) 
{
    
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)) {
        printf("%sExecutionError: Invalid type for animated polygon '%s'. Must be 'filled' or 'empty'.\n", 
               RED_COLOR, type);
        return -1;
    } 
    
    else {
        int sides = 3;

        SDL_SetRenderTarget(renderer, texture);

        Sint16 vx[12]; 
        Sint16 vy[12];

        // Calculate vertices of the polygon
        for (int i = 0; i < sides; i++) 
        {
            float angle = i * (2 * PI / sides) + (30 * M_PI / 180.0);
            vx[i] = cx + radius * cos(angle);
            vy[i] = cy + radius * sin(angle);
        }

        setRenderColor(renderer, color);

        if (strcmp(type, "empty") == 0) {
            // draw the contours of the polygon
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
            // Fill in the polygon by drawing horizontal lines
            int ymin = vy[0], ymax = vy[0];
            for (int i = 1; i < sides; i++) {
                if (vy[i] < ymin) ymin = vy[i];
                if (vy[i] > ymax) ymax = vy[i];
            }

            for (int y = ymin; y <= ymax; y++) {
                int intersections[12]; // Intersection points for a horizontal line
                int count = 0;

                // Calculate the points of intersection between the horizontal line and each edge
                for (int i = 0; i < sides; i++) {
                    int x1 = vx[i], y1 = vy[i];
                    int x2 = vx[(i + 1) % sides], y2 = vy[(i + 1) % sides];

                    if ((y1 <= y && y2 > y) || (y2 <= y && y1 > y)) { // Intersects the horizontal line
                        int x = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
                        intersections[count++] = x;
                    }
                }

                // Sort intersections in ascending order
                for (int i = 0; i < count - 1; i++) {
                    for (int j = i + 1; j < count; j++) {
                        if (intersections[i] > intersections[j]) {
                            int temp = intersections[i];
                            intersections[i] = intersections[j];
                            intersections[j] = temp;
                        }
                    }
                }

                // Draw lines between pairs of intersections
                for (int i = 0; i < count; i += 2) {
                    if (handleEvents(renderer, texture) == -1) return -1;
                    for (int x = intersections[i]; x <= intersections[i + 1]; x++) {
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                    renderTexture(renderer, texture, 4);
                }
            }
        }
        SDL_SetRenderTarget(renderer, NULL); 
    } 
    return 0;
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
int drawAnimatedCustomPolygon(SDL_Renderer *renderer, SDL_Texture *texture, int cx, int cy, int radius, int sides, SDL_Color color, char *type) 
{
    
    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)) {
        printf("%sExecutionError: Invalid type for animated polygon '%s'. Must be 'filled' or 'empty'.\n", 
               RED_COLOR, type);
        return -1;
    } 
    else if (sides < 3 || sides > 12) {
        printf("%sExecutionError: Invalid number of polygon sides %d. Must be between 3 and 12.\n", 
               RED_COLOR, sides);
        return -1;
    }
    else {

        SDL_SetRenderTarget(renderer, texture);

        Sint16 vx[12]; 
        Sint16 vy[12];

        // Calculate vertices of the polygon
        for (int i = 0; i < sides; i++) 
        {
            float angle = i * (2 * PI / sides);
            vx[i] = cx + radius * cos(angle);
            vy[i] = cy + radius * sin(angle);
        }

        setRenderColor(renderer, color);

        if (strcmp(type, "empty") == 0) {
            // draw the contours of the polygon
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
            // Fill in the polygon by drawing horizontal lines
            int ymin = vy[0], ymax = vy[0];
            for (int i = 1; i < sides; i++) {
                if (vy[i] < ymin) ymin = vy[i];
                if (vy[i] > ymax) ymax = vy[i];
            }

            for (int y = ymin; y <= ymax; y++) {
                int intersections[12]; // Intersection points for a horizontal line
                int count = 0;

                // Calculate the points of intersection between the horizontal line and each edge
                for (int i = 0; i < sides; i++) {
                    int x1 = vx[i], y1 = vy[i];
                    int x2 = vx[(i + 1) % sides], y2 = vy[(i + 1) % sides];

                    if ((y1 <= y && y2 > y) || (y2 <= y && y1 > y)) { // Intersects the horizontal line
                        int x = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
                        intersections[count++] = x;
                    }
                }

                // Sort intersections in ascending order
                for (int i = 0; i < count - 1; i++) {
                    for (int j = i + 1; j < count; j++) {
                        if (intersections[i] > intersections[j]) {
                            int temp = intersections[i];
                            intersections[i] = intersections[j];
                            intersections[j] = temp;
                        }
                    }
                }

                // Draw lines between pairs of intersections
                for (int i = 0; i < count; i += 2) {
                    if (handleEvents(renderer, texture) == -1) return -1;
                    for (int x = intersections[i]; x <= intersections[i + 1]; x++) {
                        SDL_RenderDrawPoint(renderer, x, y);
                    }
                    renderTexture(renderer, texture, 4);
                }
            }
        }
        SDL_SetRenderTarget(renderer, NULL); 
    } 
    return 0;
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
int drawAnimatedLine(SDL_Renderer *renderer, SDL_Texture *texture, int x1, int y1, int x2, int y2, int thickness, SDL_Color color, char *type) {

    if ((strcmp(type, "filled") != 0) && (strcmp(type, "empty") != 0)){
        printf("%sExecutionError: Invalid type for line '%s'. Must be 'filled' or 'empty'.\n", 
               RED_COLOR, type);
        return -1;
    }
    else
    {
        SDL_SetRenderTarget(renderer, texture);

        setRenderColor(renderer, color);

        // Variable initialisation for the Bresenham algorithm
        int dx = abs(x2 - x1);
        int dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;

        while (x1 != x2 || y1 != y2) {
            if (handleEvents(renderer, texture) == -1) return -1;

            // Draw the actual point
            if (strcmp(type, "filled") == 0) {
                for (int tx = -thickness / 2; tx <= thickness / 2; tx++) {
                    for (int ty = -thickness / 2; ty <= thickness / 2; ty++) {
                        SDL_RenderDrawPoint(renderer, x1 + tx, y1 + ty);
                    }
                }
            } else {
                SDL_RenderDrawPoint(renderer, x1, y1);
            }

            renderTexture(renderer, texture, 10);

            // Update of coordinates for the Bresenham algorithm
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x1 += sx; }
            if (e2 < dx) { err += dx; y1 += sy; }
        }
        SDL_SetRenderTarget(renderer, NULL); 
    }
    return 0;
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
int drawShape(SDL_Renderer *renderer, SDL_Texture *texture, char *shape, char *mode, char *type, SDL_Color color, ...) {
    Shape newShape;
    newShape.selected = false;
    newShape.color = color;
    newShape.typeForm = type;
    newShape.rotation = 0;
    newShape.isAnimating = false;
    newShape.zoom = 1.0f;
    newShape.zoom_direction = 1.0f;
    newShape.bounce_velocity = 0;
    newShape.bounce_direction = 0;

    newShape.animations[0] = ANIM_NONE;
    newShape.animations[1] = ANIM_NONE;
    newShape.animations[2] = ANIM_NONE;
    newShape.num_animations = 0;
    newShape.animation_parser = ANIM_NONE;

    // Set shape type based on parameter
    if (strcmp(shape, "circle") == 0) {
        newShape.type = SHAPE_CIRCLE;
    } else if (strcmp(shape, "rectangle") == 0) {
        newShape.type = SHAPE_RECTANGLE;
    } else if (strcmp(shape, "arc") == 0) {
        newShape.type = SHAPE_ARC;
    } else if (strcmp(shape, "ellipse") == 0) {
        newShape.type = SHAPE_ELLIPSE;
    } else if (strcmp(shape, "line") == 0) {
        newShape.type = SHAPE_LINE;
    } else if (strcmp(shape, "polygon") == 0) {
        newShape.type = SHAPE_POLYGON;
    } else if (strcmp(shape, "triangle") == 0) {
        newShape.type = SHAPE_TRIANGLE;
    } else if (strcmp(shape, "square") == 0) {
        newShape.type = SHAPE_SQUARE;
    } else if (strcmp(shape, "rounded_rectangle") == 0) {
        newShape.type = SHAPE_ROUNDED_RECTANGLE;
    }

    va_list args;
    va_start(args, color);// Initialize the variable argument list with the last named parameter
    
    // Set the texture as the rendering target.
    SDL_SetRenderTarget(renderer, texture);

    // Determine if the shape should be drawn with animation.
    int isAnimated = (strcmp(mode, "animated") == 0);

    // Handle drawing for each shape type.
    if (strcmp(shape, "circle") == 0) {
        // Extract arguments specific
        int x = va_arg(args, int);
        int y = va_arg(args, int);
        int radius = va_arg(args, int);

        // Call the appropriate drawing function based on the mode.
        if (isAnimated) {
            if(drawAnimatedCircle(renderer, texture, x, y, radius, color, type) == -1) return -1;
        } else {
            if(drawCircle(renderer, texture, x, y, radius, color, type) == -1) return -1;
        }

        // Register the circle shape in the shapes list.
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

        newShape.data.rectangle.x = x;
        newShape.data.rectangle.y = y;
        newShape.data.rectangle.width = w;
        newShape.data.rectangle.height = h;

        addShape(newShape);
    } 
    else if (strcmp(shape, "square") == 0) {
        int x = va_arg(args, int);
        int y = va_arg(args, int);
        int c = va_arg(args, int);

        if (isAnimated) {
            if(drawAnimatedSquare(renderer, texture, x, y, c, color, type) == -1) return -1;
        } else {
            if(drawSquare(renderer, texture, x, y, c, color, type) == -1) return -1;
        }

        newShape.data.square.x = x;
        newShape.data.square.y = y;
        newShape.data.square.c = c;

        addShape(newShape);
    }
    else if (strcmp(shape, "arc") == 0) {
        // Extract arc parameters.
        int x = va_arg(args, int);
        int y = va_arg(args, int);
        int radius = va_arg(args, int);

        // Ensure a minimum radius.
        if (radius < 5) radius = 5;

        // Handle the start and end angles.
        int start_angle = va_arg(args, int);
        start_angle = fmax(0, fmin(360, start_angle));
        int end_angle = va_arg(args, int);
        end_angle = fmax(0, fmin(360, end_angle));

        // Swap angles if necessary.
        if (start_angle > end_angle) {
            int temp = start_angle;
            start_angle = end_angle;
            end_angle = temp;
        }

        if (isAnimated) {
            if(drawAnimatedArc(renderer, texture, x, y, radius, start_angle, end_angle, color, type) == -1) return -1;
        } else {
            if(drawArc(renderer, texture, x, y, radius, start_angle, end_angle, color, type) == -1) return -1;
        }

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

        newShape.data.polygon.cx = cx;
        newShape.data.polygon.cy = cy;
        newShape.data.polygon.radius = radius;
        newShape.data.polygon.sides = sides;

        addShape(newShape);
    } 
    else if (strcmp(shape, "triangle") == 0) {
        int cx = va_arg(args, int);
        int cy = va_arg(args, int);
        int radius = va_arg(args, int);

        if (isAnimated) {
            if(drawAnimatedTriangle(renderer, texture, cx, cy, radius, color, type) == -1) return -1;
        } else {
            if(drawTriangle(renderer, texture, cx, cy, radius, color, type) == -1) return -1;
        }

        newShape.data.triangle.cx = cx;
        newShape.data.triangle.cy = cy;
        newShape.data.triangle.radius = radius;

        addShape(newShape);
    }

    // Restore the default rendering target.
    SDL_SetRenderTarget(renderer, NULL);

    // End the variable argument processing.
    va_end(args);
    return 0;
}
