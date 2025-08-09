/*
 * input_test.c - Comprehensive test suite for SGP Input functions
 * 
 * This test validates all SGP input functions including edge detection,
 * button state management, multi-controller support, and edge cases.
 */

#include "sgp_test.h"

// Global SGP state for tests
SGP sgp;

// Test state for mocking joypad reads
static u16 mock_joy1_state = 0;
static u16 mock_joy2_state = 0;

// Mock SGDK function implementations
u16 JOY_readJoypad(u16 joy) { 
    return (joy == JOY_1) ? mock_joy1_state : mock_joy2_state;
}

void MAP_scrollTo(Map* map, u32 x, u32 y) { (void)map; (void)x; (void)y; }
void VDP_drawText(const char* str, u16 x, u16 y) { (void)str; (void)x; (void)y; }
void SYS_doVBlankProcess(void) {}
void VDP_setHorizontalScroll(u16 bg, s16 scroll) { (void)bg; (void)scroll; }
void VDP_setVerticalScroll(u16 bg, s16 scroll) { (void)bg; (void)scroll; }
void SPR_setPosition(Sprite* sprite, s16 x, s16 y) { (void)sprite; (void)x; (void)y; }
void VDP_setWindowVPos(bool enable, u16 pos) { (void)enable; (void)pos; }
void VDP_drawTextEx(u16 plane, const char* str, u16 attr, u16 x, u16 y, u16 method) { 
    (void)plane; (void)str; (void)attr; (void)x; (void)y; (void)method; 
}
u16 TILE_ATTR(u16 pal, bool priority, bool flipV, bool flipH) { 
    (void)pal; (void)priority; (void)flipV; (void)flipH; return 0; 
}

// Test utilities
static void set_mock_joypad_state(u16 joy1_state, u16 joy2_state) {
    mock_joy1_state = joy1_state;
    mock_joy2_state = joy2_state;
}

static void reset_sgp_input_state() {
    sgp.input.joy1_state = 0;
    sgp.input.joy2_state = 0;
    sgp.input.joy1_previous = 0;
    sgp.input.joy2_previous = 0;
}

static int test_count = 0;
static int test_passed = 0;

#define TEST(name, expr) do { \
    test_count++; \
    if (expr) { \
        test_passed++; \
        printf("✓ %s\n", name); \
    } else { \
        printf("✗ %s\n", name); \
    } \
} while(0)

//=============================================================================
// Test Suite 1: SGP_PollInput Function
//=============================================================================

void test_poll_input_basic() {
    printf("\n=== Test Suite 1: SGP_PollInput Basic Functionality ===\n");
    
    // Test 1: Initial state polling
    reset_sgp_input_state();
    set_mock_joypad_state(0, 0);
    SGP_PollInput();
    TEST("Poll input - initial state zero", 
         sgp.input.joy1_state == 0 && sgp.input.joy2_state == 0 &&
         sgp.input.joy1_previous == 0 && sgp.input.joy2_previous == 0);
    
    // Test 2: State change detection
    set_mock_joypad_state(BUTTON_A, BUTTON_B);
    SGP_PollInput();
    TEST("Poll input - state change detection", 
         sgp.input.joy1_state == BUTTON_A && sgp.input.joy2_state == BUTTON_B &&
         sgp.input.joy1_previous == 0 && sgp.input.joy2_previous == 0);
    
    // Test 3: Previous state tracking
    set_mock_joypad_state(BUTTON_A | BUTTON_UP, BUTTON_B | BUTTON_DOWN);
    SGP_PollInput();
    TEST("Poll input - previous state tracking", 
         sgp.input.joy1_state == (BUTTON_A | BUTTON_UP) && 
         sgp.input.joy2_state == (BUTTON_B | BUTTON_DOWN) &&
         sgp.input.joy1_previous == BUTTON_A && sgp.input.joy2_previous == BUTTON_B);
    
    // Test 4: Multiple button combinations
    set_mock_joypad_state(BUTTON_A | BUTTON_B | BUTTON_UP | BUTTON_LEFT, 
                         BUTTON_A | BUTTON_RIGHT | BUTTON_DOWN);
    SGP_PollInput();
    TEST("Poll input - multiple button combinations", 
         sgp.input.joy1_state == (BUTTON_A | BUTTON_B | BUTTON_UP | BUTTON_LEFT) &&
         sgp.input.joy2_state == (BUTTON_A | BUTTON_RIGHT | BUTTON_DOWN));
}

