#!/usr/bin/env -S bash ../.port_include.sh
port='v8'
version='14.8.102'
useconfigure='true'
files=(
    "git+https://chromium.googlesource.com/v8/v8.git#${version}"
)
depends=(
    'gn'
    'ninja'
    'python3'
    'abseil'
    'zlib'
    'libicu'
)

# Map SerenityOS arch names to V8 target_cpu names
case "$SERENITY_ARCH" in
    x86_64)  v8_cpu='x64' ;;
    aarch64) v8_cpu='arm64' ;;
    riscv64) v8_cpu='riscv64' ;;
    *)       echo "Unsupported arch: $SERENITY_ARCH"; exit 1 ;;
esac

pre_configure() {
    # V8 requires the Chromium 'build/' infrastructure that depot_tools normally
    # provides via gclient. Fetch it at the commit pinned in V8's DEPS file.
    local build_rev
    build_rev=$(grep -A1 '"build"' "${workdir}/DEPS" | grep -oP "(?<=')\w+(?=')" | head -1)
    if [ -z "$build_rev" ]; then
        echo "Could not determine build/ commit from DEPS"
        exit 1
    fi
    echo "Fetching Chromium build/ at ${build_rev}..."
    git clone --no-checkout \
        https://chromium.googlesource.com/chromium/src/build \
        "${workdir}/build"
    git -C "${workdir}/build" checkout "$build_rev"
}

configure() {
    run gn gen out/serenity --args="
        target_os=\"serenity\"
        target_cpu=\"${v8_cpu}\"
        is_debug=false
        is_component_build=false
        v8_use_external_startup_data=false
        v8_enable_i18n_support=true
        v8_enable_webassembly=false
        v8_jitless=true
        use_custom_libcxx=false
        clang_use_chrome_plugins=false
        use_sysroot=false
        treat_warnings_as_errors=false
        use_system_zlib=true
        absl_in_chromium=false
    "
}

build() {
    run ninja -C out/serenity d8
}

install() {
    run mkdir -p "${SERENITY_INSTALL_ROOT}/usr/local/bin"
    run mkdir -p "${SERENITY_INSTALL_ROOT}/usr/local/include/v8"
    run cp out/serenity/d8 "${SERENITY_INSTALL_ROOT}/usr/local/bin/d8"
    run cp -r include/v8*.h "${SERENITY_INSTALL_ROOT}/usr/local/include/v8/"
}
