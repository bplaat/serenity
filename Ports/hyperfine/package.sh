#!/usr/bin/env -S bash ../.port_include.sh
port='hyperfine'
version='1.20.0'
files=(
    "https://github.com/sharkdp/hyperfine/archive/refs/tags/v${version}.tar.gz#f90c3b096af568438be7da52336784635a962c9822f10f98e5ad11ae8c7f5c64"
)
workdir="hyperfine-${version}"
# Dependencies are handled via [patch.crates-io] in Toolchain/Rust/.cargo/config.toml
# which automatically uses all crates in Toolchain/Rust/crates/

# Run Toolchain/BuildRust.sh first to set up the Rust toolchain (rustup + rust-std).

build() {
    local serenity_source_dir
    serenity_source_dir="$(realpath "${PORT_META_DIR}/../..")"

    local rust_dir="${serenity_source_dir}/Toolchain/Rust"

    # Regenerate cargo config so [patch.crates-io] reflects all installed crates
    bash "${rust_dir}/generate-cargo-config.sh"

    local cargo_config="${rust_dir}/.cargo/config.toml"

    # Detect target from the generated cargo config
    local serenity_target
    serenity_target="$(grep '^\[target\.' "${cargo_config}" | head -1 | sed 's/\[target\.\(.*\)\]/\1/')"
    if [ -z "${serenity_target}" ]; then
        echo "ERROR: Could not detect Serenity target from ${cargo_config}."
        exit 1
    fi

    local target_json="${rust_dir}/${serenity_target}.json"
    if [ ! -f "${target_json}" ]; then
        echo "ERROR: ${target_json} not found. Run Toolchain/BuildRust.sh first."
        exit 1
    fi

    host_env

    # No need to explicitly update Cargo.lock - the [patch.crates-io] in config
    # will automatically use the patched crates when resolving dependencies

    cargo +nightly build \
            -Z build-std=std,panic_abort \
            -Z json-target-spec \
            --config "${cargo_config}" \
            --manifest-path "${PORT_BUILD_DIR}/${workdir}/Cargo.toml" \
            --target-dir "${PORT_BUILD_DIR}/target" \
            --target "${target_json}" \
            --release
}

install() {
    local serenity_source_dir
    serenity_source_dir="$(realpath "${PORT_META_DIR}/../..")"

    local rust_dir="${serenity_source_dir}/Toolchain/Rust"
    local cargo_config="${rust_dir}/.cargo/config.toml"

    local serenity_target
    serenity_target="$(grep '^\[target\.' "${cargo_config}" | head -1 | sed 's/\[target\.\(.*\)\]/\1/')"
    local target_arch="${serenity_target%%-*}"

    local install_root="${serenity_source_dir}/Build/${target_arch}clang/Root"
    mkdir -p "${install_root}/usr/local/bin"
    cp "${PORT_BUILD_DIR}/target/${serenity_target}/release/hyperfine" \
       "${install_root}/usr/local/bin/hyperfine"
    echo "Installed hyperfine to ${install_root}/usr/local/bin/hyperfine"
}