//=============================================================================
// Test Suite 2: SGP_ButtonPressed Function (Edge Detection)
//=============================================================================

void test_button_pressed() {
    printf("\n=== Test Suite 2: SGP_ButtonPressed Edge Detection ===\n");
    
    // Test 1: Fresh button press detection
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A, 0);
    SGP_PollInput();
    TEST("Button pressed - fresh press JOY_1", SGP_ButtonPressed(JOY_1, BUTTON_A));
    TEST("Button pressed - no press JOY_2", !SGP_ButtonPressed(JOY_2, BUTTON_A));
    
    // Test 2: Held button should not register as pressed
    SGP_PollInput();  // Same state, should be considered held
    TEST("Button pressed - held button not pressed", !SGP_ButtonPressed(JOY_1, BUTTON_A));
    
    // Test 3: Multiple button press detection
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A | BUTTON_B, BUTTON_UP | BUTTON_DOWN);
    SGP_PollInput();
    TEST("Button pressed - multiple buttons JOY_1", 
         SGP_ButtonPressed(JOY_1, BUTTON_A) && SGP_ButtonPressed(JOY_1, BUTTON_B));
    TEST("Button pressed - multiple buttons JOY_2", 
         SGP_ButtonPressed(JOY_2, BUTTON_UP) && SGP_ButtonPressed(JOY_2, BUTTON_DOWN));
    
    // Test 4: Combined button mask detection
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A | BUTTON_B, 0);
    SGP_PollInput();
    TEST("Button pressed - combined mask", SGP_ButtonPressed(JOY_1, BUTTON_A | BUTTON_B));
    
    // Test 5: Partial button mask (only one button of mask pressed)
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A, 0);  // Only A pressed, not B
    SGP_PollInput();
    TEST("Button pressed - partial mask detection", SGP_ButtonPressed(JOY_1, BUTTON_A | BUTTON_B));  // Should be true - ANY button
    
    // Test 6: Sequential button presses
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A, 0);
    SGP_PollInput();
    bool first_press = SGP_ButtonPressed(JOY_1, BUTTON_A);
    
    set_mock_joypad_state(BUTTON_A | BUTTON_B, 0);  // Add B while A held
    SGP_PollInput();
    bool second_press = SGP_ButtonPressed(JOY_1, BUTTON_B);
    TEST("Button pressed - sequential presses", first_press && second_press);
}

//=============================================================================
// Test Suite 3: SGP_ButtonReleased Function (Release Detection)
//=============================================================================

void test_button_released() {
    printf("\n=== Test Suite 3: SGP_ButtonReleased Release Detection ===\n");
    
    // Test 1: Button release detection
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A, BUTTON_B);
    SGP_PollInput();  // Press buttons
    
    set_mock_joypad_state(0, 0);  // Release buttons
    SGP_PollInput();
    TEST("Button released - basic release JOY_1", SGP_ButtonReleased(JOY_1, BUTTON_A));
    TEST("Button released - basic release JOY_2", SGP_ButtonReleased(JOY_2, BUTTON_B));
    
    // Test 2: No release when button not previously pressed
    reset_sgp_input_state();
    set_mock_joypad_state(0, 0);
    SGP_PollInput();
    TEST("Button released - no false release", !SGP_ButtonReleased(JOY_1, BUTTON_A));
    
    // Test 3: Partial release detection
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A | BUTTON_B, 0);
    SGP_PollInput();  // Press A and B
    
    set_mock_joypad_state(BUTTON_A, 0);  // Release only B
    SGP_PollInput();
    TEST("Button released - partial release", 
         SGP_ButtonReleased(JOY_1, BUTTON_B) && !SGP_ButtonReleased(JOY_1, BUTTON_A));
    
    // Test 4: Multiple button release
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A | BUTTON_B | BUTTON_UP, BUTTON_LEFT | BUTTON_RIGHT);
    SGP_PollInput();  // Press multiple
    
    set_mock_joypad_state(0, 0);  // Release all
    SGP_PollInput();
    TEST("Button released - multiple releases", 
         SGP_ButtonReleased(JOY_1, BUTTON_A | BUTTON_B | BUTTON_UP) &&
         SGP_ButtonReleased(JOY_2, BUTTON_LEFT | BUTTON_RIGHT));
    
    // Test 5: Release with simultaneous new press
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A, 0);
    SGP_PollInput();  // Press A
    
    set_mock_joypad_state(BUTTON_B, 0);  // Release A, press B
    SGP_PollInput();
    TEST("Button released - release with new press", 
         SGP_ButtonReleased(JOY_1, BUTTON_A) && SGP_ButtonPressed(JOY_1, BUTTON_B));
}

