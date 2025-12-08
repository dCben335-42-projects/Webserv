#!/bin/bash

# Create a script that finds every folder containing certain file types
# and generates a shell script that adds include flags into the output file.

# ex : ./include_flags.sh src "*.hpp" include_flags.mk INCLUDE_FLAGS 1

if [ "$#" -ne 5 ]; then
    echo "Usage: $0 <folder> <pattern> <output.mk> <variable_name> <prefix_flag>"
    echo "  prefix_flag: 1 = prefix folder, 0 = relative only"
    exit 1
fi

FOLDER="$1"
PATTERN="$2"
OUTPUT="$3"
VAR_NAME="$4"
PREFIX_FLAG="$5"

FOLDER="${FOLDER%/}"

> "$OUTPUT"

find "$FOLDER" -type f -name "$PATTERN" | sort | while read -r file; do
    REL_PATH="${file#"$FOLDER"/}"
    DIR_PATH="$(dirname "$REL_PATH")"

    if [ "$PREFIX_FLAG" -eq 1 ]; then
        FINAL_DIR="$FOLDER/$DIR_PATH"
    else
        FINAL_DIR="$DIR_PATH"
    fi

    echo "$VAR_NAME += -I $FINAL_DIR" >> "$OUTPUT"
done
