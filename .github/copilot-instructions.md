# Sega Genesis Platform Abstraction Layer (SGP)

Always reference these instructions first and fallback to search or bash commands only when you encounter unexpected information that does not match the info here.

SGP is a header-only C library for Sega Genesis/MegaDrive development using SGDK. It provides helper functions for input management, camera control, collision detection, and debug printing. The library is designed for performance with inline functions and efficient state management.

## Working Effectively

### Repository Structure
- `sgp.h` (620 lines) - Complete header-only library implementation
- `README.md` - Overview and quick start guide  
- `API_REFERENCE.md` - Complete API documentation with examples
- `.vscode/c_cpp_properties.json` - VS Code configuration for SGDK development
- `.gitignore` - Basic ignore file (only .DS_Store)

### No Build System Required
- This is a **header-only library** - no compilation or build process is needed
- Users include `sgp.h` directly in their SGDK projects
- Library provides all functionality through inline functions and macros
- NEVER try to build this library standalone - it only works when included in SGDK projects

### Dependencies and Environment
- **Required**: SGDK (Sega Genesis Development Kit) v2.x compatible
- **Target**: Motorola 68000 (m68k) architecture
- **Compiler**: m68k-elf-gcc (part of SGDK toolchain)
- **Platform**: Sega Genesis/MegaDrive (16-bit console)

### Development Environment Setup
This library cannot be built or tested in standard Linux environments because:
1. Requires SGDK which provides genesis.h and specialized types/functions
2. Targets m68k architecture, not x86_64
3. No standalone build system exists
4. No test suite exists

### Validation Process
Since traditional building/testing is not possible, validate changes using:

1. **Syntax Validation** (can be done in this environment):
   ```bash
   # Create mock SGDK environment for basic syntax checking
   mkdir -p /tmp/sgp-validation
   cd /tmp/sgp-validation
   cp /path/to/repository/sgp.h .
   ```

2. **Create minimal mock genesis.h for syntax testing**:
   ```c
   // Save as genesis.h in validation directory
   #include <stdbool.h>
   #include <stddef.h>
   
   typedef unsigned char u8;
   typedef unsigned short u16;  
   typedef unsigned int u32;
   typedef signed short s16;
   typedef signed int s32;
   typedef int fix16;
   typedef int fix32;
   
   #define JOY_1 0
   #define JOY_2 1
   #define BUTTON_A 0x0040
   #define BUTTON_B 0x0010
   #define BUTTON_UP 0x0001
   #define BUTTON_DOWN 0x0002
   #define BUTTON_LEFT 0x0004
   #define BUTTON_RIGHT 0x0008
   
   // Additional SGDK mocks for compilation
   #define screenWidth 320
   #define screenHeight 224
   #define BG_B 1
   #define WINDOW 0
   #define PAL1 1
   #define DMA 1
   #define F32_toInt(x) ((int)(x))
   #define FIX32(x) ((fix32)(x))
   
   typedef struct { void* data; u16 w; u16 h; } Map;
   typedef struct { void* data; } Sprite;
   
   // Mock function declarations
   extern u16 JOY_readJoypad(u16 joy);
   extern void MAP_scrollTo(Map* map, u32 x, u32 y);
   extern void VDP_drawText(const char* str, u16 x, u16 y);
   extern void SYS_doVBlankProcess(void);
   extern void VDP_setHorizontalScroll(u16 bg, s16 scroll);
   extern void VDP_setVerticalScroll(u16 bg, s16 scroll);
   extern void SPR_setPosition(Sprite* sprite, s16 x, s16 y);
   extern void VDP_setWindowVPos(bool enable, u16 pos);
   extern void VDP_drawTextEx(u16 plane, const char* str, u16 attr, u16 x, u16 y, u16 method);
   extern u16 TILE_ATTR(u16 pal, bool priority, bool flipV, bool flipH);
   ```

3. **Create test file and compile**:
   ```c
   // Save as test_validation.c
   #include "sgp.h"
   SGP sgp;  // Required global state
   int main() {
       SGP_init();
       SGP_PollInput();
       return 0;
   }
   ```

4. **Test compilation commands** (takes ~0.1 seconds total):
   ```bash
   # Basic syntax check
   gcc -c test_validation.c -I. -std=c99
   
   # With DEBUG mode (tests conditional compilation)  
   gcc -c test_validation.c -I. -std=c99 -DDEBUG
   
   # With extra warnings
   gcc -c test_validation.c -I. -std=c99 -Wall -Wextra
   
   # All should compile without errors
   ```

5. **Complete validation example**:
   ```bash
   # Full validation workflow (takes ~0.1 seconds)
   mkdir -p /tmp/sgp-validation
   cd /tmp/sgp-validation
   cp /path/to/repository/sgp.h .
   
   # Create genesis.h mock (see above)
   # Create test_validation.c (see above)
   
   # Run all validation tests
   gcc -c test_validation.c -I. -std=c99 && \
   gcc -c test_validation.c -I. -std=c99 -DDEBUG && \
   gcc -c test_validation.c -I. -std=c99 -Wall -Wextra
   
   # Success if all compile without errors
   echo "Validation passed!"
   ```

