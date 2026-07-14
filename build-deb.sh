#!/bin/bash
# autor josejp2424
set -euo pipefail

PROJECT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
XTERM_VERSION="${XTERM_VERSION:-410}"
ESSORA_VERSION="${ESSORA_VERSION:-1.1.0-1}"
BUILD_DIR="$PROJECT_DIR/build"
SRC_DIR="$BUILD_DIR/xterm-${XTERM_VERSION}"
PKGROOT="$BUILD_DIR/package-root"
DIST_DIR="$PROJECT_DIR/dist"
SOURCE_TARBALL="$PROJECT_DIR/source/xterm-${XTERM_VERSION}.tgz"

die() {
    printf 'ERROR: %s\n' "$*" >&2
    exit 1
}

need() {
    command -v "$1" >/dev/null 2>&1 || die "missing command: $1"
}

for tool in python3 tar make cc sed awk find install ldd dpkg-deb pkg-config; do
    need "$tool"
done


"$PROJECT_DIR/scripts/check-build-deps.sh"

if [[ ! -s "$SOURCE_TARBALL" ]]; then
    "$PROJECT_DIR/scripts/fetch-source.sh"
fi

rm -rf "$BUILD_DIR"
mkdir -p "$SRC_DIR" "$PKGROOT" "$DIST_DIR"

printf '\n==> Extracting xterm %s\n' "$XTERM_VERSION"
tar -xzf "$SOURCE_TARBALL" --strip-components=1 -C "$SRC_DIR"

printf '\n==> Applying EssoraTerm source modifications\n'
python3 "$PROJECT_DIR/scripts/patch-source.py" \
    "$SRC_DIR" --project-root "$PROJECT_DIR"

for icon in \
    "$SRC_DIR/icons/essoraterm.svg" \
    "$SRC_DIR/icons/essoraterm_48x48.png" \
    "$SRC_DIR/icons/essoraterm_48x48.xpm"; do
    [[ -s "$icon" ]] || die "EssoraTerm configure icon is missing: $icon"
done

cd "$SRC_DIR"
CONFIG_HELP="$(./configure --help 2>/dev/null || true)"
CONFIG_ARGS=(
    "--prefix=/usr"
    "--bindir=/usr/bin"
    "--datadir=/usr/share"
    "--with-app-class=EssoraTerm"
    "--with-terminal-type=xterm-256color"
    "--with-icon-name=essoraterm"
    "--with-icon-symlink=NONE"
    "--program-transform-name=s,^xterm$,essoraterm,"
    "--with-xterm-symlink=xterm"
    "--disable-setuid"
    "--disable-setgid"
)

add_config_option() {
    local option="$1"
    if grep -Fq -- "${option%%=*}" <<<"$CONFIG_HELP"; then
        CONFIG_ARGS+=("$option")
    fi
}

add_config_option "--disable-desktop"
add_config_option "--disable-toolbar"
add_config_option "--enable-wide-chars"
add_config_option "--enable-256-color"
add_config_option "--enable-load-vt-fonts"
add_config_option "--with-xpm"
add_config_option "--with-xft"

printf '\n==> Configuring\n'
./configure "${CONFIG_ARGS[@]}"

printf '\n==> Compiling\n'
JOBS="${JOBS:-}"
if [[ -z "$JOBS" ]]; then
    if command -v nproc >/dev/null 2>&1; then
        JOBS="$(nproc)"
    else
        JOBS=2
    fi
fi
make -j"$JOBS"

printf '\n==> Installing into package-root\n'
make DESTDIR="$PKGROOT" install

[[ -x "$PKGROOT/usr/bin/essoraterm" ]] ||
    die "the build did not create /usr/bin/essoraterm"

printf '\n==> Building EssoraTerm launcher and preferences\n'
install -Dm755 "$PKGROOT/usr/bin/essoraterm" \
    "$PKGROOT/usr/lib/essoraterm/essoraterm-bin"
rm -f "$PKGROOT/usr/bin/essoraterm"

