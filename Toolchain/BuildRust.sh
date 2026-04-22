#!/usr/bin/env bash
# Set up the Rust cross-compilation toolchain for SerenityOS.
# This script installs the nightly toolchain, patches Rust std, and generates
# the cargo config. Patched crates are installed as port dependencies.
#
# Run this once after setting up the SerenityOS build, then build Rust ports
# with their individual package.sh files which will auto-install crate dependencies.
set -euo pipefail

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

. "${DIR}/../Meta/shell_include.sh"

exit_if_running_as_root "Do not run BuildRust.sh as root"

echo "=== Checking rustup ==="
if ! command -v rustup &> /dev/null; then
    echo "ERROR: rustup is not installed."
    echo "Please install it first: https://rustup.rs/"
    exit 1
fi

echo "=== Installing Rust nightly toolchain ==="
rustup toolchain install nightly

echo "=== Installing rust-src component ==="
rustup component add rust-src --toolchain nightly

echo "=== Patching Rust std for SerenityOS ==="
bash "${DIR}/Rust/std/package.sh"

echo "=== Generating cargo config ==="
bash "${DIR}/Rust/generate-cargo-config.sh" "${SERENITY_ARCH:-}"

RUST_DIR="${DIR}/Rust"
CARGO_CONFIG="${RUST_DIR}/.cargo/config.toml"
TARGET_JSON_GLOB=("${RUST_DIR}"/*-unknown-serenity.json)
TARGET_JSON="${TARGET_JSON_GLOB[0]}"

echo ""
echo "=== Rust SerenityOS toolchain setup complete! ==="
echo ""
echo "Cargo config: ${CARGO_CONFIG}"
echo ""
echo "To build a Rust port, use:"
echo "  cargo +nightly build \\"
echo "    -Z build-std=std,panic_abort \\"
echo "    -Z json-target-spec \\"
echo "    --config ${CARGO_CONFIG} \\"
echo "    --target ${TARGET_JSON} \\"
echo "    --release"