4. **Manual code review focused on**:
   - C99 standard compliance
   - Proper use of SGDK types (u8, u16, fix32, etc.)
   - Inline function definitions are correct
   - No missing includes or dependencies
   - Consistent with existing code style

### Code Style and Standards
- Use SGDK types: u8, u16, u32, s16, s32, fix16, fix32, bool
- All functions are static inline for performance
- Use descriptive function names with SGP_ prefix
- Follow existing naming conventions (CamelCase for functions)
- Keep functions focused and efficient (Genesis has limited resources)
- Use bitwise operations for flags and performance
- Fixed-point arithmetic for position calculations

### Debug Features
- Debug functions are conditionally compiled with #ifdef DEBUG
- When DEBUG is defined: SGP_ToggleDebug(), SGP_isDebugEnabled(), SGP_DebugPrint()
- When DEBUG is undefined: debug functions are not available
- Always test both DEBUG and release modes when modifying debug code

### Key Components

#### Global State
```c
SGP sgp; // Must be declared in one .c file in user project
```

#### Input System
- Dual controller support (JOY_1, JOY_2)
- Edge detection for button presses/releases
- State tracking between frames

#### Camera System  
- Smooth following with customizable targets
- Map boundary clamping
- Screen shake effects
- Fixed-point precision for smooth movement

#### Collision System
- Box-to-box collision detection
- Tile-based level collision with directional support
- Optimized for Genesis tile system (16x16 pixels)

## Common Tasks

### Adding New Features
1. Follow existing patterns in sgp.h
2. Use static inline functions for performance
3. Add to appropriate section (Input/Camera/Collision/Debug)
4. Update API_REFERENCE.md with new functions
5. Test syntax with mock compilation
6. Ensure compatibility with existing SGDK projects

### Modifying Existing Code
1. NEVER change public API without major version bump
2. Maintain backwards compatibility
3. Test with both DEBUG and release modes
4. Validate all inline functions compile correctly
5. Check fixed-point arithmetic for precision issues

### Documentation Updates
- Update README.md for major changes
- Keep API_REFERENCE.md synchronized with code
- Include usage examples for new features
- Document any breaking changes clearly

### Version Management
- Major version matches SGDK compatibility (currently 2.x)
- Minor versions for new features (2.1, 2.2)
- Patch versions for bug fixes (2.1.1, 2.1.2)

## Validation Checklist

Before submitting changes:
- [ ] Code compiles with mock SGDK headers (syntax check) - run full validation process
- [ ] All inline functions use proper SGDK types (u8, u16, u32, s16, s32, fix16, fix32, bool)
- [ ] Fixed-point arithmetic is correct (use FIX32() for constants, F32_toInt() for conversion)
- [ ] Debug code works in both DEBUG and release modes  
- [ ] API_REFERENCE.md updated if public API changed
- [ ] Code follows existing style and patterns (static inline, SGP_ prefix)
- [ ] No breaking changes to existing functions
- [ ] Performance considerations addressed (this is for 16-bit hardware)
- [ ] All new functions have proper documentation comments
- [ ] Version number updated if needed (major.minor.patch)

## File Locations Reference

```
Repository Root:
├── sgp.h                    # Main header (all functionality)
├── README.md               # Project overview
├── API_REFERENCE.md        # Complete API documentation  
├── .vscode/               # VS Code configuration
│   └── c_cpp_properties.json
└── .gitignore             # Basic ignore file

Key Sections in sgp.h:
- Lines 1-67:    License and includes
- Lines 68-126:   Type definitions and structures  
- Lines 127-182:  Core functions (init, input, etc.)
- Lines 183-204:  Debug functions (conditional)
- Lines 205-302:  Camera system
- Lines 303-618:  Collision system
```

## Troubleshooting

### Common Validation Issues

**"genesis.h not found"**
- Ensure you created the mock genesis.h file in the validation directory
- Use the complete mock provided in validation instructions

**"conflicting types for Map/Sprite"**  
- Don't include both real genesis.h and mock genesis.h
- Only include sgp.h in test files, it will pull in the mock genesis.h

**"undeclared identifier 'true'/'false'"**
- Ensure mock genesis.h includes `#include <stdbool.h>`
- Test both with and without -DDEBUG flag

**"implicit declaration of function"**
- Add missing SGDK function declarations to mock genesis.h
- Most SGDK functions can be mocked as simple extern declarations

**Performance considerations for Sega Genesis:**
- Use bitwise operations instead of division/modulo when possible
- Prefer fixed-point over floating-point (no FPU on Genesis)
- Keep functions inline for performance
- Minimize memory allocations (very limited RAM)

## Important Notes

- **NEVER CANCEL**: No long-running builds exist - this is header-only
- **NO TESTS**: No automated test suite - rely on syntax validation and manual review
- **SGDK DEPENDENCY**: Cannot run without full SGDK environment
- **PERFORMANCE CRITICAL**: Target is 16-bit hardware with limited resources
- **HEADER-ONLY**: All functionality must be in sgp.h with inline functions

When in doubt, refer to existing SGDK documentation and follow established patterns in the codebase.