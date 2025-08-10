# Makefile for SGP tests
# 
# This Makefile builds and runs tests for the SGP header-only library.
# Tests are built with mock SGDK dependencies to allow compilation 
# outside of the full SGDK environment.

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -I../
LDFLAGS = 

# Test targets
SMOKE_TEST = smoke_test.test
COLLISION_TEST = collision_test.test
INPUT_TEST = input_test.test
CAMERA_TEST = camera_test.test

# Source files
SMOKE_TEST_SRC = smoke_test.c
COLLISION_TEST_SRC = collision_test.c
INPUT_TEST_SRC = input_test.c
CAMERA_TEST_SRC = camera_test.c

# Default target
all: $(SMOKE_TEST) $(COLLISION_TEST) $(INPUT_TEST) $(CAMERA_TEST)

# Build smoke test
$(SMOKE_TEST): $(SMOKE_TEST_SRC)
	@echo "Building smoke test..."
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Build smoke test with DEBUG mode
$(SMOKE_TEST)_debug: $(SMOKE_TEST_SRC)
	@echo "Building smoke test (DEBUG mode)..."
	$(CC) $(CFLAGS) -DDEBUG -o $@ $< $(LDFLAGS)

# Build collision test
$(COLLISION_TEST): $(COLLISION_TEST_SRC)
	@echo "Building collision test..."
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Build collision test with DEBUG mode
$(COLLISION_TEST)_debug: $(COLLISION_TEST_SRC)
	@echo "Building collision test (DEBUG mode)..."
	$(CC) $(CFLAGS) -DDEBUG -o $@ $< $(LDFLAGS)

# Build input test
$(INPUT_TEST): $(INPUT_TEST_SRC)
	@echo "Building input test..."
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Build input test with DEBUG mode
$(INPUT_TEST)_debug: $(INPUT_TEST_SRC)
	@echo "Building input test (DEBUG mode)..."
	$(CC) $(CFLAGS) -DDEBUG -o $@ $< $(LDFLAGS)

# Build camera test
$(CAMERA_TEST): $(CAMERA_TEST_SRC)
	@echo "Building camera test..."
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Build camera test with DEBUG mode
$(CAMERA_TEST)_debug: $(CAMERA_TEST_SRC)
	@echo "Building camera test (DEBUG mode)..."
	$(CC) $(CFLAGS) -DDEBUG -o $@ $< $(LDFLAGS)

# Run smoke test
test: $(SMOKE_TEST)
	@echo "Running smoke test..."
	@./$(SMOKE_TEST)
	@make clean

# Run smoke test with DEBUG mode
test_debug: $(SMOKE_TEST)_debug
	@echo "Running smoke test (DEBUG mode)..."
	@./$(SMOKE_TEST)_debug
	@make clean

# Run collision test
collision: $(COLLISION_TEST)
	@echo "Running collision test..."
	@./$(COLLISION_TEST)
	@make clean

# Run collision test with DEBUG mode
collision_debug: $(COLLISION_TEST)_debug
	@echo "Running collision test (DEBUG mode)..."
	@./$(COLLISION_TEST)_debug
	@make clean

# Run input test
input: $(INPUT_TEST)
	@echo "Running input test..."
	@./$(INPUT_TEST)
	@make clean

# Run input test with DEBUG mode
input_debug: $(INPUT_TEST)_debug
	@echo "Running input test (DEBUG mode)..."
	@./$(INPUT_TEST)_debug
	@make clean

# Run camera test
camera: $(CAMERA_TEST)
	@echo "Running camera test..."
	@./$(CAMERA_TEST)
	@make clean

# Run camera test with DEBUG mode
camera_debug: $(CAMERA_TEST)_debug
	@echo "Running camera test (DEBUG mode)..."
	@./$(CAMERA_TEST)_debug
	@make clean

# Clean build artifacts
clean:
	@echo "Cleaning test artifacts..."
	@rm -f *.test *.o *.test_debug

# Check syntax only (no linking)
syntax_check: $(SMOKE_TEST_SRC)
	@echo "Checking syntax..."
	$(CC) $(CFLAGS) -fsyntax-only $<

# Check syntax with DEBUG mode
syntax_check_debug: $(SMOKE_TEST_SRC)
	@echo "Checking syntax (DEBUG mode)..."
	$(CC) $(CFLAGS) -DDEBUG -fsyntax-only $<

# Check collision test syntax
collision_syntax_check: $(COLLISION_TEST_SRC)
	@echo "Checking collision test syntax..."
	$(CC) $(CFLAGS) -fsyntax-only $<

# Check collision test syntax with DEBUG mode
collision_syntax_check_debug: $(COLLISION_TEST_SRC)
	@echo "Checking collision test syntax (DEBUG mode)..."
	$(CC) $(CFLAGS) -DDEBUG -fsyntax-only $<

# Check input test syntax
input_syntax_check: $(INPUT_TEST_SRC)
	@echo "Checking input test syntax..."
	$(CC) $(CFLAGS) -fsyntax-only $<

# Check input test syntax with DEBUG mode
input_syntax_check_debug: $(INPUT_TEST_SRC)
	@echo "Checking input test syntax (DEBUG mode)..."
	$(CC) $(CFLAGS) -DDEBUG -fsyntax-only $<

# Check camera test syntax
camera_syntax_check: $(CAMERA_TEST_SRC)
	@echo "Checking camera test syntax..."
	$(CC) $(CFLAGS) -fsyntax-only $<

# Check camera test syntax with DEBUG mode
camera_syntax_check_debug: $(CAMERA_TEST_SRC)
	@echo "Checking camera test syntax (DEBUG mode)..."
	$(CC) $(CFLAGS) -DDEBUG -fsyntax-only $<

# Run all syntax checks
syntax: syntax_check syntax_check_debug collision_syntax_check collision_syntax_check_debug input_syntax_check input_syntax_check_debug camera_syntax_check camera_syntax_check_debug
	@echo "✓ All syntax checks passed"

# Run all tests
all_tests: test collision input camera
	@echo "✓ All tests completed"
	@make clean

# Help target
help:
	@echo "SGP Test Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all           - Build all tests"
	@echo "  test          - Build and run smoke test"
	@echo "  test_debug    - Build and run smoke test with DEBUG mode"
	@echo "  collision     - Build and run collision test"
	@echo "  collision_debug - Build and run collision test with DEBUG mode"
	@echo "  input         - Build and run input test"
	@echo "  input_debug   - Build and run input test with DEBUG mode"
	@echo "  camera        - Build and run camera test"
	@echo "  camera_debug  - Build and run camera test with DEBUG mode"
	@echo "  all_tests     - Run smoke, collision, input, and camera tests"
	@echo "  syntax        - Check syntax for all tests (both normal and DEBUG)"
	@echo "  clean         - Remove build artifacts"
	@echo "  help          - Show this help"

.PHONY: all test test_debug collision collision_debug input input_debug camera camera_debug all_tests clean syntax_check syntax_check_debug collision_syntax_check collision_syntax_check_debug input_syntax_check input_syntax_check_debug camera_syntax_check camera_syntax_check_debug syntax help