//=============================================================================
// Test Suite 4: SGP_ButtonDown Function (State Detection)
//=============================================================================

void test_button_down() {
    printf("\n=== Test Suite 4: SGP_ButtonDown State Detection ===\n");
    
    // Test 1: Basic button down detection
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A, BUTTON_B);
    SGP_PollInput();
    TEST("Button down - basic state JOY_1", SGP_ButtonDown(JOY_1, BUTTON_A));
    TEST("Button down - basic state JOY_2", SGP_ButtonDown(JOY_2, BUTTON_B));
    TEST("Button down - not pressed", !SGP_ButtonDown(JOY_1, BUTTON_B));
    
    // Test 2: Multiple buttons down
    set_mock_joypad_state(BUTTON_A | BUTTON_B | BUTTON_UP, BUTTON_LEFT | BUTTON_RIGHT | BUTTON_DOWN);
    SGP_PollInput();
    TEST("Button down - multiple buttons", 
         SGP_ButtonDown(JOY_1, BUTTON_A) && SGP_ButtonDown(JOY_1, BUTTON_B) && 
         SGP_ButtonDown(JOY_1, BUTTON_UP));
    TEST("Button down - multiple buttons JOY_2", 
         SGP_ButtonDown(JOY_2, BUTTON_LEFT) && SGP_ButtonDown(JOY_2, BUTTON_RIGHT) && 
         SGP_ButtonDown(JOY_2, BUTTON_DOWN));
    
    // Test 3: Combined button mask detection
    TEST("Button down - combined mask", SGP_ButtonDown(JOY_1, BUTTON_A | BUTTON_B));
    TEST("Button down - partial mask", SGP_ButtonDown(JOY_1, BUTTON_A | BUTTON_LEFT));  // A is pressed, so should be true
    
    // Test 4: State persistence across polls
    SGP_PollInput();  // Same state
    TEST("Button down - state persistence", SGP_ButtonDown(JOY_1, BUTTON_A));
    
    // Test 5: State changes
    set_mock_joypad_state(0, 0);
    SGP_PollInput();
    TEST("Button down - state change to not pressed", !SGP_ButtonDown(JOY_1, BUTTON_A));
}

//=============================================================================
// Test Suite 5: Edge Cases and Error Conditions
//=============================================================================

