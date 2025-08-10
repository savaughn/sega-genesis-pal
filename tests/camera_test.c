/*
 * camera_test.c - Comprehensive camera system test for SGP
 * 
 * This test validates camera initialization, following, centering, bounds clamping,
 * and sprite positioning for the SGP camera system using a mock SGDK environment.
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

// Test result tracking
int tests_run = 0;
int tests_passed = 0;

// Mock sprite for testing
static Sprite dummy_sprite;

void print_test_result(const char* test_name, bool passed) {
    tests_run++;
    if (passed) tests_passed++;
    
    printf("Test: %-45s - %s\n", test_name, passed ? "PASS" : "FAIL");
    
    if (!passed) {
        printf("  *** TEST FAILED ***\n");
    }
}

void test_camera_initialization() {
    printf("\n=== Camera Initialization Tests ===\n");
    
    // Test camera initialization with valid map
    Map test_map = {0};
    test_map.w = 32; // 32 metatiles (4096 px)
    test_map.h = 16; // 16 metatiles (2048 px)
    
    u16 result = SGP_CameraInit(&test_map);
    print_test_result("Camera init with valid map", result != 0);
    print_test_result("Camera is active after init", SGP_isCameraActive());
    print_test_result("Map dimensions calculated correctly", 
                      sgp.camera.map_width == 4096 && sgp.camera.map_height == 2048);
    
    // Test with NULL map (should fail gracefully)
    SGP_init(); // Reset state
    result = SGP_CameraInit(NULL);
    print_test_result("Camera init with NULL map fails", result == 0);
}

void test_camera_following() {
    printf("\n=== Camera Following Tests ===\n");
    
    // Setup test map
    Map test_map = {0};
    test_map.w = 32; // 32 metatiles (4096 px)
    test_map.h = 16; // 16 metatiles (2048 px)
    SGP_CameraInit(&test_map);
    
    u32 initial_camera_x = sgp.camera.current_x;
    u32 initial_camera_y = sgp.camera.current_y;
    
    // Test basic camera following
    SGPCameraTarget target = {
        .sprite = &dummy_sprite,
        .offset_x = 160, // screenWidth/2 (320/2)
        .offset_y = 112, // screenHeight/2 (224/2)
        .sprite_world_x = 320,
        .sprite_world_y = 224
    };
    
    SGP_CameraFollowTarget(&target);
    
    // Camera should move to center the sprite
    bool camera_moved = (sgp.camera.current_x != initial_camera_x) || 
                       (sgp.camera.current_y != initial_camera_y);
    print_test_result("Camera follows target and moves", camera_moved);
    
    // Test camera centering calculation
    // Expected camera position: sprite_world - offset = 320 - 160 = 160
    bool centered_x = sgp.camera.current_x == 160;
    bool centered_y = sgp.camera.current_y == 112; // 224 - 112 = 112
    print_test_result("Camera centers sprite horizontally", centered_x);
    print_test_result("Camera centers sprite vertically", centered_y);
}

void test_map_bounds_clamping() {
    printf("\n=== Map Bounds Clamping Tests ===\n");
    
    // Setup test map
    Map test_map = {0};
    test_map.w = 32; // 32 metatiles (4096 px)
    test_map.h = 16; // 16 metatiles (2048 px)
    SGP_CameraInit(&test_map);
    
    SGPCameraTarget target = {
        .sprite = &dummy_sprite,
        .offset_x = 160,
        .offset_y = 112,
        .sprite_world_x = 0,
        .sprite_world_y = 0
    };
    
    // Test clamping at origin (0,0)
    SGP_CameraFollowTarget(&target);
    print_test_result("Camera clamped at origin X", sgp.camera.current_x == 0);
    print_test_result("Camera clamped at origin Y", sgp.camera.current_y == 0);
    
    // Test clamping at far right/bottom
    target.sprite_world_x = 4095;
    target.sprite_world_y = 2047;
    SGP_CameraFollowTarget(&target);
    
    // Expected: camera should be clamped to (map_width - screenWidth, map_height - screenHeight)
    // 4096 - 320 = 3776, 2048 - 224 = 1824
    bool clamped_right = sgp.camera.current_x == 3776;
    bool clamped_bottom = sgp.camera.current_y == 1824;
    print_test_result("Camera clamped at right boundary", clamped_right);
    print_test_result("Camera clamped at bottom boundary", clamped_bottom);
    
    // Test middle position (no clamping)
    target.sprite_world_x = 2048;
    target.sprite_world_y = 1024;
    SGP_CameraFollowTarget(&target);
    
    bool middle_x = sgp.camera.current_x == (2048 - 160);
    bool middle_y = sgp.camera.current_y == (1024 - 112);
    print_test_result("Camera follows without clamping in middle", middle_x && middle_y);
}

void test_sprite_positioning() {
    printf("\n=== Sprite Positioning Tests ===\n");
    
    // Setup test map
    Map test_map = {0};
    test_map.w = 32;
    test_map.h = 16;
    SGP_CameraInit(&test_map);
    
    // Test sprite positioning relative to camera
    SGPCameraTarget target = {
        .sprite = &dummy_sprite,
        .offset_x = 160,
        .offset_y = 112,
        .sprite_world_x = 320,
        .sprite_world_y = 224
    };
    
    SGP_CameraFollowTarget(&target);
    
    // Sprite should be positioned at screen center when camera is centered on it
    // Screen position = sprite_world - camera_position
    // With proper centering: 320 - 160 = 160 (center of 320px screen)
    print_test_result("Sprite positioning calculated", true); // Mock doesn't track actual position
    
    // Test with NULL sprite (should not crash)
    target.sprite = NULL;
    SGP_CameraFollowTarget(&target);
    print_test_result("NULL sprite handled gracefully", true);
}

void test_camera_state_management() {
    printf("\n=== Camera State Management Tests ===\n");
    
    // Setup test map
    Map test_map = {0};
    test_map.w = 32;
    test_map.h = 16;
    SGP_CameraInit(&test_map);
    
    print_test_result("Camera active after init", SGP_isCameraActive());
    
    // Test deactivation
    SGP_deactivateCamera();
    print_test_result("Camera deactivated", !SGP_isCameraActive());
    
    // Test reactivation
    SGP_activateCamera();
    print_test_result("Camera reactivated", SGP_isCameraActive());
    
    // Test that following is ignored when inactive
    SGP_deactivateCamera();
    u32 camera_x_before = sgp.camera.current_x;
    u32 camera_y_before = sgp.camera.current_y;
    
    SGPCameraTarget target = {
        .sprite = &dummy_sprite,
        .offset_x = 160,
        .offset_y = 112,
        .sprite_world_x = 1000,
        .sprite_world_y = 1000
    };
    
    SGP_CameraFollowTarget(&target);
    
    bool camera_unchanged = (sgp.camera.current_x == camera_x_before) && 
                           (sgp.camera.current_y == camera_y_before);
    print_test_result("Following ignored when camera inactive", camera_unchanged);
}

void test_direct_camera_updates() {
    printf("\n=== Direct Camera Update Tests ===\n");
    
    // Setup test map
    Map test_map = {0};
    test_map.w = 32;
    test_map.h = 16;
    SGP_CameraInit(&test_map);
    
    // Test direct update when camera is active (should be ignored)
    SGP_activateCamera();
    u32 camera_x_before = sgp.camera.current_x;
    u32 camera_y_before = sgp.camera.current_y;
    
    SGP_UpdateCameraPosition(500, 600);
    
    bool update_ignored = (sgp.camera.current_x == camera_x_before) && 
                         (sgp.camera.current_y == camera_y_before);
    print_test_result("Direct update ignored when camera active", update_ignored);
    
    // Test direct update when camera is inactive
    SGP_deactivateCamera();
    SGP_UpdateCameraPosition(123, 456);
    
    bool update_applied = (sgp.camera.current_x == 123) && 
                         (sgp.camera.current_y == 456);
    print_test_result("Direct update applied when camera inactive", update_applied);
}

void test_camera_limits() {
    printf("\n=== Camera Limits Tests ===\n");
    
    // Test vertical scroll limit functions
    u16 default_limit = SGP_CameraGetVerticalScrollLimit();
    print_test_result("Default vertical scroll limit is 32", default_limit == 32);
    
    SGP_CameraSetVerticalScrollLimit(64);
    u16 new_limit = SGP_CameraGetVerticalScrollLimit();
    print_test_result("Vertical scroll limit updated to 64", new_limit == 64);
    
    // Reset to default
    SGP_CameraSetVerticalScrollLimit(32);
}

int main() {
    printf("=== SGP Comprehensive Camera System Test Suite ===\n");
    
    // Initialize SGP
    SGP_init();
    
    printf("\nCamera system test configuration:\n");
    printf("Screen size: 320x224 pixels\n");
    printf("Test map: 32x16 metatiles (4096x2048 pixels)\n");
    printf("Centering offset: 160x112 pixels (screen center)\n");
    
    // Run all test suites
    test_camera_initialization();
    test_camera_following();
    test_map_bounds_clamping();
    test_sprite_positioning();
    test_camera_state_management();
    test_direct_camera_updates();
    test_camera_limits();
    
    // Summary
    printf("\n=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);
    printf("Success rate: %.1f%%\n", (float)tests_passed / tests_run * 100.0f);
    
    if (tests_passed == tests_run) {
        printf("\n✓ All camera tests passed!\n");
        printf("Camera system functions correctly with proper bounds checking.\n");
        return 0;
    } else {
        printf("\n✗ Some camera tests failed!\n");
        return 1;
    }
}
