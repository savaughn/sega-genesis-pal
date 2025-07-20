# Sega Genesis Platform Abstraction Layer (SGP)
A lightweight, header-only abstraction layer for Sega Genesis development using [SGDK](https://github.com/Stephane-D/SGDK). SGP provides ergonomic, high-performance input handling and camera helpers for Genesis games and demos. Inspired by [raylib](https://github.com/raysan5/raylib).

## Features

- **Input Management**: Edge detection for button presses/releases with dual controller support
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

## Data Structures

### SGP
Global platform state containing input data:

```c
typedef struct {
    u16 joy1_state;      ///< Current state of joypad 1
    u16 joy2_state;      ///< Current state of joypad 2
    u16 joy1_previous;   ///< Previous state of joypad 1
    u16 joy2_previous;   ///< Previous state of joypad 2
} input;

/**
 * @brief Platform-wide state for input and camera.
 *
 * Holds current and previous joypad states for both controllers,
 * as well as camera position, zoom, and rotation in fixed-point.
 */
typedef struct {
    input input;         ///< Input state
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
