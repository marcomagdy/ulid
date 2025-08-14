#!/bin/bash

# This script is used as a Claude Code hook to build a CMake project.
# If the build fails, the tool should exit with code 2 and the failure details should be printed on stderr.
# Exiting with code 0 will provide no feedback to Claude (which is fine if the build succeeds).
# Exiting with code 1 is reserved for errors in the hook script itself.

# Discard the JSON input
cat > /dev/null

# Change to project root
cd $CLAUDE_PROJECT_DIR

# Build the project
echo "Building CMake project..."
build_output=$( { cmake --build build; } 2>&1 )
build_status=$?
if [ "$build_status" -ne 0 ]; then
    echo "Build failed:" >&2
    echo "$build_output" >&2
    exit 2
fi

echo "✅ Build successful"
exit 0