void test_edge_cases() {
    printf("\n=== Test Suite 5: Edge Cases and Error Conditions ===\n");
    
    // Test 1: Zero button mask
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A, 0);
    SGP_PollInput();
    TEST("Edge case - zero button mask pressed", !SGP_ButtonPressed(JOY_1, 0));
    TEST("Edge case - zero button mask released", !SGP_ButtonReleased(JOY_1, 0));
    TEST("Edge case - zero button mask down", !SGP_ButtonDown(JOY_1, 0));
    
    // Test 2: Invalid controller ID (behavior should be consistent)
    u16 invalid_joy = 5;  // Not JOY_1 or JOY_2
    bool invalid_pressed = SGP_ButtonPressed(invalid_joy, BUTTON_A);
    bool invalid_released = SGP_ButtonReleased(invalid_joy, BUTTON_A);
    bool invalid_down = SGP_ButtonDown(invalid_joy, BUTTON_A);
    TEST("Edge case - invalid controller consistent behavior", 
         !invalid_pressed && !invalid_released && !invalid_down);
    
    // Test 3: Maximum button combinations
    u16 all_buttons = BUTTON_A | BUTTON_B | BUTTON_UP | BUTTON_DOWN | BUTTON_LEFT | BUTTON_RIGHT;
    reset_sgp_input_state();
    set_mock_joypad_state(all_buttons, all_buttons);
    SGP_PollInput();
    TEST("Edge case - all buttons pressed", SGP_ButtonPressed(JOY_1, all_buttons));
    TEST("Edge case - all buttons down", SGP_ButtonDown(JOY_1, all_buttons));
    
    set_mock_joypad_state(0, 0);
    SGP_PollInput();
    TEST("Edge case - all buttons released", SGP_ButtonReleased(JOY_1, all_buttons));
    
    // Test 4: Rapid state changes
    reset_sgp_input_state();
    for (int i = 0; i < 5; i++) {
        set_mock_joypad_state((i % 2) ? BUTTON_A : 0, 0);
        SGP_PollInput();
    }
    // Should end with button not pressed (i=4, even, so 0)
    TEST("Edge case - rapid state changes", !SGP_ButtonDown(JOY_1, BUTTON_A));
    
    // Test 5: State consistency between controllers
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A, BUTTON_A);
    SGP_PollInput();
    bool joy1_state = SGP_ButtonDown(JOY_1, BUTTON_A);
    bool joy2_state = SGP_ButtonDown(JOY_2, BUTTON_A);
    TEST("Edge case - controller independence", joy1_state && joy2_state);
}

//=============================================================================
// Test Suite 6: Complex Input Scenarios
//=============================================================================

void test_complex_scenarios() {
    printf("\n=== Test Suite 6: Complex Input Scenarios ===\n");
    
    // Test 1: Fighting game combo simulation
    reset_sgp_input_state();
    
    // Frame 1: Down + A
    set_mock_joypad_state(BUTTON_DOWN | BUTTON_A, 0);
    SGP_PollInput();
    bool combo_start = SGP_ButtonPressed(JOY_1, BUTTON_DOWN) && SGP_ButtonPressed(JOY_1, BUTTON_A);
    
    // Frame 2: Hold Down, release A
    set_mock_joypad_state(BUTTON_DOWN, 0);
    SGP_PollInput();
    bool combo_continue = SGP_ButtonDown(JOY_1, BUTTON_DOWN) && SGP_ButtonReleased(JOY_1, BUTTON_A);
    
    // Frame 3: Down + Right + B
    set_mock_joypad_state(BUTTON_DOWN | BUTTON_RIGHT | BUTTON_B, 0);
    SGP_PollInput();
    bool combo_finish = SGP_ButtonDown(JOY_1, BUTTON_DOWN) && 
                       SGP_ButtonPressed(JOY_1, BUTTON_RIGHT) && 
                       SGP_ButtonPressed(JOY_1, BUTTON_B);
    
    TEST("Complex scenario - fighting game combo", combo_start && combo_continue && combo_finish);
    
    // Test 2: Simultaneous two-player input
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A | BUTTON_UP, BUTTON_B | BUTTON_DOWN);
    SGP_PollInput();
    
    bool p1_input = SGP_ButtonPressed(JOY_1, BUTTON_A) && SGP_ButtonPressed(JOY_1, BUTTON_UP);
    bool p2_input = SGP_ButtonPressed(JOY_2, BUTTON_B) && SGP_ButtonPressed(JOY_2, BUTTON_DOWN);
    TEST("Complex scenario - simultaneous two-player", p1_input && p2_input);
    
    // Test 3: Input buffering simulation (rapid press/release)
    reset_sgp_input_state();
    bool buffer_test = true;
    
    for (int frame = 0; frame < 10; frame++) {
        u16 state = (frame % 3 == 0) ? BUTTON_A : 0;  // Press every 3rd frame
        set_mock_joypad_state(state, 0);
        SGP_PollInput();
        
        if (frame % 3 == 0) {
            if (!SGP_ButtonPressed(JOY_1, BUTTON_A)) buffer_test = false;
        } else {
            if (SGP_ButtonPressed(JOY_1, BUTTON_A)) buffer_test = false;
        }
    }
    TEST("Complex scenario - input buffering simulation", buffer_test);
    
    // Test 4: Directional input combinations
    reset_sgp_input_state();
    
    // Test diagonal movement
    set_mock_joypad_state(BUTTON_UP | BUTTON_RIGHT, 0);
    SGP_PollInput();
    bool diagonal = SGP_ButtonDown(JOY_1, BUTTON_UP) && SGP_ButtonDown(JOY_1, BUTTON_RIGHT);
    
    // Test opposite directions (should be possible but unusual)
    set_mock_joypad_state(BUTTON_UP | BUTTON_DOWN, 0);
    SGP_PollInput();
    bool opposite = SGP_ButtonDown(JOY_1, BUTTON_UP) && SGP_ButtonDown(JOY_1, BUTTON_DOWN);
    
    TEST("Complex scenario - diagonal movement", diagonal);
    TEST("Complex scenario - opposite directions", opposite);
}

