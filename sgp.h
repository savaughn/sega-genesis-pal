/*
 * sgp.h - Sega Genesis Platform Abstraction Layer (header-only)
 *
 * MIT License
 *
 * Copyright (c) 2025 Spencer Vaughn
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Version: v2.2
 */
#ifndef SGP_H
#define SGP_H

#ifndef SGP_TEST_MODE
#include <genesis.h>
#endif

#define BUTTON_NONE 0x0000
#define VDP_SPRITE_OFFSET 0x80 // Offset for sprite coordinates in VDP

// Collision flag bitmasks
#define COLLIDE_DOWN (1 << 0)
#define COLLIDE_UP (1 << 1)
#define COLLIDE_LEFT (1 << 2)
#define COLLIDE_RIGHT (1 << 3)

// Bitwise flag helper macros
#define SET_ACTIVE(flags, mask) ((flags) |= (mask))
#define SET_INACTIVE(flags, mask) ((flags) &= ~(mask))
#define FLAG_IS_ACTIVE(flags, mask) (((flags) & (mask)) != 0)
#define FLAG_IS_INACTIVE(flags, mask) (((flags) & (mask)) == 0)

#define SGP_OOB_HORIZONTAL_SOLID true
#define SGP_OOB_HORIZONTAL_PASSABLE false

// Maximum number of player entities
#define SGP_MAX_PLAYER_COUNT 2

static const u16 SOLID_TILE = 1;
/**
 * On the 68000 (m68k) architecture, the m68k-elf-cc compiler (GCC for m68k)
 * handles the modulo operator (%) in C by generating a function call or a
 * sequence of instructions, depending on the operands:
 * For powers of two (e.g., % 16):
 * The compiler will optimize x % 16 to x & 15 (a bitwise AND), which is very fast and efficient.
 */
static const u16 COLLISION_TILE_SIZE_MASK = 15;
static const u16 PIXELS_TO_TILE_SHIFT = 4; // 16 pixels per tile

// Each metatile is 16x16 pixels, so 128x128 pixels block is 8x8 metatiles
static inline u16 SGP_MetatilesToPixels(u16 x) { return x << 7; }

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
/**
 * @brief Input state for joypads.
 */
typedef struct
{
    u16 joy1_state;    // Current state of joypad 1
    u16 joy2_state;    // Current state of joypad 2
    u16 joy1_previous; // Previous state of joypad 1
    u16 joy2_previous; // Previous state of joypad 2
} SGPInput;

typedef struct
{
    Map *current;
    Map *previous;
    u16 height;
    u16 width;
} SGPMap;

/**
 * @brief Camera state for smooth scrolling and transformations.
 *
 * Uses fixed-point math for precision on the Genesis.
 */
typedef struct
{
    u8 type;         // Camera type (e.g. CAMERA_SMOOTH)
    u32 current_x;   // Camera X position (integer for MAP_scrollTo)
    u32 current_y;   // Camera Y position (integer for MAP_scrollTo)
    Sprite *sprite;
    bool active;
    Map *map; // Pointer to the current map being viewed
    u16 map_height;
    u16 map_width;
    u16 max_vertical_scroll; // in Tiles (default 32), used to limit camera scroll
} SGPCamera;

typedef struct
{
    u16 x; // X position of the box
    u16 y; // Y position of the box
    u16 w; // Width of the box
    u16 h; // Height of the box
} SGPBox;

/**
 * @brief Platform-wide state for input and camera.
 *
 * Holds current and previous joypad states for both controllers,
 * as well as camera position, zoom, and rotation in fixed-point.
 */
typedef struct
{
    SGPInput input;   // Input state
    SGPCamera camera; // Camera state
} SGP;

/**
 * @brief Movement directions for sprites and objects.
 */
typedef enum
{
    SGP_DIR_UP = 1,
    SGP_DIR_DOWN = 2,
    SGP_DIR_LEFT = 4,
    SGP_DIR_RIGHT = 8
} SGPMovementDirection;

typedef struct
{
    u16 row_length;
    u16 data_length;
    const u8 *collision_data;
} SGPLevelCollisionData;

/**
 * @brief Global platform state (must be defined in one .c file).
 */
extern SGP sgp;

/**
 * @brief Initializes default Sega Genesis Platform Abstraction Layer (SGP) state.
 * Call once at startup.
 */
