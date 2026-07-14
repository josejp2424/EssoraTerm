#!/bin/sh
set -eu

PROJECT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
VERSION=${XTERM_VERSION:-410}
DEST="$PROJECT_DIR/source/xterm-${VERSION}.tgz"
URL="https://invisible-mirror.net/archives/xterm/xterm-${VERSION}.tgz"

if [ -s "$DEST" ]; then
    printf '%s\n' "Using existing $DEST"
    exit 0
fi

mkdir -p "$PROJECT_DIR/source"
printf '%s\n' "Downloading xterm patch #${VERSION}..."

if command -v curl >/dev/null 2>&1; then
    curl --fail --location --retry 3 --output "$DEST.part" "$URL"
elif command -v wget >/dev/null 2>&1; then
    wget -O "$DEST.part" "$URL"
else
    printf '%s\n' "ERROR: install curl/wget or copy xterm-${VERSION}.tgz into source/" >&2
    exit 1
fi

mv -f "$DEST.part" "$DEST"
printf '%s\n' "Saved: $DEST"
