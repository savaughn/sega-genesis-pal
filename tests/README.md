# SGP Tests

This directory contains tests for the SGP (Sega Genesis Platform) header-only library.

## Test Files

- **`smoke_test.c`** - Basic smoke test that validates SGP compiles and functions work
- **`collision_test.c`** - Comprehensive collision detection test suite
- **`input_test.c`** - Comprehensive input function test suite
- **`camera_test.c`** - Camera system test suite for following, centering, and map bounds
- **`sgp_test.h`** - Test wrapper header with mock SGDK dependencies
- **`Makefile`** - Build system for tests

## Running Tests

### Quick Start
```bash
cd tests/
make test          # Run basic smoke test
make collision     # Run collision detection test
make input         # Run input function test
make camera        # Run camera system test
make all_tests     # Run all tests (smoke, collision, input, and camera)
make test_debug    # Run smoke test with DEBUG mode enabled
make collision_debug # Run collision test with DEBUG mode enabled
make input_debug   # Run input test with DEBUG mode enabled
make camera_debug  # Run camera test with DEBUG mode enabled
```

### Available Targets

- `make test` - Build and run smoke test
- `make collision` - Build and run collision test
- `make input` - Build and run input test
- `make camera` - Build and run camera test
- `make test_debug` - Build and run smoke test with DEBUG mode
- `make collision_debug` - Build and run collision test with DEBUG mode
- `make input_debug` - Build and run input test with DEBUG mode
- `make camera_debug` - Build and run camera test with DEBUG mode
- `make all_tests` - Run all tests (smoke, collision, input, and camera)
- `make syntax` - Check syntax for all tests (no execution)
- `make clean` - Remove build artifacts
- `make help` - Show all available targets

## Test Structure

The tests use a mock SGDK environment to allow compilation and testing outside of the full SGDK development environment. This enables:

- Continuous integration testing
- Local development validation
- Syntax checking
- Basic functionality verification

### Mock Environment

The `sgp_test.h` header provides:
- Mock SGDK type definitions (`u8`, `u16`, `s16`, etc.)
- Mock VDP and controller definitions
- Mock function implementations for SGDK calls
- Test mode flag (`SGP_TEST_MODE`) to skip `genesis.h` inclusion

## Test Coverage

### Smoke Test (`smoke_test.c`)

The smoke test validates:

- ✅ **SGP Initialization** - `SGP_init()` sets proper default state
- ✅ **Input Polling** - `SGP_PollInput()` works without crashes
- ✅ **Button Functions** - Edge detection and state checking functions work
- ✅ **Collision Helpers** - Tile collision helper functions work correctly
- ✅ **Box Collision** - Rectangle collision detection works
- ✅ **Utility Functions** - Metatile conversion and other utilities work
- ✅ **Debug Functions** - Debug mode features work (when `DEBUG` is defined)

### Collision Test (`collision_test.c`)

The collision test validates:

- ✅ **Basic Collision Detection** - Wall/boundary collision in all directions
- ✅ **Corridor Movement** - Movement in open spaces and hitting inner walls
- ✅ **Out-of-Bounds Handling** - Axis-specific OOB behavior (horizontal vs vertical)
- ✅ **Multi-Player Support** - Separate collision state for multiple players
- ✅ **Collision Caching** - Position-based caching and proper cache invalidation
- ✅ **Axis-Specific Sampling** - Center-row sampling for horizontal movement

### Camera Test (`camera_test.c`)

The camera test validates:

- ✅ **Camera Initialization** - `SGP_CameraInit()` sets up map bounds correctly
- ✅ **Camera Following** - `SGP_CameraFollowTarget()` follows sprite positions
- ✅ **Camera Centering** - Proper centering calculation with offset values
- ✅ **Map Bounds Clamping** - Camera position clamped to map boundaries
- ✅ **Sprite Positioning** - Sprite screen position calculated relative to camera
- ✅ **Camera State Management** - Activate/deactivate camera functionality
- ✅ **Direct Camera Updates** - Manual camera position updates when inactive

### Expected Output

**Smoke Test:**
```
=== SGP Smoke Test Suite ===

Testing SGP_init()... PASS
Testing SGP_PollInput()... PASS
Testing button functions... PASS
Testing collision helper functions... PASS
Testing box collision... PASS
Testing metatile conversion... PASS
Testing debug functions... PASS (DEBUG mode only)

=== Test Summary ===
Tests run: 6-7
Tests passed: 6-7
Tests failed: 0
Success rate: 100.0%

✓ All smoke tests passed!
SGP header compiles and basic functions work correctly.
```

**Collision Test:**
```
=== SGP Comprehensive Collision Test Suite ===

Test level layout (8x8 tiles, 16px each):
# = solid, . = empty
[... level layout display ...]

=== Basic Collision Tests ===
[... 5 collision tests ...]

=== Corridor Movement Tests ===
[... 4 movement tests ...]

=== Out of Bounds Tests ===
[... 6 OOB tests ...]

=== Multi-Player Support Tests ===
[... 3 multiplayer tests ...]

=== Collision Caching Tests ===
[... 3 caching tests ...]

=== Axis-Specific Sampling Tests ===
[... 2 sampling tests ...]

=== Test Summary ===
Tests run: 23
Tests passed: 23
Tests failed: 0
Success rate: 100.0%

✓ All tests passed!
```

**Camera Test:**
```
=== SGP Camera System Test Suite ===

Testing camera initialization... PASS
Testing camera following with centering... PASS
Testing map bounds clamping (far right/bottom)... PASS
Testing map bounds clamping (origin 0,0)... PASS
Testing camera deactivation and direct updates... PASS

=== Test Summary ===
Tests run: 5
Tests passed: 5
Tests failed: 0
Success rate: 100.0%

✓ All camera tests passed!
Camera system functions correctly with proper bounds checking.
```

## Adding New Tests

To add new tests:

1. Create a new `.c` file in the `tests/` directory
2. Include `sgp_test.h` for mock SGDK environment
3. Implement mock functions as needed
4. Add build target to `Makefile`
5. Update this README

## Requirements

- GCC or compatible C compiler
- Standard C99 support
- Make utility

## Notes

- Tests run in a mock environment and don't require SGDK installation
- Mock functions provide minimal implementations to prevent crashes
- Test mode prevents inclusion of `genesis.h` which requires full SGDK
- All SGP features should be testable in this environment
