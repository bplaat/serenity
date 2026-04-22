#!/usr/bin/env -S bash ../.port_include.sh
port='rust-rustix'
version='1.1.2'
files=()
workdir="rustix-${version}"

fetch() {
    source "${PORT_META_DIR}/../.rust_crate_include.sh"
    fetch_rust_crate "rustix" "${version}" "cd15f8a2c5551a84d56efdc1cd049089e409ac19a3072d5037a17fd70719ff3e"
}

configure() { :; }
build() { :; }

install() {
    source "${PORT_META_DIR}/../.rust_crate_include.sh"
    install_rust_crate "rustix" "${version}"
}
