# SGP - Sega Genesis Platform Abstraction Layer

A lightweight, header-only abstraction layer for Sega Genesis development using [SGDK](https://github.com/Stephane-D/SGDK). SGP provides ergonomic, high-performance input handling and camera helpers for Genesis games and demos.

## Features

- **Input Management**: Edge detection for button presses/releases with dual controller support
- **Camera System**: Fixed-point camera with smooth following, clamping, and transforms
- **Header-Only**: No compilation required - just include and use
- **Performance Focused**: Inline functions and efficient state management
- **SGDK Compatible**: Built specifically for SGDK development workflow

## Quick Start

1. Include the header in your project:

```c
#include "sgp.h"
```

2. Define the global state in one .c file:

```c
SGP sgp;
```

3. Initialize and use in your main loop:

```c
int main() {
    SGP_init();
    
    while(1) {
        SGP_PollInput();
        
        if (SGP_ButtonPressed(JOY_1, BUTTON_A)) {
            // Handle A button press
        }
        
        VDP_waitVSync();
    }
}
```

## API Reference

### Initialization

#### `SGP_init()`
Initializes the SGP state with default values. Call once at startup.

```c
SGP_init();
```

### Input Functions

#### `SGP_PollInput()`
Updates input state for both controllers. Call once per frame before reading input.

```c
SGP_PollInput();
```

#### `SGP_ButtonPressed(u16 joy, u16 button)`
Returns true if button(s) were just pressed (edge detection).

```c
if (SGP_ButtonPressed(JOY_1, BUTTON_A | BUTTON_B)) {
    // A or B was just pressed on controller 1
}
```

#### `SGP_ButtonReleased(u16 joy, u16 button)`
Returns true if button(s) were just released.

```c
if (SGP_ButtonReleased(JOY_1, BUTTON_START)) {
    // Start button was just released
}
```

#### `SGP_ButtonDown(u16 joy, u16 button)`
Returns true if button(s) are currently held down.

```c
if (SGP_ButtonDown(JOY_1, BUTTON_LEFT)) {
    player_x -= 2;
}
```

### Camera Functions

#### `SGP_CameraInit(SGPCamera* cam, fix16 x, fix16 y)`
Initializes a camera with default values at the specified position.

```c
SGPCamera camera;
SGP_CameraInit(&camera, FIX16(160), FIX16(112));
```

#### `SGP_CameraSmoothFollow(SGPCamera* cam, fix16 target_x, fix16 target_y, fix16 smooth)`
Smoothly moves camera toward target position using interpolation.

```c
// Follow player with 10% smoothing
SGP_CameraSmoothFollow(&camera, player_x, player_y, FIX16(0.1));
```

#### `SGP_CameraClamp(SGPCamera* cam, fix16 min_x, fix16 min_y, fix16 max_x, fix16 max_y)`
Clamps camera position within specified bounds.

```c
// Keep camera within level boundaries
SGP_CameraClamp(&camera, FIX16(0), FIX16(0), FIX16(800), FIX16(600));
```

## Data Structures

### SGP
Global platform state containing input and camera data:

```c
typedef struct {
    u16 joy1_state;        // Current state of joypad 1
    u16 joy2_state;        // Current state of joypad 2
    u16 joy1_previous;     // Previous state of joypad 1
    u16 joy2_previous;     // Previous state of joypad 2
    fix16 camera_x;        // Camera X position (fixed-point)
    fix16 camera_y;        // Camera Y position (fixed-point)
    fix16 camera_zoom;     // Camera zoom (1.0 = default)
    fix16 camera_rotation; // Camera rotation (radians)
} SGP;
```

### SGPCamera
Camera structure for advanced camera operations:

```c
typedef struct {
    fix16 offset_x;   // Camera offset X
    fix16 offset_y;   // Camera offset Y
    fix16 target_x;   // Camera target X
    fix16 target_y;   // Camera target Y
    fix16 rotation;   // Camera rotation (radians)
    fix16 zoom;       // Camera zoom (1.0 = default)
} SGPCamera;
```

## Usage Examples

### Basic Input Handling

```c
#include <genesis.h>
#include "sgp.h"

SGP sgp; // Global state

int main() {
    SGP_init();
    
    s16 player_x = 160;
    s16 player_y = 112;
    
    while(1) {
        SGP_PollInput();
        
        // Movement with held buttons
        if (SGP_ButtonDown(JOY_1, BUTTON_LEFT))  player_x -= 2;
        if (SGP_ButtonDown(JOY_1, BUTTON_RIGHT)) player_x += 2;
        if (SGP_ButtonDown(JOY_1, BUTTON_UP))    player_y -= 2;
        if (SGP_ButtonDown(JOY_1, BUTTON_DOWN))  player_y += 2;
        
        // Jump on A press
        if (SGP_ButtonPressed(JOY_1, BUTTON_A)) {
            // Trigger jump
        }
        
        VDP_waitVSync();
    }
}
```

### Camera Following

```c
#include "sgp.h"

SGP sgp;

int main() {
    SGP_init();
    
    SGPCamera camera;
    SGP_CameraInit(&camera, FIX16(160), FIX16(112));
    
    fix16 player_x = FIX16(160);
    fix16 player_y = FIX16(112);
    
    while(1) {
        SGP_PollInput();
        
        // Update player position
        if (SGP_ButtonDown(JOY_1, BUTTON_LEFT))  player_x -= FIX16(2);
        if (SGP_ButtonDown(JOY_1, BUTTON_RIGHT)) player_x += FIX16(2);
        
        // Smooth camera follow
        SGP_CameraSmoothFollow(&camera, player_x, player_y, FIX16(0.05));
        
        // Keep camera in bounds
        SGP_CameraClamp(&camera, FIX16(0), FIX16(0), FIX16(1024), FIX16(512));
        
        // Use camera position for scrolling
        VDP_setHorizontalScroll(PLAN_A, -fix16ToInt(camera.target_x));
        VDP_setVerticalScroll(PLAN_A, -fix16ToInt(camera.target_y));
        
        VDP_waitVSync();
    }
}
```

## Controller Constants

SGP uses SGDK's standard button constants:

- `BUTTON_A`, `BUTTON_B`, `BUTTON_C`
- `BUTTON_X`, `BUTTON_Y`, `BUTTON_Z`
- `BUTTON_START`, `BUTTON_MODE`
- `BUTTON_UP`, `BUTTON_DOWN`, `BUTTON_LEFT`, `BUTTON_RIGHT`
- `BUTTON_NONE` - No buttons pressed

## Requirements

- [SGDK (Sega Genesis Development Kit)](https://github.com/Stephane-D/SGDK)
- C99 compatible compiler

## License

MIT License - see source file for full license text.

## Contributing

Feel free to submit issues and pull requests to improve SGP!
