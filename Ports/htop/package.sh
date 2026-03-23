#!/usr/bin/env -S bash ../.port_include.sh
port='htop'
version='3.4.1'
files=(
    "https://github.com/htop-dev/htop/releases/download/${version}/htop-${version}.tar.xz#904f7d4580fc11cffc7e0f06895a4789e0c1c054435752c151e812fead9f6220"
)
depends=("ncurses")
useconfigure='true'
configopts=(
    "--disable-affinity"
    "--disable-delayacct"
    "--with-ncurses"
)

post_fetch() {
    cp -r "${PORT_META_DIR}/patches/serenity" "${workdir}/"
}

pre_configure() {
    run autoreconf -fi
}
