# !/bin/bash

# Create a script that finds every folder containing certain type of files passed as parameters
# and generates a shell script that adds include flags into a file_name passed as parameter.

# ex : ./include_flags.sh src "*.hpp" include_flags.mk INCLUDE_FLAGS

if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <folder> <pattern> <output.mk> <variable_name>"
    exit 1
fi

FOLDER="$1"
PATTERN="$2"
OUTPUT="$3"
VAR_NAME="$4"

FOLDER="${FOLDER%/}"

> "$OUTPUT"

find "$FOLDER" -type f -name "$PATTERN" | sort | while read -r file; do
    REL_PATH="${file#"$FOLDER"/}"
    echo "$VAR_NAME += -I $(dirname "$REL_PATH")" >> "$OUTPUT"
done