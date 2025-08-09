/*
 * collision_test.c - Comprehensive collision test for SGP_PlayerLevelCollision
 * 
 * This test validates collision detection, caching behavior, and edge cases
 * for the SGP collision system using a mock SGDK environment.
 */

#include "sgp_test.h"

// Mock SGDK function implementations
u16 JOY_readJoypad(u16 joy) { (void)joy; return 0; }
void MAP_scrollTo(Map* map, u32 x, u32 y) { (void)map; (void)x; (void)y; }
void VDP_drawText(const char* str, u16 x, u16 y) { (void)str; (void)x; (void)y; }
void SYS_doVBlankProcess(void) {}
void VDP_setHorizontalScroll(u16 bg, s16 scroll) { (void)bg; (void)scroll; }
void VDP_setVerticalScroll(u16 bg, s16 scroll) { (void)bg; (void)scroll; }
void SPR_setPosition(Sprite* sprite, s16 x, s16 y) { (void)sprite; (void)x; (void)y; }
void VDP_setWindowVPos(bool enable, u16 pos) { (void)enable; (void)pos; }
void VDP_drawTextEx(u16 plane, const char* str, u16 attr, u16 x, u16 y, u16 method) { 
    (void)plane; (void)str; (void)attr; (void)x; (void)y; (void)method; }
u16 TILE_ATTR(u16 pal, bool priority, bool flipV, bool flipH) { 
    (void)pal; (void)priority; (void)flipV; (void)flipH; return 0; }

// Required global SGP state
SGP sgp;

// Test collision data: 8x8 grid representing a platformer level
const u8 test_level_data[] = {
    1, 1, 1, 1, 1, 1, 1, 1,  // Row 0: Top boundary
    1, 0, 0, 0, 0, 0, 0, 1,  // Row 1: Open corridor
    1, 0, 1, 1, 1, 1, 0, 1,  // Row 2: Inner walls
    1, 0, 1, 0, 0, 1, 0, 1,  // Row 3: Rooms with walls
    1, 0, 1, 0, 0, 1, 0, 1,  // Row 4: Rooms with walls
    1, 0, 1, 1, 1, 1, 0, 1,  // Row 5: Inner walls
    1, 0, 0, 0, 0, 0, 0, 1,  // Row 6: Open corridor
    1, 1, 1, 1, 1, 1, 1, 1   // Row 7: Bottom boundary
};

SGPLevelCollisionData test_level = {
    .row_length = 8,
    .data_length = 64,
    .collision_data = test_level_data
};

// Test result tracking
int tests_run = 0;
int tests_passed = 0;

void print_test_result(const char* test_name, bool expected, bool actual) {
    tests_run++;
    bool passed = (expected == actual);
    if (passed) tests_passed++;
    
    printf("Test: %-35s Expected: %-12s Got: %-12s - %s\n", 
           test_name, 
           expected ? "COLLISION" : "NO_COLLISION",
           actual ? "COLLISION" : "NO_COLLISION",
           passed ? "PASS" : "FAIL");
    
    if (!passed) {
        printf("  *** TEST FAILED ***\n");
    }
}

void test_basic_collisions() {
    printf("\n=== Basic Collision Tests ===\n");
    
    const u16 player_width = 16;
    const u16 player_height = 16;
    const u16 player_index = 0;
    
    // Test in empty space (tile 1,1 = pixel 16,16)
    bool result = SGP_PlayerLevelCollision(player_index, 16, 16, player_width, player_height, &test_level, SGP_DIR_LEFT);
    print_test_result("Empty space - move left", false, result);
    
    // Test hitting walls
    result = SGP_PlayerLevelCollision(player_index, 0, 16, player_width, player_height, &test_level, SGP_DIR_LEFT);
    print_test_result("Hit left boundary wall", true, result);
    
    result = SGP_PlayerLevelCollision(player_index, 112, 16, player_width, player_height, &test_level, SGP_DIR_RIGHT);
    print_test_result("Hit right boundary wall", true, result);
    
    result = SGP_PlayerLevelCollision(player_index, 16, 0, player_width, player_height, &test_level, SGP_DIR_UP);
    print_test_result("Hit top boundary wall", true, result);
    
    result = SGP_PlayerLevelCollision(player_index, 16, 112, player_width, player_height, &test_level, SGP_DIR_DOWN);
    print_test_result("Hit bottom boundary wall", true, result);
}

void test_corridor_movement() {
    printf("\n=== Corridor Movement Tests ===\n");
    
    const u16 player_width = 16;
    const u16 player_height = 16;
    const u16 player_index = 0;
    
    // Test movement in open corridor (row 1: empty spaces)
    bool result = SGP_PlayerLevelCollision(player_index, 16, 16, player_width, player_height, &test_level, SGP_DIR_RIGHT);
    print_test_result("Corridor move right", false, result);
    
    result = SGP_PlayerLevelCollision(player_index, 32, 16, player_width, player_height, &test_level, SGP_DIR_LEFT);
    print_test_result("Corridor move left", false, result);
    
    // Test hitting inner walls (moving from open space into wall)
    result = SGP_PlayerLevelCollision(player_index, 32, 48, player_width, player_height, &test_level, SGP_DIR_LEFT);
    print_test_result("Hit inner wall moving left", true, result);
    
    // Test movement in inner rooms
    result = SGP_PlayerLevelCollision(player_index, 48, 48, player_width, player_height, &test_level, SGP_DIR_RIGHT);
    print_test_result("Inner room move right", false, result);
}

