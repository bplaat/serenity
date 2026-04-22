#!/usr/bin/env -S bash ../.port_include.sh
port='rust-libc'
version='0.2.185'
files=()
workdir="libc-${version}"

fetch() {
    source "${PORT_META_DIR}/../.rust_crate_include.sh"
    fetch_rust_crate "libc" "${version}" "52ff2c0fe9bc6cb6b14a0592c2ff4fa9ceb83eea9db979b0487cd054946a2b8f"
}

configure() { :; }
build() { :; }

install() {
    source "${PORT_META_DIR}/../.rust_crate_include.sh"
    install_rust_crate "libc" "${version}"
}
