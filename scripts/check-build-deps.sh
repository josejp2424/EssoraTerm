#!/bin/sh
set -eu

# EssoraTerm build dependency check.
# This checks development headers and linker files, not only runtime libraries.

CC_BIN=${CC:-cc}
TMPBASE=${TMPDIR:-/var/tmp}
TMPDIR_ET=$(mktemp -d "$TMPBASE/essoraterm-deps.XXXXXX") || exit 1
trap 'rm -rf "$TMPDIR_ET"' EXIT HUP INT TERM

missing=""

add_missing() {
    case " $missing " in
        *" $1 "*) : ;;
        *) missing="$missing $1" ;;
    esac
}

check_header() {
    header=$1
    package=$2
    if [ ! -f "/usr/include/$header" ]; then
        printf '  MISSING header: /usr/include/%s  (%s)\n' "$header" "$package" >&2
        add_missing "$package"
    fi
}

compile_test() {
    name=$1
    packages=$2
    libs=$3
    source=$4
    src="$TMPDIR_ET/$name.c"
    bin="$TMPDIR_ET/$name"
    printf '%s\n' "$source" > "$src"
    if ! "$CC_BIN" "$src" -o "$bin" $libs >/dev/null 2>"$TMPDIR_ET/$name.log"; then
        printf '  MISSING linker support: %s\n' "$name" >&2
        sed -n '1,8p' "$TMPDIR_ET/$name.log" | sed 's/^/    /' >&2
        for package in $packages; do
            add_missing "$package"
        done
    fi
}

printf '%s\n' '==> Checking X11 development files for EssoraTerm'

command -v "$CC_BIN" >/dev/null 2>&1 || {
    printf '  MISSING compiler: %s\n' "$CC_BIN" >&2
    add_missing build-essential
}

check_header X11/Xlib.h libx11-dev
check_header X11/Intrinsic.h libxt-dev
check_header X11/Xaw/SimpleMenu.h libxaw7-dev
check_header X11/Xaw/Box.h libxaw7-dev
check_header X11/Xaw/SmeBSB.h libxaw7-dev
check_header X11/Xaw/SmeLine.h libxaw7-dev
check_header X11/Xmu/CharSet.h libxmu-dev
check_header X11/Xmu/Atoms.h libxmu-dev
check_header X11/xpm.h libxpm-dev
check_header X11/Xft/Xft.h libxft-dev
check_header fontconfig/fontconfig.h libfontconfig1-dev
check_header gtk-3.0/gtk/gtk.h libgtk-3-dev

if command -v "$CC_BIN" >/dev/null 2>&1; then
    compile_test xt 'libxt-dev libx11-dev libice-dev libsm-dev' '-lXt -lX11' \
        '#include <X11/Intrinsic.h>
int main(void) { XtToolkitInitialize(); return 0; }'

    compile_test xaw 'libxaw7-dev libxmu-dev libxt-dev libx11-dev' '-lXaw -lXmu -lXt -lX11' \
        '#include <X11/Intrinsic.h>
#include <X11/Xaw/SimpleMenu.h>
int main(void) { XawSimpleMenuAddGlobalActions((XtAppContext)0); return 0; }'

    compile_test xpm 'libxpm-dev libx11-dev' '-lXpm -lX11' \
        '#include <X11/xpm.h>
int main(void) { XpmAttributes a; a.valuemask = 0; XpmFreeAttributes(&a); return 0; }'

    if command -v pkg-config >/dev/null 2>&1 && pkg-config --exists gtk+-3.0 2>/dev/null; then
        gtk_cflags=$(pkg-config --cflags gtk+-3.0)
        gtk_libs=$(pkg-config --libs gtk+-3.0)
        compile_test gtk3 'libgtk-3-dev' "$gtk_cflags $gtk_libs" \
            '#include <gtk/gtk.h>
int main(int argc, char **argv) { gtk_init(&argc, &argv); return 0; }'
    fi
fi

if command -v pkg-config >/dev/null 2>&1; then
    if ! pkg-config --exists xft 2>/dev/null; then
        printf '%s\n' '  MISSING pkg-config module: xft (libxft-dev/libfontconfig1-dev)' >&2
        add_missing libxft-dev
        add_missing libfontconfig1-dev
    fi
else
    printf '%s\n' '  MISSING command: pkg-config' >&2
    add_missing pkg-config
fi

if [ -n "$missing" ]; then
    cat >&2 <<EOF_MSG

ERROR: EssoraTerm cannot be compiled because the X11 development files are incomplete.
The installed runtime libraries are not enough; X11 and GTK3 development files are required.

Missing/recommended packages:$missing

Puppy / EssoraPup:
  1. Load the DEVX SFS that belongs to this exact ISO/version.
  2. If DEVX is already loaded, install the missing -dev packages shown above.

Debian / Devuan / Essora (run as root, without sudo):
  ./scripts/install-build-deps.sh

After installing the dependencies, run:
  ./build-deb.sh
EOF_MSG
    exit 1
fi

printf '%s\n' 'X11 development files are ready.'
