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
 * Purpose: Header-only platform abstraction layer for Sega Genesis development
 *          using SGDK. Provides ergonomic, high-performance input and camera
 *          helpers for Genesis development.
 */
#ifndef SGP_H
#define SGP_H

#include <genesis.h>

#define BUTTON_NONE 0x0000
#define VDP_SPRITE_OFFSET 0x80 // Offset for sprite coordinates in VDP

// Each metatile is 16x16 pixels, so 128x128 pixels block is 8x8 metatiles
static inline s16 __inPx(s16 x) { return x << 7; }

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
} input;

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
    fix16 offset_x;  // Camera offset X
    fix32 offset_y;  // Camera offset Y
    fix32 *target_x; // Camera target X
    fix32 *target_y; // Camera target Y
    u8 type;         // Camera type (e.g. CAMERA_SMOOTH)
    s16 current_x;   // Camera X position (integer for MAP_scrollTo)
    s16 current_y;   // Camera Y position (integer for MAP_scrollTo)
    Sprite *sprite;
    s16 sprite_width;  // Width of the sprite being followed
    s16 sprite_height; // Height of the sprite being followed
    bool active;
    SGPMap *map; // Pointer to the current map being viewed
    s8 max_vertical_scroll; // in Tiles (default 32), used to limit camera scroll
} SGPCamera;

/**
 * @brief Platform-wide state for input and camera.
 *
 * Holds current and previous joypad states for both controllers,
 * as well as camera position, zoom, and rotation in fixed-point.
 */
typedef struct
{
    input input;      // Input state
    SGPCamera camera; // Camera state
} SGP;

/**
 * @brief Global platform state (must be defined in one .c file).
 */
extern SGP sgp;

static bool showDebug = FALSE;
static inline void toggleDebug(void)
{
    showDebug = showDebug ? FALSE : TRUE;
}
static inline void SGP_DebugPrint(const char *text, s16 x, s16 y)
{
    if (showDebug)
    {
        VDP_setWindowVPos(FALSE, 4);
        VDP_drawTextEx(WINDOW, text, TILE_ATTR(PAL1, FALSE, FALSE, FALSE), x, y, DMA);
    }
    else
    {
        VDP_setWindowVPos(FALSE, 0);
    }
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
    static u16 *const state_ptrs[2] = {&sgp.input.joy1_state, &sgp.input.joy2_state};
    static u16 *const prev_ptrs[2] = {&sgp.input.joy1_previous, &sgp.input.joy2_previous};
    u16 state = *state_ptrs[joy & 1];
    u16 prev = *prev_ptrs[joy & 1];
    return ((state & button) & ~(prev & button)) != 0;
}

/**
 * @brief Returns true if the specified button(s) were just released for the given joypad.
 *
 * @param joy JOY_1 or JOY_2
 * @param button Button mask
 */
static inline bool SGP_ButtonReleased(u16 joy, u16 button)
{
    static u16 *const state_ptrs[2] = {&sgp.input.joy1_state, &sgp.input.joy2_state};
    static u16 *const prev_ptrs[2] = {&sgp.input.joy1_previous, &sgp.input.joy2_previous};
    u16 state = *state_ptrs[joy & 1];
    u16 prev = *prev_ptrs[joy & 1];
    return ((~state & button) & (prev & button)) != 0;
}

/**
 * @brief Returns true if the specified button(s) are currently held down for the given joypad.
 *
 * @param joy JOY_1 or JOY_2
 * @param button Button mask
 */
static inline bool SGP_ButtonDown(u16 joy, u16 button)
{
    static u16 *const state_ptrs[2] = {&sgp.input.joy1_state, &sgp.input.joy2_state};
    u16 state = *state_ptrs[joy & 1];
    return (state & button) != 0;
}

//----------------------------------------------------------------------------------
// Camera Functions (Fixed Point for Genesis)
//----------------------------------------------------------------------------------
typedef struct
{
    Sprite *sprite;      // Sprite to follow
    fix32 *target_x_ptr; // Target X position (fixed-point)
    fix32 *target_y_ptr; // Target Y Position (fixed-point)
    s16 sprite_width;    // Width of the sprite being followed
    s16 sprite_height;   // Height of the sprite being followed
} SGPCameraTarget;

/**
 * @brief Initializes a Camera struct with default values and a given target position.
 * @param cam Pointer to Camera struct
 * @param current_x   Target X position
 * @param current_y   Target Y position
 */
static inline void SGP_CameraInit(const Map *map)
{
    sgp.camera.map->current = map;
    sgp.camera.map->height = __inPx(map->h);
    sgp.camera.map->width = __inPx(map->w);
    sgp.camera.active = TRUE;
}
/**
 * @brief Clamps the given fixed-point position to the map bounds.
 * @param x Pointer to X position (fixed-point)
 * @param y Pointer to Y position (fixed-point)
 * @param width Width of the entity
 * @param height Height of the entity
 */
static inline void SGP_ClampPositionToMapBounds(fix32 *x, fix32 *y, s16 width, s16 height)
{
    s16 pos_x = F32_toInt(*x);
    s16 pos_y = F32_toInt(*y);

    // Clamp position to map bounds (entity always visible on map, accounting for sprite dimensions)
    if (pos_x < 0)
        *x = FIX32(0);
    if (pos_x > sgp.camera.map->width - 1 - width)
        *x = FIX32(sgp.camera.map->width - 1 - width);
    if (pos_y < 0)
        *y = FIX32(0);
    if (pos_y > sgp.camera.map->height - height)
        *y = FIX32(sgp.camera.map->height - height);
}