//=============================================================================
// Test Suite 7: Performance and State Management
//=============================================================================

void test_performance_and_state() {
    printf("\n=== Test Suite 7: Performance and State Management ===\n");
    
    // Test 1: State consistency after many polls
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A, BUTTON_B);
    
    for (int i = 0; i < 100; i++) {
        SGP_PollInput();
    }
    TEST("Performance - state consistency after many polls", 
         SGP_ButtonDown(JOY_1, BUTTON_A) && SGP_ButtonDown(JOY_2, BUTTON_B));
    
    // Test 2: Memory state isolation between controllers
    reset_sgp_input_state();
    set_mock_joypad_state(BUTTON_A, 0);
    SGP_PollInput();
    
    // Verify JOY_2 state doesn't affect JOY_1 detection
    bool joy1_correct = SGP_ButtonPressed(JOY_1, BUTTON_A) && SGP_ButtonDown(JOY_1, BUTTON_A);
    bool joy2_correct = !SGP_ButtonPressed(JOY_2, BUTTON_A) && !SGP_ButtonDown(JOY_2, BUTTON_A);
    TEST("Performance - controller state isolation", joy1_correct && joy2_correct);
    
    // Test 3: Previous state tracking accuracy
    reset_sgp_input_state();
    u16 test_states[] = {BUTTON_A, BUTTON_B, BUTTON_A | BUTTON_B, 0, BUTTON_UP};
    int num_states = sizeof(test_states) / sizeof(test_states[0]);
    
    bool tracking_correct = true;
    for (int i = 0; i < num_states; i++) {
        set_mock_joypad_state(test_states[i], 0);
        SGP_PollInput();
        
        // Check previous state matches what we set last time (except first iteration)
        if (i > 0) {
            u16 expected_prev = test_states[i-1];
            if (sgp.input.joy1_previous != expected_prev) {
                tracking_correct = false;
                break;
            }
        }
    }
    TEST("Performance - previous state tracking accuracy", tracking_correct);
}

//=============================================================================
// Main Test Runner
//=============================================================================

int main() {
    printf("SGP Input Function Test Suite\n");
    printf("=============================\n");
    
    // Initialize SGP
    SGP_init();
    
    // Run all test suites
    test_poll_input_basic();
    test_button_pressed();
    test_button_released();
    test_button_down();
    test_edge_cases();
    test_complex_scenarios();
    test_performance_and_state();
    
    // Print summary
    printf("\n=== Test Summary ===\n");
    printf("Tests run: %d\n", test_count);
    printf("Tests passed: %d\n", test_passed);
    printf("Tests failed: %d\n", test_count - test_passed);
    printf("Success rate: %.1f%%\n", (test_count > 0) ? (100.0 * test_passed / test_count) : 0.0);
    
    if (test_passed == test_count) {
        printf("✓ All input tests passed!\n");
        return 0;
    } else {
        printf("✗ Some input tests failed.\n");
        return 1;
    }
}
