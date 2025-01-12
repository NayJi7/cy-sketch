# Draw++ SDL Application

## Overview
The C part of the Draw++ project uses the SDL2 library to create an interactive drawing application. This application allows creating and manipulating various geometric shapes with advanced animation and interaction features.

## Contributors

### Louaye Saghir & Alice Franco
- Shared features:
  - Shape rotation system
  - Shape zoom in/out
  - Shape deletion management
  - Shape selection

### Alice Franco
- Interactive cursor development
- Animated shape drawing system (progressive creation)
- Mouse event handling

### Louaye Saghir
- Code optimization (drawShape)
- Global event handling (handleEvent)
- "Escape Shape" game development
- Instant shape creation
- User interface (on-screen text and information)

### Adam Terrak
- Complete shape animation system
- "Defense Shape" game development
- Animation optimization

## Prerequisites
- SDL2
- SDL2_gfx
- SDL2_ttf

## Features

### Supported Shapes
- Circles
- Rectangles
- Squares
- Triangles
- Arcs
- Ellipses
- Lines
- Polygons (3 to 12 sides)

### Drawing Modes
- **Animated**: Shape draws progressively pixel by pixel
- **Instant**: Shape appears immediately
- **Style**: Choice between outline or filled shape

### Interface Information
- **Top Left**: Cursor coordinates (x, y)
- **Top Right**: Last key pressed
- **Bottom Left**: Selected shape animations
- **Bottom Right**: Detailed shape information
  - Shape type
  - Position
  - Available animations
  - Angles
  - Rotation
  - Dimensions

### Interactive Features

#### Basic Shape Manipulation
- **Movement**: 
  - Mouse: Drag & Drop
  - Keyboard: Arrow keys (←, →, ↑, ↓)
- **Rotation**: 
  - 'd': Clockwise
  - 'q': Counter-clockwise
- **Selection**: 
  - 'e': Toggle selection
  - Mouse click
- **Zoom**: 
  - '*': Enlarge
  - '/': Reduce
- **Reset**: 'r' to restore initial position and size

#### Layer Management
- 's': Move shape backward
- 'z': Move shape forward

#### Animation Controls
- '+': Add animation
- '-': Remove animation
- 'a': Start animation
- 'n': Stop animation

### Game Modes (Toggle with 'g')
1. **Escape Game**
   - Objective: Capture fleeing shapes
   - Shapes escape from the cursor

2. **Defense Game**
   - Objective: Protect shapes from red projectiles
   - Destroy enemy projectiles

Game Controls:
- 'ENTER': Start game
- 'SPACE': Exit game mode

### Additional Controls
- 'Delete': Delete selected shape
- 'Escape': Close application

## Building and Running

## Note
Interface colors automatically adapt to the background color for better readability.
