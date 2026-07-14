#!/bin/sh
set -eu

if [ "$(id -u)" -ne 0 ]; then
    printf '%s\n' 'ERROR: run this helper as root. It does not use sudo.' >&2
    exit 1
fi

if ! command -v apt-get >/dev/null 2>&1; then
    cat >&2 <<'EOF_MSG'
ERROR: apt-get was not found.
On Puppy/EssoraPup, load the DEVX SFS matching the running ISO and install
these development packages with the available package manager:
  libx11-dev libxt-dev libxaw7-dev libxmu-dev libxpm-dev libxft-dev
  libfontconfig1-dev libxinerama-dev libice-dev libsm-dev libgtk-3-dev
EOF_MSG
    exit 1
fi

export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get install -y \
    build-essential dpkg-dev curl pkg-config \
    libx11-dev libxt-dev libxaw7-dev libxmu-dev libxpm-dev \
    libxft-dev libfontconfig1-dev libxinerama-dev libice-dev libsm-dev \
    libgtk-3-dev

printf '%s\n' 'EssoraTerm build dependencies installed.'