COMMON_WARNINGS=(
    -std=c11 -O2 -pipe -Wall -Wextra -Wpedantic -Wformat=2
    -Wshadow -Wstrict-prototypes -Wmissing-prototypes
)
cc "${COMMON_WARNINGS[@]}" \
    -o "$PKGROOT/usr/bin/essoraterm" \
    "$PROJECT_DIR/src/essoraterm-launcher.c"
cc "${COMMON_WARNINGS[@]}" \
    $(pkg-config --cflags gtk+-3.0) \
    -o "$PKGROOT/usr/bin/essoraterm-settings" \
    "$PROJECT_DIR/src/essoraterm-settings.c" \
    $(pkg-config --libs gtk+-3.0)

rm -f "$PKGROOT/usr/bin/xterm" "$PKGROOT/usr/bin/uxterm"
ln -s essoraterm "$PKGROOT/usr/bin/xterm"
ln -s essoraterm "$PKGROOT/usr/bin/uxterm"

if [[ -d "$PKGROOT/usr/share/applications" ]]; then
    find "$PKGROOT/usr/share/applications" -maxdepth 1 -type f \
        \( -iname '*xterm*' -o -iname '*uxterm*' -o -iname '*koi8*' \
           -o -iname '*essoraterm*' \) -delete
fi
if [[ -d "$PKGROOT/usr/share/icons" ]]; then
    find "$PKGROOT/usr/share/icons" -type f \
        \( -iname '*xterm*' -o -iname '*uxterm*' -o -iname '*koi8*' \
           -o -iname '*essoraterm*' \) -delete
fi
if [[ -d "$PKGROOT/usr/share/pixmaps" ]]; then
    find "$PKGROOT/usr/share/pixmaps" -type f \
        \( -iname '*xterm*' -o -iname '*uxterm*' -o -iname '*koi8*' \
           -o -iname '*essoraterm*' \) -delete
fi

find "$PKGROOT" -type f -path '*/app-defaults/*' \
    \( -name 'XTerm*' -o -name 'UXTerm*' -o -name 'KOI8RXTerm*' \
       -o -name 'EssoraTerm*' -o -name 'UEssoraTerm*' \
       -o -name 'KOI8REssoraTerm*' \) -delete 2>/dev/null || true

install -Dm644 "$PROJECT_DIR/packaging/EssoraTerm" \
    "$PKGROOT/etc/X11/app-defaults/EssoraTerm"
install -Dm644 "$PROJECT_DIR/packaging/essoraterm.desktop" \
    "$PKGROOT/usr/share/applications/essoraterm.desktop"
install -Dm644 "$PROJECT_DIR/packaging/essoraterm-settings.desktop" \
    "$PKGROOT/usr/share/applications/essoraterm-settings.desktop"

while IFS= read -r -d '' icon; do
    rel="${icon#"$PROJECT_DIR/assets/icons/"}"
    install -Dm644 "$icon" "$PKGROOT/usr/share/icons/$rel"
done < <(find "$PROJECT_DIR/assets/icons" -type f -print0)

install -Dm644 "$PROJECT_DIR/assets/essoraterm.png" \
    "$PKGROOT/usr/share/pixmaps/essoraterm.png"
install -Dm644 "$PROJECT_DIR/LICENSES/COPYING.xterm" \
    "$PKGROOT/usr/share/doc/essoraterm/COPYING.xterm"
install -Dm644 "$PROJECT_DIR/LICENSES/COPYING.essoraterm" \
    "$PKGROOT/usr/share/doc/essoraterm/COPYING.essoraterm"
install -Dm644 "$PROJECT_DIR/README.md" \
    "$PKGROOT/usr/share/doc/essoraterm/README.md"

ARCH="$(dpkg --print-architecture 2>/dev/null || true)"
if [[ -z "$ARCH" ]]; then
    case "$(uname -m)" in
        x86_64) ARCH=amd64 ;;
        i?86) ARCH=i386 ;;
        aarch64) ARCH=arm64 ;;
        armv7*) ARCH=armhf ;;
        *) ARCH="$(uname -m)" ;;
    esac
