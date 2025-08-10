# Sega Genesis Platform Abstraction Layer (SGP) v2.2 
[![Build](https://github.com/savaughn/sega-genesis-pal/actions/workflows/version-bump.yaml/badge.svg?branch=main&event=workflow_run)](https://github.com/savaughn/sega-genesis-pal/actions/workflows/version-bump.yaml) [![Run Tests](https://github.com/savaughn/sega-genesis-pal/actions/workflows/test.yaml/badge.svg?branch=main)](https://github.com/savaughn/sega-genesis-pal/actions/workflows/test.yaml)

A lightweight, header-only abstraction layer for Sega Genesis/MegaDrive development using [SGDK](https://github.com/Stephane-D/SGDK). SGP provides helper functions for common usage patterns such as camera control, input handling, and basic collisions for Sega Genesis/MegaDrive development. Inspired by [raylib](https://github.com/raysan5/raylib).

## Features

- **Input Management**: Edge detection for button presses/releases with dual controller support
- **Camera System**: Smooth following, clamping, shake, and direct update
- **Collision System**: Box collision and Player-Level collision
- **Debug Print**: Toggleable debug text output
- **Header-Only**: No compilation required - just include and use
- **Performance Focused**: Inline functions and efficient state management
- **SGDK Compatible**: Built specifically for SGDK development workflow

## Versioning

SGP follows the major version of SGDK for compatibility. For example, SGP 2.x is designed to work with SGDK 2.x. Minor and patch versions of SGP (e.g., 2.1, 2.1.1) indicate new features or bugfixes, but the major version will always match the supported SGDK major version.

## Warning
This is a header-only library, so all definitions are in the header file. This means you can use the structures directly in your code, but you should not modify them unless you know what you're doing. The public API provides getter and setter functions for all values that should be accessed by users.

This library is under active development, so expect changes and improvements over time. Always check the [API Reference](API_REFERENCE.md) for the latest functions and usage patterns. Breaking changes may occur.

## Quick Start
```c
// Include the SGP header
#include "sgp.h"
// Define the global state in one .c file
SGP sgp;
// Initialize SGP
int main(_bool)
{
    SGP_init();
    while(1) {
        SGP_PollInput();
        if (SGP_ButtonPressed(JOY_1, BUTTON_A)) {
            // Handle A button press
        }
        SYS_doVBlankProcess();
    }
}
```

## Full API Reference

See [API_REFERENCE.md](API_REFERENCE.md) for the complete API, data structures, and usage examples.

## TODO
- Collisions for all types such as platforms and slopes
- Collision for top-down vs side-scrolling
- Physics (gravity)

## Requirements

- [SGDK (Sega Genesis Development Kit)](https://github.com/Stephane-D/SGDK)
- C99 compatible compiler

## License

MIT License - see source file for full license text.

## Contributing

Fork and PR, please! Contributions are welcome to improve SGP.

## Acknowledgements
- [SGDK](https://github.com/Stephane-D/SGDK)
- [Pigsy's Retro Game Dev Tutorials](https://www.youtube.com/@PigsysRetroGameDevTutorials)