/**
 * @brief Follows a target position with the camera, clamping to map bounds.
 * @param target Pointer to CameraTarget struct
 */
static inline void SGP_CameraFollowTarget(SGPCameraTarget *target)
{
    if (!sgp.camera.active)
    {
        return; // Camera not active, skip following
    }
    s16 target_x_map = F32_toInt(*target->target_x_ptr);
    s16 target_y_map = F32_toInt(*target->target_y_ptr);

    // Center camera on target, but clamp camera to map bounds
    s16 new_camera_x = target_x_map - (screenWidth / 2) + (target->sprite_width / 2);
    s16 new_camera_y = target_y_map - (screenHeight / 2) + (target->sprite_height / 2);

    if (new_camera_x < 0)
        new_camera_x = 0;
    if (new_camera_x > sgp.camera.map->width - screenWidth)
        new_camera_x = sgp.camera.map->width - screenWidth;
    if (new_camera_y < 0)
        new_camera_y = 0;
    if (new_camera_y > sgp.camera.map->height - screenHeight)
        new_camera_y = sgp.camera.map->height - screenHeight;

    if ((sgp.camera.current_x != new_camera_x) ||
        (sgp.camera.current_y != new_camera_y))
    {
        sgp.camera.current_x = new_camera_x;
        sgp.camera.current_y = new_camera_y;

        static s16 bg_hscroll = 0, bg_vscroll = 0;
        bg_hscroll = (0 - new_camera_x) >> 3; // Convert to tile units (8 pixels)
        bg_vscroll = new_camera_y >> 3;       // Convert to tile units (8

        if (bg_vscroll > 32) // assuming
        {
            bg_vscroll = 32;
        }

        MAP_scrollTo(sgp.camera.map->current, new_camera_x, new_camera_y);
        VDP_setHorizontalScroll(BG_B, bg_hscroll);
        VDP_setVerticalScroll(BG_B, bg_vscroll);
    }
    if (target->sprite)
    {
        SPR_setPosition(target->sprite,
                        target_x_map - new_camera_x,
                        target_y_map - new_camera_y);
    }
}

/**
 * @brief Activates the camera, allowing it to follow a target.
 */
static inline void SGP_activateCamera(void)
{
    sgp.camera.active = TRUE;
}
/**
 * @brief Deactivates the camera, stopping it from following a target.
 */
static inline void SGP_deactivateCamera(void)
{
    sgp.camera.active = FALSE;
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
static inline void SGP_UpdateCameraPosition(s16 x, s16 y)
{
    if (sgp.camera.active)
    {
        return; // if active, camera tracks target
    }
    sgp.camera.current_x = x;
    sgp.camera.current_y = y;
    MAP_scrollTo(sgp.camera.map->current, x, y);
}
/**
 * @brief Sets the horizontal scroll limit for the camera.
 * @param limit New vertical scroll limit in tiles
 */
static inline void SGP_CameraSetVerticalScrollLimit(s16 limit)
{
    if (limit < 0)
    {
        limit = 0;
    }
    sgp.camera.max_vertical_scroll = limit;
}

/**
 * @brief Gets the vertical scroll limit for the camera.
 * @return Current vertical scroll limit in tiles
 */
static inline s16 SGP_CameraGetVerticalScrollLimit(void)
{
    return sgp.camera.max_vertical_scroll;
}


/**
 * @brief Shakes the camera for a specified duration and intensity.
 * @param duration Duration of the shake in frames
 * @param intensity intensity of the shake (in pixels)
 */
static inline void SGP_ShakeCamera(s16 duration, s16 intensity)
{
    SGP_deactivateCamera(); // Disable camera tracking during shake
    for (s16 i = 0; i < duration; i++)
    {
        s16 shake_x = (i % 2 == 0) ? intensity : -intensity;
        sgp.camera.current_x += shake_x;
        SGP_UpdateCameraPosition(sgp.camera.current_x, sgp.camera.current_y);
        SYS_doVBlankProcess();           // Wait for VBlank to apply shake
        sgp.camera.current_x -= shake_x; // Restore position for next frame
    }
    SGP_activateCamera(); // Re-enable camera tracking after shake
}

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
    sgp.camera.offset_x = 0;
    sgp.camera.offset_y = 0;
    sgp.camera.target_x = 0;
    sgp.camera.target_y = 0;
    sgp.camera.current_x = 0;
    sgp.camera.current_y = 0;
    sgp.camera.active = FALSE;
    static Map defaultMap = {
        .w = 0,
        .h = 0,
        .metaTiles = NULL,
        .blocks = NULL,
        .blockIndexes = NULL,
        .blockRowOffsets = NULL,
        .plane = BG_A,
        .baseTile = 0,
        .posX = 0,
        .posY = 0,
        .wMask = 0,
        .hMask = 0,
        .planeWidth = 0,
        .planeHeight = 0,
        .planeWidthMaskAdj = 0,
        .planeHeightMaskAdj = 0,
        .planeWidthSftAdj = 0,
        .firstUpdate = TRUE,
        .lastXT = 0,
        .lastYT = 0,
    };
    if (!sgp.camera.map)
    {
        static SGPMap defaultSGPMap;
        sgp.camera.map = &defaultSGPMap;
    }
    sgp.camera.map->current = &defaultMap;
    sgp.camera.map->previous = &defaultMap;
    sgp.camera.map->height = 0;
    sgp.camera.map->width = 0;
    sgp.camera.max_vertical_scroll = 32;
}

#endif // SGP_H
