#!/usr/bin/env bash
# Rust std port: applies SerenityOS patches to the nightly Rust standard library.
# Must be run after: rustup toolchain install nightly && rustup component add rust-src --toolchain nightly
set -euo pipefail

PORT_META_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

NIGHTLY_TOOLCHAIN="$(rustup toolchain list | grep nightly | head -1 | awk '{print $1}')"
if [ -z "${NIGHTLY_TOOLCHAIN}" ]; then
    echo "ERROR: No nightly toolchain found. Run: rustup toolchain install nightly"
    exit 1
fi

RUST_SRC="${HOME}/.rustup/toolchains/${NIGHTLY_TOOLCHAIN}/lib/rustlib/src/rust/library"
if [ ! -d "${RUST_SRC}" ]; then
    echo "ERROR: rust-src not found at ${RUST_SRC}"
    echo "       Run: rustup component add rust-src --toolchain nightly"
    exit 1
fi

SERENITY_DIR="${RUST_SRC}/std/src/os/serenity"
MARKER_FILE="${SERENITY_DIR}/.serenity_patched"
MARKER_CONTENT="$(rustc +nightly --version | awk '{print $2}')"

if [ -f "${MARKER_FILE}" ] && [ "$(cat "${MARKER_FILE}")" = "${MARKER_CONTENT}" ]; then
    echo "==> rust-std already patched for SerenityOS (${NIGHTLY_TOOLCHAIN})"
    exit 0
fi

echo "==> Patching Rust std for SerenityOS (${NIGHTLY_TOOLCHAIN})..."

# Reinstall rust-src to get a pristine base (cached by rustup, fast on repeat runs)
echo "==> Reinstalling rust-src (for clean base)..."
rustup component remove --toolchain nightly rust-src 2>/dev/null || true
rustup component add --toolchain nightly rust-src

# Copy new source files from the port's src/ directory
echo "==> Copying src/ files..."
mkdir -p "${SERENITY_DIR}"
cp -r "${PORT_META_DIR}/src/." "${RUST_SRC}/std/src/"

# Apply patches to existing files (patch -N skips already-applied hunks without prompts)
echo "==> Applying patches..."
for patch in "${PORT_META_DIR}/patches/"*.patch; do
    pname="$(basename "${patch}")"
    echo "    Applying ${pname}..."
    if ! (cd "${RUST_SRC}" && patch -p1 -N < "${patch}"); then
        echo "ERROR: Failed to apply ${pname}"
        exit 1
    fi
done

echo "${MARKER_CONTENT}" > "${MARKER_FILE}"
echo "==> Rust std patched for SerenityOS"
