#!/bin/bash

# Script to filter ReShade.log for XInput-related lines
# Usage: ./filter_xinput_logs.sh

# Source log file path
SOURCE_LOG="C:/Program Files (x86)/Steam/steamapps/common/Ghost of a Tale/ReShade.log"

# Output file for filtered results
OUTPUT_FILE="xinput_filtered.log"

# Check if source log exists
if [ ! -f "$SOURCE_LOG" ]; then
    echo "Error: Source log file not found: $SOURCE_LOG"
    echo "Please make sure the path is correct and the file exists."
    exit 1
fi

echo "Filtering ReShade.log for XInput-related lines..."
echo "Source: $SOURCE_LOG"
echo "Output: $OUTPUT_FILE"

# Filter the log file for lines containing "XInput" (case-insensitive)
# Using grep with -i for case-insensitive matching
grep -i "XInput" "$SOURCE_LOG" > "$OUTPUT_FILE"

# Count the filtered lines
LINE_COUNT=$(wc -l < "$OUTPUT_FILE")

echo "Filtering complete!"
echo "Found $LINE_COUNT lines containing 'XInput'"
echo "Results saved to: $OUTPUT_FILE"

# Show first few lines as preview
if [ $LINE_COUNT -gt 0 ]; then
    echo ""
    echo "Preview of filtered results:"
    echo "================================"
    head -10 "$OUTPUT_FILE"
    
    if [ $LINE_COUNT -gt 10 ]; then
        echo "... and $((LINE_COUNT - 10)) more lines"
    fi
else
    echo "No XInput-related lines found in the log."
fi
