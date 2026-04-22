# Rust Standard Library for SerenityOS

This directory contains patches and source code to add SerenityOS support to the Rust nightly standard library.

## What It Does

When installed, this port:
1. Adds `os::serenity` module to the Rust standard library
2. Patches `std::io`, `std::sys`, and other core modules for SerenityOS compatibility
3. Enables cross-compilation of Rust code targeting SerenityOS

## Files

- `package.sh` - Installation script that applies patches to the nightly toolchain
- `patches/` - Unified diff patches to apply to Rust std source
- `src/os/serenity/` - New SerenityOS-specific modules:
  - `mod.rs` - Public API exports
  - `raw.rs` - Raw system bindings
  - `fs.rs` - Filesystem module

## Installation

This is normally installed automatically by running:

```bash
./Toolchain/BuildRust.sh
```

Or manually:

```bash
./Toolchain/Rust/std/package.sh
```

The script:
1. Detects your installed nightly Rust toolchain
2. Reinstalls rust-src component to get a clean base
3. Copies source files from `src/` directory
4. Applies patches from `patches/` directory
5. Marks the installation with a version marker

## Prerequisites

Before running this script, ensure:

```bash
rustup toolchain install nightly
rustup component add rust-src --toolchain nightly
```

## SerenityOS Support

The patches add:
- `#[cfg(target_os = "serenity")]` conditional compilation blocks
- `os::serenity::raw` module with SerenityOS syscall definitions
- `os::serenity` module with platform-specific APIs
- Thread name support for SerenityOS
- Platform detection in `std::sys`

## Version Tracking

The script creates `.serenity_patched` file in the std source directory with the nightly version, preventing re-patching when already patched for that version.

## Troubleshooting

### "No nightly toolchain found"

Install nightly:
```bash
rustup toolchain install nightly
```

### "rust-src not found"

Add the rust-src component:
```bash
rustup component add rust-src --toolchain nightly
```

### Patches fail to apply

This usually means:
1. A newer nightly version changed the std structure
2. Patches were already applied
3. The wrong nightly version is active

Try reinstalling rust-src:
```bash
rustup component remove rust-src --toolchain nightly
rustup component add rust-src --toolchain nightly
```

Then re-run the patch script.
