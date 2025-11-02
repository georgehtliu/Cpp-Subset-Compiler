#!/bin/bash
# Build and run the WLP4 compiler pipeline
# Usage: ./run.sh [input.wlp4]

INPUT="${1:-input.wlp4}"
OUTPUT_ASM="output.asm"
OUTPUT_MERL="output.merl"
LINKED_MERL="linked.merl"
FINAL_MIPS="final.mips"
BINARY="wlp4gen"

# Source environment setup if available (for CS241 course tools)
if [ -f /u/cs241/setup ]; then
    source /u/cs241/setup
fi

# Compile code generator
echo "Compiling code generator..."
g++ -std=c++14 -g -o "$BINARY" wlp4gen.cc

# Run compilation pipeline
echo "Running compilation pipeline..."
wlp4scan < "$INPUT" | wlp4parse | wlp4type > "${INPUT}ti"
"$BINARY" < "${INPUT}ti" > "$OUTPUT_ASM"

# Link and assemble
if command -v cs241.linkasm &> /dev/null; then
    cs241.linkasm < "$OUTPUT_ASM" > "$OUTPUT_MERL"
    cs241.linker "$OUTPUT_MERL" print.merl alloc.merl > "$LINKED_MERL"
    cs241.merl 0 < "$LINKED_MERL" > "$FINAL_MIPS" 2> /dev/null
    
    if command -v mips.twoints &> /dev/null; then
        echo "Running MIPS simulator..."
        mips.twoints "$FINAL_MIPS"
    fi
else
    echo "Note: CS241 tools not available. Generated assembly: $OUTPUT_ASM"
fi