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
 *          helpers for Genesis games and demos.
 */
#ifndef SGP_H
#define SGP_H

#include <genesis.h>

#define BUTTON_NONE 0x0000

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
/**
 * @brief Input state for joypads.
 */
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

//----------------------------------------------------------------------------------
// Input Functions
//----------------------------------------------------------------------------------

/**
 * @brief Global platform state (must be defined in one .c file).
 */
extern SGP sgp;

/**
 * @brief Polls and updates the current and previous state for both controllers.
 *
 * Call this once per frame before reading input.
 */
static inline void SGP_PollInput(void) {
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
static inline bool SGP_ButtonPressed(u16 joy, u16 button) {
    static u16* const state_ptrs[2] = { &sgp.input.joy1_state, &sgp.input.joy2_state };
    static u16* const prev_ptrs[2]  = { &sgp.input.joy1_previous, &sgp.input.joy2_previous };
    u16 state = *state_ptrs[joy & 1];
    u16 prev  = *prev_ptrs[joy & 1];
    return ((state & button) & ~(prev & button)) != 0;
}

/**
 * @brief Returns true if the specified button(s) were just released for the given joypad.
 *
 * @param joy JOY_1 or JOY_2
 * @param button Button mask
 */
static inline bool SGP_ButtonReleased(u16 joy, u16 button) {
    static u16* const state_ptrs[2] = { &sgp.input.joy1_state, &sgp.input.joy2_state };
    static u16* const prev_ptrs[2]  = { &sgp.input.joy1_previous, &sgp.input.joy2_previous };
    u16 state = *state_ptrs[joy & 1];
    u16 prev  = *prev_ptrs[joy & 1];
    return ((~state & button) & (prev & button)) != 0;
}

/**
 * @brief Returns true if the specified button(s) are currently held down for the given joypad.
 *
 * @param joy JOY_1 or JOY_2
 * @param button Button mask
 */
static inline bool SGP_ButtonDown(u16 joy, u16 button) {
    static u16* const state_ptrs[2] = { &sgp.input.joy1_state, &sgp.input.joy2_state };
    u16 state = *state_ptrs[joy & 1];
    return (state & button) != 0;
}

//----------------------------------------------------------------------------------
// Platform Initialization
//----------------------------------------------------------------------------------

/**
 * @brief Initializes default Sega Genesis Platform Abstraction Layer (SGPal) state.
 * Call once at startup.
 */
static inline void SGP_init(void) {
    sgp.input.joy1_state = 0;
    sgp.input.joy2_state = 0;
    sgp.input.joy1_previous = 0;
    sgp.input.joy2_previous = 0;
}

#endif // SGP_H
