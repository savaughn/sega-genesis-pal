/*
 * smoke_test.c - Basic smoke test for SGP header-only library
 * 
 * This test validates that sgp.h compiles correctly and basic functions work.
 * It includes mock SGDK dependencies to allow compilation outside of the 
 * full SGDK environment.
 */

#include "sgp_test.h"

// Mock SGDK function implementations
u16 JOY_readJoypad(u16 joy) { 
    (void)joy; 
    return 0; 
}

void MAP_scrollTo(Map* map, u32 x, u32 y) { 
    (void)map; (void)x; (void)y; 
}

void VDP_drawText(const char* str, u16 x, u16 y) { 
    (void)str; (void)x; (void)y; 
}

void SYS_doVBlankProcess(void) {}

void VDP_setHorizontalScroll(u16 bg, s16 scroll) { 
    (void)bg; (void)scroll; 
}

void VDP_setVerticalScroll(u16 bg, s16 scroll) { 
    (void)bg; (void)scroll; 
}

void SPR_setPosition(Sprite* sprite, s16 x, s16 y) { 
    (void)sprite; (void)x; (void)y; 
}

void VDP_setWindowVPos(bool enable, u16 pos) { 
    (void)enable; (void)pos; 
}

void VDP_drawTextEx(u16 plane, const char* str, u16 attr, u16 x, u16 y, u16 method) { 
    (void)plane; (void)str; (void)attr; (void)x; (void)y; (void)method; 
}

u16 TILE_ATTR(u16 pal, bool priority, bool flipV, bool flipH) { 
    (void)pal; (void)priority; (void)flipV; (void)flipH; 
    return 0; 
}

// Required global SGP state
SGP sgp;

// Test functions
bool test_sgp_init() {
    printf("Testing SGP_init()... ");
    
    // Initialize SGP
    SGP_init();
    
    // Verify default state
    if (sgp.input.joy1_state != 0 || 
        sgp.input.joy2_state != 0 ||
        sgp.input.joy1_previous != 0 ||
        sgp.input.joy2_previous != 0) {
        printf("FAIL - Input state not properly initialized\n");
        return false;
    }
    
    if (sgp.camera.active != false ||
        sgp.camera.map != NULL ||
        sgp.camera.current_x != 0 ||
        sgp.camera.current_y != 0) {
        printf("FAIL - Camera state not properly initialized\n");
        return false;
    }
    
    printf("PASS\n");
    return true;
}

bool test_input_polling() {
    printf("Testing SGP_PollInput()... ");
    
    // Test input polling doesn't crash
    SGP_PollInput();
    
    // Since JOY_readJoypad returns 0, states should remain 0
    if (sgp.input.joy1_state != 0 || sgp.input.joy2_state != 0) {
        printf("FAIL - Input polling returned unexpected values\n");
        return false;
    }
    
    printf("PASS\n");
    return true;
}

bool test_button_functions() {
    printf("Testing button functions... ");
    
    // Test button functions with known state (all 0)
    if (SGP_ButtonPressed(JOY_1, BUTTON_A) ||
        SGP_ButtonReleased(JOY_1, BUTTON_A) ||
        SGP_ButtonDown(JOY_1, BUTTON_A)) {
        printf("FAIL - Button functions returned unexpected values\n");
        return false;
    }
    
    printf("PASS\n");
    return true;
}

bool test_collision_helpers() {
    printf("Testing collision helper functions... ");
    
    // Test collision data structure
    const u8 test_data[] = {1, 0, 1, 0};
    SGPLevelCollisionData test_level = {
        .row_length = 2,
        .data_length = 4,
        .collision_data = test_data
    };
    
    // Test helper functions
    u16 total_rows = SGP_LevelTotalRows(&test_level);
    if (total_rows != 2) {
        printf("FAIL - SGP_LevelTotalRows returned %d, expected 2\n", total_rows);
        return false;
    }
    
    // Test tile solidity checks
    bool solid = SGP_TileIsSolid(&test_level, 0, 0, true);
    if (!solid) {
        printf("FAIL - Expected solid tile at (0,0)\n");
        return false;
    }
    
    bool empty = SGP_TileIsSolid(&test_level, 1, 0, true);
    if (empty) {
        printf("FAIL - Expected empty tile at (1,0)\n");
        return false;
    }
    
    printf("PASS\n");
    return true;
}

bool test_box_collision() {
    printf("Testing box collision... ");
    
    SGPBox box1 = {10, 10, 20, 20};   // (10,10) to (29,29)
    SGPBox box2 = {35, 35, 20, 20};   // (35,35) to (54,54) - Not overlapping
    SGPBox box3 = {15, 15, 20, 20};   // (15,15) to (34,34) - Overlapping
    
    if (SGP_CheckBoxCollision(&box1, &box2)) {
        printf("FAIL - Detected collision between non-overlapping boxes\n");
        printf("  Box1: (%d,%d) %dx%d, Box2: (%d,%d) %dx%d\n", 
               box1.x, box1.y, box1.w, box1.h, box2.x, box2.y, box2.w, box2.h);
        return false;
    }
    
    if (!SGP_CheckBoxCollision(&box1, &box3)) {
        printf("FAIL - Failed to detect collision between overlapping boxes\n");
        printf("  Box1: (%d,%d) %dx%d, Box3: (%d,%d) %dx%d\n", 
               box1.x, box1.y, box1.w, box1.h, box3.x, box3.y, box3.w, box3.h);
        return false;
    }
    
    printf("PASS\n");
    return true;
}

bool test_metatile_conversion() {
    printf("Testing metatile conversion... ");
    
    u16 pixels = SGP_MetatilesToPixels(1);
    if (pixels != 128) {
        printf("FAIL - Expected 128 pixels, got %d\n", pixels);
        return false;
    }
    
    printf("PASS\n");
    return true;
}

#ifdef DEBUG
bool test_debug_functions() {
    printf("Testing debug functions... ");
    
    // Test debug toggle
    SGP_ToggleDebug();
    bool debug_state1 = SGP_isDebugEnabled();
    
    SGP_ToggleDebug();
    bool debug_state2 = SGP_isDebugEnabled();
    
    if (debug_state1 == debug_state2) {
        printf("FAIL - Debug toggle not working\n");
        return false;
    }
    
    // Test debug print (should not crash)
    SGP_DebugPrint("Test", 0, 0);
    
    printf("PASS\n");
    return true;
}
#endif

int main() {
    printf("=== SGP Smoke Test Suite ===\n\n");
    
    int tests_run = 0;
    int tests_passed = 0;
    
    // Run all tests
    if (test_sgp_init()) tests_passed++;
    tests_run++;
    
    if (test_input_polling()) tests_passed++;
    tests_run++;
    
    if (test_button_functions()) tests_passed++;
    tests_run++;
    
    if (test_collision_helpers()) tests_passed++;
    tests_run++;
    
    if (test_box_collision()) tests_passed++;
    tests_run++;
    
    if (test_metatile_conversion()) tests_passed++;
    tests_run++;
    
#ifdef DEBUG
    if (test_debug_functions()) tests_passed++;
    tests_run++;
#endif
    
    // Print summary
    printf("\n=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);
    printf("Success rate: %.1f%%\n", (float)tests_passed / tests_run * 100.0f);
    
    if (tests_passed == tests_run) {
        printf("\n✓ All smoke tests passed!\n");
        printf("SGP header compiles and basic functions work correctly.\n");
        return 0;
    } else {
        printf("\n✗ Some smoke tests failed!\n");
        return 1;
    }
}
