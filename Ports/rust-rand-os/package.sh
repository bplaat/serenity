#!/usr/bin/env -S bash ../.port_include.sh
port='rust-rand-os'
version='0.1.3'
files=()
workdir="rand_os-${version}"

fetch() {
    source "${PORT_META_DIR}/../.rust_crate_include.sh"
    fetch_rust_crate "rand_os" "${version}" "7b75f676a1e053fc562eafbb47838d67c84801e38fc1ba459e8f180deabd5071"
}

configure() { :; }
build() { :; }

install() {
    source "${PORT_META_DIR}/../.rust_crate_include.sh"
    install_rust_crate "rand_os" "${version}"
}