static inline void SGP_init(void)
{
    sgp.input.joy1_state = 0;
    sgp.input.joy2_state = 0;
    sgp.input.joy1_previous = 0;
    sgp.input.joy2_previous = 0;
    sgp.camera.current_x = 0;
    sgp.camera.current_y = 0;
    sgp.camera.active = false;
    sgp.camera.map = NULL;
    sgp.camera.map_height = 0;
    sgp.camera.map_width = 0;
    sgp.camera.max_vertical_scroll = 32;
}

//----------------------------------------------------------------------------------
// Debug Functions
//----------------------------------------------------------------------------------
#ifdef DEBUG
#define MAX_DEBUG_LINES 4
static bool showDebug = true;
static inline void SGP_ToggleDebug(void)
{
    showDebug = !showDebug;
}

static inline bool SGP_isDebugEnabled(void)
{
    return showDebug;
}
static inline void SGP_DebugPrint(const char *text, u16 x, u16 y)
{
    if (y > MAX_DEBUG_LINES)
    {
        return;
    }
    if (SGP_isDebugEnabled())
    {
        VDP_setWindowVPos(false, MAX_DEBUG_LINES + 1);
        VDP_drawTextEx(WINDOW, text, TILE_ATTR(PAL1, false, false, false), x, y, DMA);
    }
    else
    {
        VDP_setWindowVPos(false, 0);
    }
}

#endif // DEBUG

static inline void SGP_HandleError(const char *text)
{
    VDP_drawText(text, 0, 0);
    while (true)
    {
    } // Halt execution
}

//----------------------------------------------------------------------------------
// Input Functions
//----------------------------------------------------------------------------------

/**
 * @brief Polls and updates the current and previous state for both controllers.
 *
 * Call this once per frame before reading input.
 */
static inline void SGP_PollInput(void)
{
    sgp.input.joy1_previous = sgp.input.joy1_state;
    sgp.input.joy2_previous = sgp.input.joy2_state;
    sgp.input.joy1_state = JOY_readJoypad(JOY_1);
    sgp.input.joy2_state = JOY_readJoypad(JOY_2);
}

/**
 * @brief Returns true if the specified button(s) were just pressed (edge detection) for the given joypad.
 *
 * @param joy JOY_1 or JOY_2
 * @param button Button mask (e.g. BUTTON_A | BUTTON_B)
 */
static inline bool SGP_ButtonPressed(u16 joy, u16 button)
{
    u16 state = (joy == JOY_1) ? sgp.input.joy1_state : sgp.input.joy2_state;
    u16 prev = (joy == JOY_1) ? sgp.input.joy1_previous : sgp.input.joy2_previous;
    return ((state & button) != 0) && ((prev & button) == 0);
}

/**
 * @brief Returns true if the specified button(s) were just released for the given joypad.
 *
 * @param joy JOY_1 or JOY_2
 * @param button Button mask
 */
static inline bool SGP_ButtonReleased(u16 joy, u16 button)
{
    u16 state = (joy == JOY_1) ? sgp.input.joy1_state : sgp.input.joy2_state;
    u16 prev = (joy == JOY_1) ? sgp.input.joy1_previous : sgp.input.joy2_previous;
    return ((state & button) == 0) && ((prev & button) != 0);
}

/**
 * @brief Returns true if the specified button(s) are currently held down for the given joypad.
 *
 * @param joy JOY_1 or JOY_2
 * @param button Button mask
 */
static inline bool SGP_ButtonDown(u16 joy, u16 button)
{
    u16 state = (joy == JOY_1) ? sgp.input.joy1_state : sgp.input.joy2_state;
    return (state & button) != 0;
}

//----------------------------------------------------------------------------------
// Camera Functions (Fixed Point for Genesis)
//----------------------------------------------------------------------------------
typedef struct
{
    Sprite *sprite;      // Sprite to follow
    s32 offset_x;        // Offset from sprite X position (integer)
    s32 offset_y;        // Offset from sprite Y position (integer)
    s32 sprite_world_x; // Sprite's world X position (integer)
    s32 sprite_world_y; // Sprite's world Y position (integer)
} SGPCameraTarget;

/**
 * @brief Initializes a Camera struct with default values and a given target position.
 * @param cam Pointer to Camera struct
 * @param current_x   Target X position
 * @param current_y   Target Y position
 */