fi

detect_dependencies() {
    local fallback="libc6, libx11-6, libxaw7, libxft2, libxmu6, libxt6, libgtk-3-0"
    local packages=()
    local binary path pkg

    if ! command -v dpkg-query >/dev/null 2>&1; then
        printf '%s' "$fallback"
        return
    fi

    for binary in "$@"; do
        while IFS= read -r path; do
            [[ -n "$path" && -e "$path" ]] || continue
            pkg="$(dpkg-query -S "$path" 2>/dev/null | head -n1 | cut -d: -f1 || true)"
            [[ -n "$pkg" ]] || continue
            pkg="${pkg%%:*}"
            packages+=("$pkg")
        done < <(ldd "$binary" 2>/dev/null |
                 awk '/=> \// {print $3} /^\// {print $1}')
    done

    if ((${#packages[@]} == 0)); then
        printf '%s' "$fallback"
    else
        printf '%s\n' "${packages[@]}" | sort -u | paste -sd, - | sed 's/,/, /g'
    fi
}

DEPENDS="$(detect_dependencies \
    "$PKGROOT/usr/lib/essoraterm/essoraterm-bin" \
    "$PKGROOT/usr/bin/essoraterm" \
    "$PKGROOT/usr/bin/essoraterm-settings")"
INSTALLED_SIZE="$(du -sk "$PKGROOT" | awk '{print $1}')"

mkdir -p "$PKGROOT/DEBIAN"
cat > "$PKGROOT/DEBIAN/control" <<EOF
Package: essoraterm
Version: $ESSORA_VERSION
Architecture: $ARCH
Maintainer: josejp2424 <puppylinuxjosejp2424@gmail.com>
Installed-Size: $INSTALLED_SIZE
Depends: $DEPENDS
Provides: xterm, x-terminal-emulator
Conflicts: xterm
Replaces: xterm
Section: x11
Priority: optional
Homepage: https://www.essora.org/
Description: Lightweight terminal emulator for Essora and Puppy Linux
 EssoraTerm is based on xterm and adds a native right-click menu for
 Copy, Paste, Select All, Preferences and Clear Terminal. It includes a
 GTK3 preferences application written in C for colors, fonts, geometry,
 cursor and scrolling, while keeping /usr/bin/xterm compatibility.
EOF

install -m755 "$PROJECT_DIR/packaging/postinst" "$PKGROOT/DEBIAN/postinst"
install -m755 "$PROJECT_DIR/packaging/postrm" "$PKGROOT/DEBIAN/postrm"

[[ "$(readlink "$PKGROOT/usr/bin/xterm")" == "essoraterm" ]] ||
    die "xterm compatibility link is incorrect"
[[ -x "$PKGROOT/usr/lib/essoraterm/essoraterm-bin" ]] ||
    die "the real EssoraTerm binary was not staged"
[[ -x "$PKGROOT/usr/bin/essoraterm-settings" ]] ||
    die "the EssoraTerm preferences binary was not staged"

if find "$PKGROOT/usr/share/icons" "$PKGROOT/usr/share/pixmaps" \
        -type f \( -iname '*xterm*' -o -iname '*uxterm*' \) | grep -q .; then
    die "an upstream xterm icon remains in package-root"
fi

DEB="$DIST_DIR/essoraterm_${ESSORA_VERSION}_${ARCH}.deb"
rm -f "$DEB"
printf '\n==> Building %s\n' "$DEB"
if dpkg-deb --help 2>&1 | grep -q -- '--root-owner-group'; then
    dpkg-deb --root-owner-group --build "$PKGROOT" "$DEB"
else
    dpkg-deb --build "$PKGROOT" "$DEB"
fi

printf '\nBuild completed successfully.\n'
printf 'Package: %s\n' "$DEB"
printf 'Staged tree: %s\n' "$PKGROOT"
