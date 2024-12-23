switch (shape->type) {
        case SHAPE_CIRCLE:
            drawCircle(renderer, texture, 
                       shape->data.circle.x, 
                       shape->data.circle.y, 
                       shape->data.circle.radius, 
                       shape->color, 
                       "filled");

            if (shape->selected) {
                drawCircle(renderer, texture, 
                       shape->data.circle.x, 
                       shape->data.circle.y, 
                       shape->data.circle.radius + 5, 
                       0xFFFFFF00, 
                       "empty");
            }

        case SHAPE_RECTANGLE: {
            SDL_Rect rect = {shape->data.rectangle.x, shape->data.rectangle.y, shape->data.rectangle.width, shape->data.rectangle.height};
            if (strcmp(shape->typeForm, "filled") == 0) {
                SDL_RenderFillRect(renderer, &rect);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                SDL_RenderDrawRect(renderer, &rect);
            }

            if (shape->selected) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
                SDL_Rect selectionRect = {rect.x - 5, rect.y - 5, rect.w + 10, rect.h + 10};
                SDL_RenderDrawRect(renderer, &selectionRect);
            }
        }           
        case SHAPE_ELLIPSE:
            if (strcmp(shape->typeForm, "filled") == 0) {
                filledEllipseColor(renderer, shape->data.ellipse.x, shape->data.ellipse.y, shape->data.ellipse.rx, shape->data.ellipse.ry, shape->color);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                ellipseColor(renderer, shape->data.ellipse.x, shape->data.ellipse.y, shape->data.ellipse.rx, shape->data.ellipse.ry, shape->color);
            }
            

        case SHAPE_ARC:
            arcColor(renderer, shape->data.arc.x, shape->data.arc.y, shape->data.arc.radius, shape->data.arc.start_angle, shape->data.arc.end_angle, shape->color);

        case SHAPE_ROUNDED_RECTANGLE: {
            if (strcmp(shape->typeForm, "filled") == 0) {
                roundedBoxColor(renderer, shape->data.rounded_rectangle.x1, shape->data.rounded_rectangle.y1, 
                                shape->data.rounded_rectangle.x2, shape->data.rounded_rectangle.y2, 
                                shape->data.rounded_rectangle.radius, shape->color);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                roundedRectangleColor(renderer, shape->data.rounded_rectangle.x, shape->data.rounded_rectangle.y, 
                                       shape->data.rounded_rectangle.w, shape->data.rounded_rectangle.h, 
                                       shape->data.rounded_rectangle.rad, shape->color);
            }
        }

        case SHAPE_POLYGON: {
            Sint16 vx[shape->data.polygon.sides];
            Sint16 vy[shape->data.polygon.sides];

            for (int i = 0; i < shape->data.polygon.sides; i++) {
                double angle = 2.0 * M_PI * i / shape->data.polygon.sides;
                vx[i] = shape->data.polygon.cx + cos(angle) * shape->data.polygon.radius;
                vy[i] = shape->data.polygon.cy + sin(angle) * shape->data.polygon.radius;
            }

            if (strcmp(shape->typeForm, "filled") == 0) {
                filledPolygonColor(renderer, vx, vy, shape->data.polygon.sides, shape->color);
            } else if (strcmp(shape->typeForm, "empty") == 0) {
                polygonColor(renderer, vx, vy, shape->data.polygon.sides, shape->color);
            }
            
        }

        case SHAPE_LINE:
            thickLineColor(renderer, shape->data.line.x1, shape->data.line.y1, shape->data.line.x2, shape->data.line.y2, shape->data.line.thickness, shape->color);
}