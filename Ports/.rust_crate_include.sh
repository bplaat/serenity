#!/usr/bin/env bash
# Shared helper for Rust crate ports targeting SerenityOS.
# Sourced from port package.sh files that use #!/usr/bin/env -S bash ../.port_include.sh
#
# Provides two functions called from the standard port fetch() and install() hooks:
#   fetch_rust_crate NAME VERSION SHA256
#   install_rust_crate NAME VERSION
#
# The port directory may optionally contain:
#   patches/  - patch files applied by the standard port patch step
#   src/      - files overlaid on the crate root during fetch (before patching).
#               The directory structure must mirror the crate root, e.g. a file
#               at src/src/unix/serenity.rs maps to <crate>/src/unix/serenity.rs.

set -euo pipefail

# fetch_rust_crate NAME VERSION SHA256
# Downloads the crate tarball to Toolchain/Tarballs/, extracts it into the current
# directory (PORT_BUILD_DIR), and copies src/ files from PORT_META_DIR (if present).
# Must be called from within an overridden fetch() hook in a port package.sh.
fetch_rust_crate() {
    local crate_name="$1"
    local version="$2"
    local tarball_sha256="$3"

    local serenity_dir
    serenity_dir="$(realpath "${PORT_META_DIR}/../..")"
    local tarballs_dir="${serenity_dir}/Toolchain/Tarballs"
    local tarball="${crate_name}-${version}.tar.gz"
    local tarball_url="https://crates.io/api/v1/crates/${crate_name}/${version}/download"
    local workdir="${crate_name}-${version}"

    mkdir -p "${tarballs_dir}"

    if [ ! -f "${tarballs_dir}/${tarball}" ]; then
        echo "==> Fetching ${tarball}..."
        curl -L -o "${tarballs_dir}/${tarball}" "${tarball_url}"
    fi

    local actual_sha256
    actual_sha256="$(sha256sum "${tarballs_dir}/${tarball}" | awk '{print $1}')"
    if [ "${actual_sha256}" != "${tarball_sha256}" ]; then
        echo "ERROR: checksum mismatch for ${tarball}"
        echo "  expected: ${tarball_sha256}"
        echo "  actual:   ${actual_sha256}"
        exit 1
    fi

    if [ ! -d "${workdir}" ]; then
        echo "==> Extracting ${tarball}..."
        tar -xf "${tarballs_dir}/${tarball}"
    fi

    # Copy new source files into the workdir before the standard patch step runs
    if [ -d "${PORT_META_DIR}/src" ]; then
        echo "==> Copying src/ files..."
        cp -r "${PORT_META_DIR}/src/." "${workdir}/"
    fi
}

# install_rust_crate NAME VERSION
# Copies the patched workdir from PORT_BUILD_DIR to Toolchain/Rust/crates/NAME.
# Must be called from within an overridden install() hook in a port package.sh.
install_rust_crate() {
    local crate_name="$1"
    local version="$2"

    local serenity_dir
    serenity_dir="$(realpath "${PORT_META_DIR}/../..")"
    # Derive install dir from port name (strip 'rust-' prefix) so versioned ports
    # like rust-getrandom-v2 and rust-getrandom-v3 can coexist.
    local dir_name="${port#rust-}"
    local dest="${serenity_dir}/Toolchain/Rust/crates/${dir_name}"
    local workdir="${crate_name}-${version}"

    echo "==> Installing ${crate_name} ${version} to ${dest}..."
    rm -rf "${dest}"
    mkdir -p "$(dirname "${dest}")"
    cp -r "${workdir}" "${dest}"
    echo "${version}" > "${dest}/.serenity_patched"
    echo "==> rust-${crate_name} ${version} installed"
}