static inline bool SGP_CameraInit(Map *map)
{
    if (map == NULL) {
        return false;
    }
    
    sgp.camera.map = map;
    sgp.camera.map_height = SGP_MetatilesToPixels(map->h);
    sgp.camera.map_width = SGP_MetatilesToPixels(map->w);
    sgp.camera.active = true;
    return true;
}
/**
 * @brief Clamps the given fixed-point position to the map bounds.
 * @param x Pointer to X position (fixed-point)
 * @param y Pointer to Y position (fixed-point)
 * @param width Width of the entity
 * @param height Height of the entity
 */
static inline void SGP_ClampPositionToMapBounds(fix32 *x, fix32 *y, u16 width, u16 height)
{
    s16 pos_x = F32_toInt(*x);
    s16 pos_y = F32_toInt(*y);

    // Clamp position to map bounds (entity always visible on map, accounting for sprite dimensions)
    if (pos_x < 0)
        *x = FIX32(0);
    if (pos_x > sgp.camera.map_width - 1 - width)
        *x = FIX32(sgp.camera.map_width - 1 - width);
    if (pos_y < 0)
        *y = FIX32(0);
    if (pos_y > sgp.camera.map_height - height)
        *y = FIX32(sgp.camera.map_height - height);
}

/**
 * @brief Follows a target position with the camera, clamping to map bounds.
 * @param target Pointer to CameraTarget struct
 */
static inline void SGP_CameraFollowTarget(SGPCameraTarget *target)
{
    if (!sgp.camera.active) // Check if camera is active and map is set
    {
        return; // Camera not active, skip following
    }

    // Use target position directly as camera position, clamp to map bounds
    s16 new_camera_x = target->sprite_world_x - target->offset_x;
    s16 new_camera_y = target->sprite_world_y - target->offset_y;

    if (new_camera_x < 0)
        new_camera_x = 0;
    if (new_camera_x > sgp.camera.map_width - screenWidth)
        new_camera_x = sgp.camera.map_width - screenWidth;
    if (new_camera_y < 0)
        new_camera_y = 0;
    if (new_camera_y > sgp.camera.map_height - screenHeight)
        new_camera_y = sgp.camera.map_height - screenHeight;

    if ((sgp.camera.current_x != (u32)new_camera_x) ||
        (sgp.camera.current_y != (u32)new_camera_y))
    {
        sgp.camera.current_x = (u32)new_camera_x;
        sgp.camera.current_y = (u32)new_camera_y;

        static s16 bg_hscroll = 0, bg_vscroll = 0;
        bg_hscroll = (0 - new_camera_x) >> 3; // Convert to tile units (8 pixels)
        bg_vscroll = new_camera_y >> 3;       // Convert to tile units (8

        if (bg_vscroll > 32) // assuming
        {
            bg_vscroll = 32;
        }

        MAP_scrollTo(sgp.camera.map, new_camera_x, new_camera_y);
        VDP_setHorizontalScroll(BG_B, bg_hscroll);
        VDP_setVerticalScroll(BG_B, bg_vscroll);
    }
    if (target->sprite)
    {
        SPR_setPosition(target->sprite,
                        target->sprite_world_x - new_camera_x,
                        target->sprite_world_y - new_camera_y);
    }
}

/**
 * @brief Activates the camera, allowing it to follow a target.
 */
static inline void SGP_activateCamera(void)
{
    sgp.camera.active = true;
}
/**
 * @brief Deactivates the camera, stopping it from following a target.
 */
static inline void SGP_deactivateCamera(void)
{
    sgp.camera.active = false;
}
/**
 * @brief Checks if the camera is currently active.
 * @return True if the camera is active, false otherwise.
 */
static inline bool SGP_isCameraActive(void)
{
    return sgp.camera.active;
}
/**
 * @brief Updates the camera position directly without following a target.
 * @param x New X position
 * @param y New Y position
 */
static inline void SGP_UpdateCameraPosition(u32 x, u32 y)
{
    if (sgp.camera.active)
    {
        return; // if active, camera tracks target
    }
    sgp.camera.current_x = x;
    sgp.camera.current_y = y;
    MAP_scrollTo(sgp.camera.map, x, y);
}
/**
 * @brief Sets the horizontal scroll limit for the camera.
 * @param limit New vertical scroll limit in tiles
 */
static inline void SGP_CameraSetVerticalScrollLimit(u16 limit)
{
    sgp.camera.max_vertical_scroll = limit;
}

/**
 * @brief Gets the vertical scroll limit for the camera.
 * @return Current vertical scroll limit in tiles
 */
