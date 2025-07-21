# Sega Genesis Platform Abstraction Layer (SGP)
A lightweight, header-only abstraction layer for Sega Genesis development using [SGDK](https://github.com/Stephane-D/SGDK). SGP provides ergonomic, high-performance input and camera helpers for Genesis development. Inspired by [raylib](https://github.com/raysan5/raylib).

## Features

- **Input Management**: Edge detection for button presses/releases with dual controller support
- **Camera System**: Smooth following, clamping, shake, and direct update
- **Debug Print**: Toggleable debug text output
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
    // Optionally initialize camera and map
    // SGP_CameraInit(mapPtr);
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

#### `SGP_CameraInit(const Map* map)`
Initializes camera and its map pointer. Call after loading your map.

```c
SGP_CameraInit(mapPtr);
// This sets sgp.camera.map->current, width, and height
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

### Debug Functions

#### `toggleDebug()`
Toggles debug text display on/off.

#### `SGP_DebugPrint(const char* text, int x, int y)`
Draws debug text if enabled.

### Camera & Map Functions

#### `SGP_CameraFollowTarget(SGPCameraTarget* target)`
Centers camera on a target sprite, clamped to map bounds.

#### `SGP_ClampPositionToMapBounds(fix32* x, fix32* y, s16 width, s16 height)`
Clamps entity position to map bounds.

#### `SGP_activateCamera()` / `SGP_deactivateCamera()` / `SGP_isCameraActive()`
Enable/disable camera tracking, or check if active.

#### `SGP_UpdateCameraPosition(int x, int y)`
Directly set camera position (when not tracking a target).

#### `SGP_ShakeCamera(int duration, int intensity)`
Shake camera for a number of frames.

## Data Structures

### input
Joypad state for both controllers:

```c
typedef struct {
    u16 joy1_state;
    u16 joy2_state;
    u16 joy1_previous;
    u16 joy2_previous;
} input;
```

### SGPCamera
Camera state for smooth scrolling and transformations:

```c
typedef struct {
    fix16 offset_x;
    fix32 offset_y;
    fix32* target_x;
    fix32* target_y;
    u8 type;
    int current_x;
    int current_y;
    Sprite* sprite;
    s16 sprite_width;
    s16 sprite_height;
    bool active;
    SGPMap* map; // Pointer to the current map being viewed
} SGPCamera;
```

### SGPMap
Current and previous map state:

```c
typedef struct {
    Map* current;
    Map* previous;
    u16 height;
    u16 width;
} SGPMap;
```

### SGP
Global platform state containing input and camera:

```c
typedef struct {
    input input;
    SGPCamera camera;
} SGP;
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

### Camera Following & Clamping

```c
SGPCameraTarget playerTarget = {
    .sprite = playerSprite,
    .target_x_ptr = &player_x,
    .target_y_ptr = &player_y,
    .sprite_width = 16,
    .sprite_height = 16
};

// Camera and clamping use sgp.camera.map
SGP_CameraFollowTarget(&playerTarget);
SGP_ClampPositionToMapBounds(&player_x, &player_y, 16, 16);
```

### Camera Shake

```c
// Shake camera for 10 frames with 4px intensity
SGP_ShakeCamera(10, 4);
```

### Debug Print

```c
toggleDebug(); // Enable debug
SGP_DebugPrint("Hello, Debug!", 2, 2);
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
