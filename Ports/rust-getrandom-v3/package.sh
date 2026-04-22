#!/usr/bin/env -S bash ../.port_include.sh
port='rust-getrandom-v3'
version='0.3.4'
files=()
workdir="getrandom-${version}"

fetch() {
    source "${PORT_META_DIR}/../.rust_crate_include.sh"
    fetch_rust_crate "getrandom" "${version}" "899def5c37c4fd7b2664648c28120ecec138e4d395b459e5ca34f9cce2dd77fd"
}

configure() { :; }
build() { :; }

install() {
    source "${PORT_META_DIR}/../.rust_crate_include.sh"
    install_rust_crate "getrandom" "${version}"
}