static inline u16 SGP_CameraGetVerticalScrollLimit(void)
{
    return sgp.camera.max_vertical_scroll;
}

/**
 * @brief Shakes the camera for a specified duration and intensity.
 * @param duration Duration of the shake in frames
 * @param intensity intensity of the shake (in pixels)
 *
 * This is just for fun not a serious feature.
 */
static inline void SGP_ShakeCamera(u16 duration, s16 intensity)
{
    SGP_deactivateCamera(); // Disable camera tracking during shake
    for (u16 i = 0; i < duration; i++)
    {
        s16 shake_x = (i % 2 == 0) ? intensity : -intensity;
        sgp.camera.current_x += shake_x;
        SGP_UpdateCameraPosition(sgp.camera.current_x, sgp.camera.current_y);
        SYS_doVBlankProcess();           // Wait for VBlank to apply shake
        sgp.camera.current_x -= shake_x; // Restore position for next frame
    }
    SGP_activateCamera(); // Re-enable camera tracking after shake
}
//----------------------------------------------------------------------------------
// Collision Functions
//----------------------------------------------------------------------------------
static inline bool SGP_CheckBoxCollision(const SGPBox *a, const SGPBox *b)
{
    return (a->x < b->x + b->w &&
            a->x + a->w > b->x &&
            a->y < b->y + b->h &&
            a->y + a->h > b->y);
}

// Helpers for tile collision queries
static inline u16 SGP_LevelTotalRows(const SGPLevelCollisionData *level)
{
    return (level->row_length == 0) ? 0 : (level->data_length / level->row_length);
}

// Axis-aware solidity check: control OOB behavior per axis
static inline bool SGP_TileIsSolidXY(const SGPLevelCollisionData *level, s16 tile_x, s16 tile_y, bool oob_x_is_solid, bool oob_y_is_solid)
{
    const u16 row_len = level->row_length;
    const u16 total_rows = SGP_LevelTotalRows(level);
    if (tile_x < 0 || (u16)tile_x >= row_len)
        return oob_x_is_solid;
    if (tile_y < 0 || (u16)tile_y >= total_rows)
        return oob_y_is_solid;

    u32 idx = (u32)(u16)tile_y * (u32)row_len + (u32)(u16)tile_x;
    if (idx >= level->data_length)
        return oob_x_is_solid || oob_y_is_solid; // Safety
    return level->collision_data[idx] == SOLID_TILE;
}

static inline bool SGP_TileIsSolid(const SGPLevelCollisionData *level, s16 tile_x, s16 tile_y, bool oob_is_solid)
{
    return SGP_TileIsSolidXY(level, tile_x, tile_y, oob_is_solid, oob_is_solid);
}

/**
 * Checks for player collision against tiles using post-move collision.
 * Call this AFTER adjusting position for the intended direction; if true, undo that axis move.
 */
