/*
 * sgp_test.h - Test wrapper for SGP header with mock SGDK dependencies
 * 
 * This header provides mock SGDK dependencies and includes SGP for testing
 * outside of the full SGDK environment.
 */

#ifndef SGP_TEST_H
#define SGP_TEST_H

// Enable test mode to skip genesis.h inclusion
#define SGP_TEST_MODE

#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

// SGDK type definitions for mock environment
typedef unsigned char u8;
typedef unsigned short u16;  
typedef unsigned int u32;
typedef signed short s16;
typedef signed int s32;
typedef int fix16;
typedef int fix32;

// Controller definitions
#define JOY_1 0
#define JOY_2 1
#define BUTTON_A 0x0040
#define BUTTON_B 0x0010
#define BUTTON_UP 0x0001
#define BUTTON_DOWN 0x0002
#define BUTTON_LEFT 0x0004
#define BUTTON_RIGHT 0x0008

// Screen and VDP definitions
#define screenWidth 320
#define screenHeight 224
#define BG_B 1
#define WINDOW 0
#define PAL1 1
#define DMA 1

// Fixed-point macros
#define F32_toInt(x) ((int)(x))
#define FIX32(x) ((fix32)(x))

// Map and Sprite structures
typedef struct { 
    void* data; 
    u16 w; 
    u16 h; 
} Map;

typedef struct { 
    void* data; 
} Sprite;

// Mock SGDK function declarations
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

// Prevent genesis.h inclusion by defining its guard
#define GENESIS_H

// Now include SGP header (which will skip genesis.h due to guard)
#include "../sgp.h"

#endif // SGP_TEST_H
