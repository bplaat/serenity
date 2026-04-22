#!/usr/bin/env -S bash ../.port_include.sh
port='rust-errno'
version='0.3.14'
files=()
workdir="errno-${version}"

fetch() {
    source "${PORT_META_DIR}/../.rust_crate_include.sh"
    fetch_rust_crate "errno" "${version}" "39cab71617ae0d63f51a36d69f866391735b51691dbda63cf6f96d042b63efeb"
}

configure() { :; }
build() { :; }

install() {
    source "${PORT_META_DIR}/../.rust_crate_include.sh"
    install_rust_crate "errno" "${version}"
}
