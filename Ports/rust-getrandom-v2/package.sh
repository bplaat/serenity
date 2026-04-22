#!/usr/bin/env -S bash ../.port_include.sh
port='rust-getrandom-v2'
version='0.2.16'
files=()
workdir="getrandom-${version}"

fetch() {
    source "${PORT_META_DIR}/../.rust_crate_include.sh"
    fetch_rust_crate "getrandom" "${version}" "335ff9f135e4384c8150d6f27c6daed433577f86b4750418338c01a1a2528592"
}

configure() { :; }
build() { :; }

install() {
    source "${PORT_META_DIR}/../.rust_crate_include.sh"
    install_rust_crate "getrandom" "${version}"
}