static inline bool SGP_PlayerLevelCollision(
    u16 player_index, s16 player_coll_x, s16 player_coll_y, u16 player_coll_width, u16 player_coll_height,
    const SGPLevelCollisionData *level, SGPMovementDirection direction)
{
    // Per-player collision flags for early return when collision already detected and position unchanged
    static u16 prev_collide_flags[SGP_MAX_PLAYER_COUNT] = {0};
    static s16 prev_x[SGP_MAX_PLAYER_COUNT] = {0};
    static s16 prev_y[SGP_MAX_PLAYER_COUNT] = {0};

    // Tile bounds of the player's rectangle (allow per-entity collision box insets)
    const s16 col_left_px = (s16)(player_coll_x);
    const s16 col_right_px = (s16)(player_coll_x + (s16)player_coll_width - 1);
    const s16 col_top_px = (s16)(player_coll_y);
    const s16 col_bottom_px = (s16)(player_coll_y + (s16)player_coll_height - 1);

    const s16 tile_left = col_left_px >> PIXELS_TO_TILE_SHIFT;
    const s16 tile_right = col_right_px >> PIXELS_TO_TILE_SHIFT;
    const s16 tile_top = col_top_px >> PIXELS_TO_TILE_SHIFT;
    const s16 tile_bottom = col_bottom_px >> PIXELS_TO_TILE_SHIFT;

    if (direction & SGP_DIR_LEFT)
    {
        // Only check if position changed or not already flagged
        if (prev_x[player_index] == player_coll_x && prev_y[player_index] == player_coll_y && FLAG_IS_ACTIVE(prev_collide_flags[player_index], COLLIDE_LEFT))
            return true;
        bool isColliding = SGP_TileIsSolidXY(level, tile_left, tile_top, SGP_OOB_HORIZONTAL_SOLID, SGP_OOB_HORIZONTAL_PASSABLE) ||
                           SGP_TileIsSolidXY(level, tile_left, tile_bottom, SGP_OOB_HORIZONTAL_SOLID, SGP_OOB_HORIZONTAL_PASSABLE);
        prev_x[player_index] = player_coll_x;
        prev_y[player_index] = player_coll_y;
        if (isColliding)
            SET_ACTIVE(prev_collide_flags[player_index], COLLIDE_LEFT);
        else
            SET_INACTIVE(prev_collide_flags[player_index], COLLIDE_LEFT);
        return isColliding;
    }
    else if (direction & SGP_DIR_RIGHT)
    {
        if (prev_x[player_index] == player_coll_x && prev_y[player_index] == player_coll_y && FLAG_IS_ACTIVE(prev_collide_flags[player_index], COLLIDE_RIGHT))
            return true;
        bool isColliding = SGP_TileIsSolidXY(level, tile_right, tile_top, SGP_OOB_HORIZONTAL_SOLID, SGP_OOB_HORIZONTAL_PASSABLE) ||
                           SGP_TileIsSolidXY(level, tile_right, tile_bottom, SGP_OOB_HORIZONTAL_SOLID, SGP_OOB_HORIZONTAL_PASSABLE);
        prev_x[player_index] = player_coll_x;
        prev_y[player_index] = player_coll_y;
        if (isColliding)
            SET_ACTIVE(prev_collide_flags[player_index], COLLIDE_RIGHT);
        else
            SET_INACTIVE(prev_collide_flags[player_index], COLLIDE_RIGHT);
        return isColliding;
    }

    if (direction & SGP_DIR_UP)
    {
        if (prev_x[player_index] == player_coll_x && prev_y[player_index] == player_coll_y && FLAG_IS_ACTIVE(prev_collide_flags[player_index], COLLIDE_UP))
            return true;
        bool isColliding = SGP_TileIsSolidXY(level, tile_left, tile_top, SGP_OOB_HORIZONTAL_SOLID, SGP_OOB_HORIZONTAL_SOLID) ||
                           SGP_TileIsSolidXY(level, tile_right, tile_top, SGP_OOB_HORIZONTAL_SOLID, SGP_OOB_HORIZONTAL_SOLID);
        prev_x[player_index] = player_coll_x;
        prev_y[player_index] = player_coll_y;
        if (isColliding)
            SET_ACTIVE(prev_collide_flags[player_index], COLLIDE_UP);
        else
            SET_INACTIVE(prev_collide_flags[player_index], COLLIDE_UP);
        return isColliding;
    }
    else if (direction & SGP_DIR_DOWN)
    {
        if (prev_x[player_index] == player_coll_x && prev_y[player_index] == player_coll_y && FLAG_IS_ACTIVE(prev_collide_flags[player_index], COLLIDE_DOWN))
            return true;
        bool isColliding = SGP_TileIsSolidXY(level, tile_left, tile_bottom, SGP_OOB_HORIZONTAL_SOLID, SGP_OOB_HORIZONTAL_SOLID) ||
                           SGP_TileIsSolidXY(level, tile_right, tile_bottom, SGP_OOB_HORIZONTAL_SOLID, SGP_OOB_HORIZONTAL_SOLID);
        prev_x[player_index] = player_coll_x;
        prev_y[player_index] = player_coll_y;
        if (isColliding)
            SET_ACTIVE(prev_collide_flags[player_index], COLLIDE_DOWN);
        else
            SET_INACTIVE(prev_collide_flags[player_index], COLLIDE_DOWN);
        return isColliding;
    }
    // Fallback: general rectangle-solid overlap (treat OOB as solid)
    return SGP_TileIsSolid(level, tile_left, tile_top, SGP_OOB_HORIZONTAL_SOLID) ||
           SGP_TileIsSolid(level, tile_right, tile_top, SGP_OOB_HORIZONTAL_SOLID) ||
           SGP_TileIsSolid(level, tile_left, tile_bottom, SGP_OOB_HORIZONTAL_SOLID) ||
           SGP_TileIsSolid(level, tile_right, tile_bottom, SGP_OOB_HORIZONTAL_SOLID);
}

#endif // SGP_H