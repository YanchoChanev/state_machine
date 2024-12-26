#!/bin/bash

# Set the working directory
BASE_DIR="$(pwd)"
TEST_DIR="slave/tests/test_slave_restart_threads"
BUILD_DIR="$BASE_DIR/$TEST_DIR/build"
LOG_FILE="$BUILD_DIR/Testing/Temporary/LastTest.log"

# Step 1: Ensure the test directory exists
if [ ! -d "$BASE_DIR/$TEST_DIR" ]; then
    echo "Error: Directory $BASE_DIR/$TEST_DIR does not exist."
    exit 1
fi

# Step 2: Remove the existing build directory if it exists
if [ -d "$BUILD_DIR" ]; then
    echo "Removing existing build directory..."
    rm -rf "$BUILD_DIR"
fi

# Step 3: Create a new build directory
echo "Creating new build directory..."
mkdir -p "$BUILD_DIR" || { echo "Error: Could not create build directory."; exit 1; }

# Step 4: Enter the build directory
cd "$BUILD_DIR" || { echo "Error: Could not enter build directory."; exit 1; }

# Step 5: Run CMake
echo "Running CMake..."
cmake .. || { echo "Error: CMake configuration failed."; exit 1; }

# Step 6: Build the project
echo "Building the project..."
make || { echo "Error: Build failed."; exit 1; }

# Step 7: Run tests
echo "Running tests..."
make test || { echo "Error: Tests failed."; exit 1; }

# Step 8: Display the test log
if [ -f "$LOG_FILE" ]; then
    echo "Displaying test log:"
    cat "$LOG_FILE"
else
    echo "Error: Log file not found at $LOG_FILE"
    exit 1
fi

echo "Build and test completed successfully."
