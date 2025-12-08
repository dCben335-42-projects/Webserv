#!/bin/bash

if [ "$#" -ne 5 ]; then
    echo "Usage: $0 <folder> <pattern> <output.mk> <variable_name> <prefix_flag>"
    echo "  prefix_flag: 1 = prefix with folder, 0 = no prefix"
    exit 1
fi

FOLDER="$1"
PATTERN="$2"
OUTPUT="$3"
VAR_NAME="$4"
PREFIX_FLAG="$5"

# Remove trailing slash
FOLDER="${FOLDER%/}"

> "$OUTPUT"

find "$FOLDER" -type f -name "$PATTERN" | sort | while read -r file; do
    REL_PATH="${file#"$FOLDER"/}"

    if [ "$PREFIX_FLAG" -eq 1 ]; then
        FINAL_PATH="$FOLDER/$REL_PATH"
    else
        FINAL_PATH="$REL_PATH"
    fi

    echo "$VAR_NAME += $FINAL_PATH" >> "$OUTPUT"
done
