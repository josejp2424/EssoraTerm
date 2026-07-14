#!/bin/sh
set -eu
PROJECT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
rm -rf "$PROJECT_DIR/build" "$PROJECT_DIR/dist"
printf '%s\n' "Build files removed."
