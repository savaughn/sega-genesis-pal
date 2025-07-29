
# SGP API Reference

## Public API

### Initialization

- `SGP_init(void)`
- `SGP_CameraInit(Map *map)`

### Input

- `SGP_PollInput(void)`
- `SGP_ButtonPressed(u16 joy, u16 button)`
- `SGP_ButtonReleased(u16 joy, u16 button)`
- `SGP_ButtonDown(u16 joy, u16 button)`

### Camera

- `SGP_CameraFollowTarget(SGPCameraTarget *target)`
- `SGP_ClampPositionToMapBounds(fix32 *x, fix32 *y, u16 width, u16 height)`
- `SGP_activateCamera(void)`
- `SGP_deactivateCamera(void)`
- `SGP_isCameraActive(void)`
- `SGP_UpdateCameraPosition(u32 x, u32 y)`
- `SGP_ShakeCamera(u16 duration, s16 intensity)`
- `SGP_CameraSetVerticalScrollLimit(u16 limit)`
- `SGP_CameraGetVerticalScrollLimit(void)`

### Collision

- `SGP_CheckBoxCollision(const SGPBox *a, const SGPBox *b)`
- `SGP_PlayerLevelCollision(u16 player_index, s16 x, s16 y, u16 w, u16 h, const SGPLevelCollisionData *level, SGPMovementDirection direction)`

### Debug

- `SGP_ToggleDebug(void)`
- `SGP_isDebugEnabled(void)`
- `SGP_DebugPrint(const char *text, u16 x, u16 y)`



### Types

#### SGP (global state, must be defined in one .c file)
```c
typedef struct {
    SGPInput input;   // Input state
    SGPCamera camera; // Camera state
} SGP;
```

#### SGPMovementDirection (movement directions for sprites and objects)
```c
typedef enum {
    SGP_DIR_UP = 1,
    SGP_DIR_DOWN = 2,
    SGP_DIR_LEFT = 4,
    SGP_DIR_RIGHT = 8
} SGPMovementDirection;
```

#### SGPCameraTarget (for camera following)
```c
typedef struct {
    Sprite *sprite;      // Sprite to follow
    fix32 *target_x_ptr; // Target X position (fixed-point)
    fix32 *target_y_ptr; // Target Y Position (fixed-point)
    u16 sprite_width;    // Width of the sprite being followed
    u16 sprite_height;   // Height of the sprite being followed
} SGPCameraTarget;
```

#### SGPBox (for box collision)
```c
typedef struct {
    u16 x; // X position of the box
    u16 y; // Y position of the box
    u16 w; // Width of the box
    u16 h; // Height of the box
} SGPBox;
```

#### SGPLevelCollisionData (for tile collision)
```c
typedef struct {
    u16 row_length;             // Row length (tiles per row)
    u16 data_length;            // Total data array length (tiles)
    const u8 *collision_data;   // Pointer to collision tile data
} SGPLevelCollisionData;
```


## Usage Examples

### Basic Input Handling
```c
if (SGP_ButtonDown(JOY_1, BUTTON_LEFT)) {
    player_x -= 2;
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
SGP_CameraFollowTarget(&playerTarget);
SGP_ClampPositionToMapBounds(&player_x, &player_y, 16, 16);
```

### Collision Detection
```c
SGPBox player_box = { player_x, player_y, 16, 16 };
SGPBox enemy_box = { enemy_x, enemy_y, 16, 16 };
if (SGP_CheckBoxCollision(&player_box, &enemy_box)) {
    // Handle collision
}
```

### Tile/Level Collision
```c
if (SGP_PlayerLevelCollision(player_index, player_x, player_y, 16, 16, &level_data, SGP_DIR_DOWN)) {
    // Player is colliding with the ground below
}
```
---

## Internal Structures & Details

These are for contributors. Users do not need to access these directly. The nature of the header-only library means these structures are defined in the header file and can be used as needed, but they are not part of the public API. All values in these structures that can be accessed by users are documented in the public API section above as getter and setter functions.

### SGP struct members
```c
typedef struct {
    SGPInput input;   // Input state (see below)
    SGPCamera camera; // Camera state (see below)
} SGP;
```

### input struct
```c
typedef struct {
    u16 joy1_state;
    u16 joy2_state;
    u16 joy1_previous;
    u16 joy2_previous;
} SGPInput;
```

### SGPCamera struct
```c
typedef struct {
    fix16 offset_x;
    fix32 offset_y;
    fix32 *target_x;
    fix32 *target_y;
    u8 type;
    u16 current_x;
    u16 current_y;
    Sprite *sprite;
    u16 sprite_width;
    u16 sprite_height;
    bool active;
    Map *map;
    u16 map_height;
    u16 map_width;
    u16 max_vertical_scroll;
} SGPCamera;
```

### SGPMap struct
```c
typedef struct {
    Map *current;
    Map *previous;
    u16 height;
    u16 width;
} SGPMap;
```

### SGPBox struct
```c
typedef struct {
    u16 x;
    u16 y;
    u16 w;
    u16 h;
} SGPBox;
```

### SGPLevelCollisionData struct
```c
typedef struct {
    u16 row_length;
    u16 data_length;
    const u8 *collision_data;
} SGPLevelCollisionData;
```
