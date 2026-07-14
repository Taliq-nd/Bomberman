#!/usr/bin/env bash
# test.sh — Build and run ASCII Bomberman
# Usage: bash test.sh

set -e

bash "$(dirname "$0")/make.sh"

echo "Launching game..."
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OS" == "Windows_NT" ]]; then
    ./bomberman.exe
else
    ./bomberman
fi
