# Patches for V8 on SerenityOS

## `0001-build-Add-SerenityOS-to-BUILDCONFIG.gn.patch`

build: Add SerenityOS to BUILDCONFIG.gn

Adds `is_serenity` to the GN declare_args block and the target_os
dispatch chain. Without this patch `gn gen` exits immediately with
"unknown target_os".

Note: This patch applies to the Chromium `build/` directory, not V8
itself. The `build/` directory is fetched from Chromium's build repo
at the commit pinned in V8's DEPS file (see `pre_configure` in package.sh).


## `0002-include-Add-SerenityOS-macros-to-v8config.h.patch`

include: Add SerenityOS macros to v8config.h

Adds `V8_OS_SERENITY` / `V8_OS_POSIX` / `V8_OS_STRING` detection for
the `__serenity__` compiler macro. Without this, all `#if V8_OS_*`
checks in C++ source files fall through to wrong or absent paths.

Modeled after the existing `__HAIKU__` entry.


## `0003-base-Add-SerenityOS-platform-implementation.patch`

base: Add SerenityOS platform implementation

Adds `src/base/platform/platform-serenity.cc` with implementations of:
- `OS::CreateTimezoneCache()` — POSIX default
- `OS::GetSharedLibraryAddresses()` — parses `/proc/self/maps`
- `OS::SignalCodeMovingGC()` — no-op
- `OS::AdjustSchedulingParams()` — no-op
- `OS::GetFirstFreeMemoryRangeWithin()` — returns nullopt
- `Stack::ObtainCurrentThreadStackStart()` — via `pthread_getattr_np`
- `OS::CreateSharedMemoryHandleForTesting()` — stub
- `OS::DestroySharedMemoryHandle()` — stub

Also wires the file into `src/base/platform/BUILD.gn` under `is_serenity`.

No JIT allocator is provided: this port builds with `v8_jitless=true`.


## Iterative fix patches (0004+)

Additional `#ifdef` fixes will be added here as build failures surface.
The pattern is always: add `|| defined(__serenity__)` (or `|| V8_OS_SERENITY`)
next to existing `__HAIKU__` or `__QNX__` guards. Expected locations:

- `src/base/platform/platform-posix.cc` — POSIX guards
- `src/execution/frames.cc` — stack unwinding
- `src/base/sys-info.cc` — sysconf / getrlimit calls
- `src/base/semaphore.cc` — POSIX semaphore availability


## JIT support (future work)

JIT is explicitly disabled in this initial port (`v8_jitless=true`).
SerenityOS blocks `mprotect(PROT_EXEC)` on anonymous mappings by default
(requires `MS_AXALLOWED` mount flag). The path to full JIT:

1. Implement a `CodeAllocator` in `platform-serenity.cc` using
   file-backed mappings (`memfd_create` + `mmap(fd, MAP_SHARED)`)
   instead of anonymous `mmap`. File-backed regions are not subject to
   the anonymous-exec restriction, so `mprotect(RX)` succeeds.
2. No kernel changes or `MS_AXALLOWED` required.
3. Enable by removing `v8_jitless=true` from `package.sh` GN args.
