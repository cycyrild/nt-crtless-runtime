#!/usr/bin/env bash

# Wrap assembly source in a C++ translation unit to make it compatible with the Clang LTO pipeline

set -euo pipefail

CLANG="${1:?C++ compiler must be supplied}"
shift

INPUT="${@: -1}"
OUTPUT=""

for ((i = 1; i <= $#; i++)); do
    if [[ "${!i}" == "-o" ]]; then
        j=$((i + 1))
        OUTPUT="${!j}"
    fi
done

[[ -z "$OUTPUT" ]] && {
    echo "error: missing -o" >&2
    exit 1
}

TMP_CC="${OUTPUT}.module_asm.cc"

cat > "$TMP_CC" <<EOF
__asm__(
R"ASM(
$(cat "$INPUT")
)ASM"
);
EOF

ARGS=("${@:1:$#-1}" "$TMP_CC")

exec "$CLANG" -flto=full "${ARGS[@]}"