void test_out_of_bounds() {
    printf("\n=== Out of Bounds Tests ===\n");
    
    const u16 player_width = 16;
    const u16 player_height = 16;
    const u16 player_index = 0;
    
    // Horizontal OOB should be solid
    bool result = SGP_PlayerLevelCollision(player_index, -16, 32, player_width, player_height, &test_level, SGP_DIR_LEFT);
    print_test_result("OOB left - horizontal movement", true, result);
    
    result = SGP_PlayerLevelCollision(player_index, 128, 32, player_width, player_height, &test_level, SGP_DIR_RIGHT);
    print_test_result("OOB right - horizontal movement", true, result);
    
    // Vertical OOB should allow horizontal movement (axis-specific behavior)
    result = SGP_PlayerLevelCollision(player_index, 32, -16, player_width, player_height, &test_level, SGP_DIR_LEFT);
    print_test_result("OOB top - horizontal movement", false, result);
    
    result = SGP_PlayerLevelCollision(player_index, 32, 128, player_width, player_height, &test_level, SGP_DIR_RIGHT);
    print_test_result("OOB bottom - horizontal movement", false, result);
    
    // Vertical movement into vertical OOB should be solid
    result = SGP_PlayerLevelCollision(player_index, 32, -16, player_width, player_height, &test_level, SGP_DIR_UP);
    print_test_result("OOB top - vertical movement", true, result);
    
    result = SGP_PlayerLevelCollision(player_index, 32, 128, player_width, player_height, &test_level, SGP_DIR_DOWN);
    print_test_result("OOB bottom - vertical movement", true, result);
}

void test_multiplayer_support() {
    printf("\n=== Multi-Player Support Tests ===\n");
    
    const u16 player_width = 16;
    const u16 player_height = 16;
    
    // Test that different players maintain separate collision state
    bool result1 = SGP_PlayerLevelCollision(0, 16, 16, player_width, player_height, &test_level, SGP_DIR_LEFT);
    bool result2 = SGP_PlayerLevelCollision(1, 16, 16, player_width, player_height, &test_level, SGP_DIR_LEFT);
    
    print_test_result("Player 0 - empty space", false, result1);
    print_test_result("Player 1 - empty space", false, result2);
    
    // Test that collision flags are per-player
    SGP_PlayerLevelCollision(0, 0, 16, player_width, player_height, &test_level, SGP_DIR_LEFT); // Player 0 hits wall
    result2 = SGP_PlayerLevelCollision(1, 16, 16, player_width, player_height, &test_level, SGP_DIR_LEFT); // Player 1 in open space
    
    print_test_result("Player 1 unaffected by Player 0 collision", false, result2);
}

void test_collision_caching() {
    printf("\n=== Collision Caching Tests ===\n");
    
    const u16 player_width = 16;
    const u16 player_height = 16;
    const u16 player_index = 0;
    
    // First call should calculate collision
    bool result1 = SGP_PlayerLevelCollision(player_index, 0, 16, player_width, player_height, &test_level, SGP_DIR_LEFT);
    // Second call at same position should use cached result
    bool result2 = SGP_PlayerLevelCollision(player_index, 0, 16, player_width, player_height, &test_level, SGP_DIR_LEFT);
    
    print_test_result("First collision check", true, result1);
    print_test_result("Cached collision result", true, result2);
    
    // Position change should recalculate
    bool result3 = SGP_PlayerLevelCollision(player_index, 16, 16, player_width, player_height, &test_level, SGP_DIR_LEFT);
    print_test_result("Position changed - recalculated", false, result3);
}

void test_axis_specific_sampling() {
    printf("\n=== Axis-Specific Sampling Tests ===\n");
    
    const u16 player_width = 16;
    const u16 player_height = 16;
    const u16 player_index = 0;
    
    // Test that horizontal movement uses center sampling to avoid wall-sticking
    // Position where player touches ground but should still move horizontally
    bool result = SGP_PlayerLevelCollision(player_index, 16, 96, player_width, player_height, &test_level, SGP_DIR_RIGHT);
    print_test_result("Ground contact - horizontal movement", false, result);
    
    // Test that vertical movement checks full width
    result = SGP_PlayerLevelCollision(player_index, 16, 96, player_width, player_height, &test_level, SGP_DIR_DOWN);
    print_test_result("Ground contact - vertical movement", true, result);
}

int main() {
    printf("=== SGP Comprehensive Collision Test Suite ===\n");
    
    // Initialize SGP
    SGP_init();
    
    printf("\nTest level layout (8x8 tiles, 16px each):\n");
    printf("# = solid, . = empty\n");
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            printf("%c ", test_level_data[y * 8 + x] ? '#' : '.');
        }
        printf("\n");
    }
    
    // Run all test suites
    test_basic_collisions();
    test_corridor_movement();
    test_out_of_bounds();
    test_multiplayer_support();
    test_collision_caching();
    test_axis_specific_sampling();
    
    // Summary
    printf("\n=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);
    printf("Success rate: %.1f%%\n", (float)tests_passed / tests_run * 100.0f);
    
    if (tests_passed == tests_run) {
        printf("\n✓ All tests passed!\n");
        return 0;
    } else {
        printf("\n✗ Some tests failed!\n");
        return 1;
    }
}
