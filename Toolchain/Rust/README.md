# Rust Toolchain for SerenityOS

This directory contains the Rust cross-compilation toolchain and patched crates for building Rust software for SerenityOS.

## Structure

- `std/` - Patches and sources for SerenityOS support in Rust's standard library
- `crates/` - Patched Rust crates that require SerenityOS-specific modifications
- `*-unknown-serenity.json` - Target specifications for SerenityOS (aarch64, x86_64, riscv64)
- `.cargo/config.toml` - Generated Cargo configuration for cross-compilation
- `generate-cargo-config.sh` - Script to generate Cargo configuration from installed crates

## Setup Instructions

### 1. Initial Toolchain Setup

Run the build script to set up the Rust toolchain:

```bash
./Toolchain/BuildRust.sh
```

This will:
- Install the nightly Rust toolchain
- Add rust-src component
- Patch the Rust standard library with SerenityOS support
- Generate the initial Cargo configuration

### 2. Building Rust Ports

Each Rust port (located in `Ports/rust-*/`) provides:
- Patched source code for Rust crates
- Patches to existing crate functionality
- A `package.sh` script that integrates with SerenityOS's port system

To build a Rust port:

```bash
cd Ports/[port-name]
./package.sh
```

Common port names follow the pattern: `rust-${crate_name}(-${version})`

### 3. Building Rust Applications

Rust application ports (like `hyperfine`, `ripgrep`) require the toolchain and patched crates to be set up first. They use the `[patch.crates-io]` mechanism in the Cargo configuration to use locally-patched versions of required crates.

Build a Rust application:

```bash
cd Ports/[application-name]
./package.sh
```

## Patched Crates

The following crates have SerenityOS-specific patches:

| Crate | Version | Purpose |
|-------|---------|---------|
| `libc` | 0.2.185 | SerenityOS libc bindings |
| `rustix` | 1.1.2 | Safe low-level system calls for SerenityOS |
| `errno` | 0.3.14 | errno location for SerenityOS |
| `rand_os` | 0.1.3 | OS random number source for SerenityOS |
| `getrandom` | 0.2.16, 0.3.4 | Cryptographic randomness for SerenityOS |

When you install these ports, they are automatically registered in the Cargo configuration via `[patch.crates-io]` entries, ensuring applications build with these patched versions.

## Adding New Rust Ports

### For Patched Crates

1. Create a new port directory: `Ports/rust-${crate_name}(-${version})/`
2. Add `package.sh` that:
   - Sources `.rust_crate_include.sh`
   - Calls `fetch_rust_crate()` for downloading
   - Calls `install_rust_crate()` in the install step
3. Add patches to `patches/` directory if needed
4. Add new source files to `src/` directory if needed
5. Update `Ports/AvailablePorts.md`

Example `package.sh`:

```bash
#!/usr/bin/env -S bash ../.port_include.sh
port='rust-mylib'
version='1.0.0'
files=()
workdir="mylib-${version}"

fetch() {
    source "${PORT_META_DIR}/../.rust_crate_include.sh"
    fetch_rust_crate "mylib" "${version}" "[SHA256-HASH]"
}

configure() { :; }
build() { :; }

install() {
    source "${PORT_META_DIR}/../.rust_crate_include.sh"
    install_rust_crate "mylib" "${version}"
}
```

### For Rust Applications

1. Create a new port directory: `Ports/[app-name]/`
2. Add source tarball and add its URL with checksum to `files=()` in `package.sh`
3. Implement `build()` that:
   - Calls `generate-cargo-config.sh`
   - Runs `cargo +nightly build` with:
     - `-Z build-std=std,panic_abort` (use patched std)
     - `-Z json-target-spec` (use JSON target spec)
     - `--target serenity-target.json`
     - Any application-specific flags
4. Implement `install()` that copies the binary to the appropriate location
5. Declare dependencies on patched crates via `depends=(rust-libc rust-rustix ...)`
6. Update `Ports/AvailablePorts.md`

## Cargo Configuration

The `.cargo/config.toml` is generated automatically by `generate-cargo-config.sh`. It:

1. Sets the linker to the appropriate SerenityOS cross-compiler
2. Adds `--sysroot` to link against SerenityOS libraries
3. Registers all installed patched crates via `[patch.crates-io]`

To regenerate after installing new crate ports:

```bash
./Toolchain/Rust/generate-cargo-config.sh
```

## Dependency Strategy

We use a **hybrid approach** for dependencies:

- **Patched Crates**: Only crates that require SerenityOS-specific code are ported and patched
- **Pure Rust Crates**: Other dependencies are fetched from crates.io at build time

This balances:
- Transparency (patched crates are explicit)
- Maintainability (fewer crates to patch)
- Correctness (pure-Rust code doesn't need patching)

## Troubleshooting

### Checksum Mismatch

If a port fails with checksum mismatch errors:

1. Verify the SHA256 in the port's `package.sh`
2. Update it if the upstream changed:
   ```bash
   sha256sum Toolchain/Tarballs/[crate]-[version].tar.gz
   ```
3. Update the checksum in the port's `fetch()` function

### Missing SerenityOS Target

If you see "could not find target specification", ensure `BuildRust.sh` has been run and `Toolchain/Rust/*.json` files exist.

### Cargo Build Failures

If cargo build fails:

1. Check that all patched crates are installed: `ls Toolchain/Rust/crates/`
2. Verify `.cargo/config.toml` was generated correctly
3. Ensure the nightly toolchain is installed: `rustup toolchain list`

## References

- [Rust Platform Support](https://forge.rust-lang.org/release/platform-support.html)
- [Cargo Documentation](https://doc.rust-lang.org/cargo/)
- [SerenityOS Build System](../../README.md)
