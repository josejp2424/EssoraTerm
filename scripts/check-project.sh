#!/bin/sh
set -eu
PROJECT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
TMPBASE=${TMPDIR:-/var/tmp}
TMPDIR_ET=$(mktemp -d "$TMPBASE/essoraterm-check.XXXXXX") || exit 1
trap 'rm -rf "$TMPDIR_ET"' EXIT HUP INT TERM

python3 - "$PROJECT_DIR/scripts/patch-source.py" <<'PY'
import pathlib
import sys
path = pathlib.Path(sys.argv[1])
compile(path.read_text(encoding="utf-8"), str(path), "exec")
PY

for file in \
    "$PROJECT_DIR/assets/mini.xterm_48x48.xpm" \
    "$PROJECT_DIR/assets/icons/hicolor/scalable/apps/essoraterm.svg" \
    "$PROJECT_DIR/assets/icons/hicolor/16x16/apps/essoraterm.png" \
    "$PROJECT_DIR/assets/icons/hicolor/24x24/apps/essoraterm.png" \
    "$PROJECT_DIR/assets/icons/hicolor/32x32/apps/essoraterm.png" \
    "$PROJECT_DIR/assets/icons/hicolor/48x48/apps/essoraterm.png" \
    "$PROJECT_DIR/assets/icons/hicolor/64x64/apps/essoraterm.png" \
    "$PROJECT_DIR/assets/icons/hicolor/128x128/apps/essoraterm.png" \
    "$PROJECT_DIR/assets/icons/hicolor/256x256/apps/essoraterm.png" \
    "$PROJECT_DIR/src/essoraterm-launcher.c" \
    "$PROJECT_DIR/src/essoraterm-settings.c" \
    "$PROJECT_DIR/packaging/essoraterm-settings.desktop"; do
    test -s "$file"
done

for file in \
    "$PROJECT_DIR/build-deb.sh" \
    "$PROJECT_DIR/scripts/fetch-source.sh" \
    "$PROJECT_DIR/scripts/check-build-deps.sh" \
    "$PROJECT_DIR/scripts/install-build-deps.sh"; do
    test -x "$file"
done

cc -std=c11 -O2 -Wall -Wextra -Wpedantic -Wformat=2 \
    -Wshadow -Wstrict-prototypes -Wmissing-prototypes \
    -o "$TMPDIR_ET/essoraterm-launcher" \
    "$PROJECT_DIR/src/essoraterm-launcher.c"

if pkg-config --exists gtk+-3.0 2>/dev/null; then
    cc -std=c11 -fsyntax-only -Wall -Wextra -Wpedantic -Wformat=2 \
        -Wshadow -Wstrict-prototypes -Wmissing-prototypes \
        $(pkg-config --cflags gtk+-3.0) \
        "$PROJECT_DIR/src/essoraterm-settings.c"
fi

grep -q 'essoraterm-popup-edit-menu' "$PROJECT_DIR/scripts/patch-source.py"
grep -q 'do_essora_preferences' "$PROJECT_DIR/scripts/patch-source.py"
grep -q 'essoraterm-settings' "$PROJECT_DIR/scripts/patch-source.py"
grep -q '<Btn3Up>: essoraterm-popup-edit-menu()' "$PROJECT_DIR/packaging/EssoraTerm"
grep -q '<Motion>: highlight()' "$PROJECT_DIR/packaging/EssoraTerm"
grep -q 'rightScrollBar: true' "$PROJECT_DIR/packaging/EssoraTerm"
grep -q 'StartScroll(Continuous)' "$PROJECT_DIR/packaging/EssoraTerm"
grep -q 'XGrabPointer' "$PROJECT_DIR/scripts/patch-source.py"
grep -q 'XtNpopdownCallback' "$PROJECT_DIR/scripts/patch-source.py"
grep -q '#1F7A52' "$PROJECT_DIR/packaging/EssoraTerm"
grep -q 'ESSORATERM_BINARY' "$PROJECT_DIR/src/essoraterm-launcher.c"
grep -q 'settings.conf' "$PROJECT_DIR/src/essoraterm-settings.c"
grep -q '1.1.0-1' "$PROJECT_DIR/build-deb.sh"

printf '%s\n' "EssoraTerm project checks passed."
