# Agent Notes

## Build System

Run `./cb build` to list all available targets and tools. Build a target with `./cb build <platform>:<arch>:<sysroot>`. Set `BUILD_TYPE=(dbg|rel)` for debug or release mode.

Consult `examples/blam/cblam-testing/BUILDING.md` for more specific instructions.

Run a native target with `./cb run <platform>:<arch>:<sysroot>/<Binary>`; the binary must be named. Parameters to the program are passed after `--`. `cb run` does not rebuild, so build first. BlamGraphics takes its assets directory first, then the map:

```sh
./cb run desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics -- \
    multi_build/desktop-x86_64-buildroot-linux-gnu-multi/examples/blam/cblam-testing/assets/ \
    /mnt/blam/pc/bloodgulch.map
```

### Primary build targets

| Target | Description |
|--------|-------------|
| `desktop:x86_64-buildroot-linux-gnu:multi` | `Linux x86_64v3 AVX2 Core OpenGL (SDL2)` - cross-compiler, GCC 16.2 C++23 |
| `desktop:x64-linux:multi` | `Linux x86_64 AVX2 Core OpenGL (SDL2)` - system libs |
| `desktop:aarch64-buildroot-linux-gnu:multi` | `Linux ARMv8.1 OpenGL ES 3.x (SDL2)` |

### Other platforms

| Target | Description |
|--------|-------------|
| `android:arm64:35` | Android 15 ARM64 |
| `android:arm64:32` | Android 12 ARM64 |
| `android:arm64:30` | Android 11 ARM64 |
| `console:powerpc-eabi:cube` | Gamecube |
| `console:powerpc-eabi:wii` | Wii |
| `beaglebone:arm-buildroot-linux-gnueabihf:ti-sgx` | BeagleBone ARMv7A + TI-SGX GL ES 2.0 |
| `desktop:x86_64-w64-mingw32:posix` | `Windows x86_64 (SDL2/MinGW)` |
| `web:wasm32-emscripten:webgl2` | Wasm32 for Web + WebGL2 (SDL2) |

### Other tools

| Command | Description |
|---------|-------------|
| `./cb format` | Format code according to `.clang-format` |
| `./cb query-source` | Search through codebase |
| `./cb get-notes` | Get notes/todos from code |
| `./cb version` | Manage project version |

## Component System (`compo` namespace)

### Header layout

- `entity_container.h` — defines `EntityContainer`, forward-declares `EntityRef<T>` and `ComponentRef<T,C>`
- `entity_reference.h` — defines `EntityRef<T>` and `ComponentRef<T,C>`, includes `entity_container.h`
- `proxy.h` — defines `ContainerProxy` and `ConstrainedProxy<C,S,Svc>`, includes `entity_reference.h`

Headers that need the full `EntityRef` definition should include `entity_reference.h`, not `entity_container.h`.

### Proxy hierarchy

```
EntityContainer            (full power - framework only)
  +-- ContainerProxy       (select, match, ref, create_entity, remove_entity_if, service)
        +-- ConstrainedProxy<C,S,Svc>  (get, subsystem, service - constrained by manifest type lists)
```

- `ConstrainedProxy::service<T>()` has a `requires` constraint limiting access to the manifest's `ServiceList`. It hides the unconstrained `ContainerProxy::service<T>()` via C++ name hiding — this is intentional.
- `ContainerProxy::ref<ContainerType>(entity)` (templated) returns `EntityRef<ContainerType>` routed through the proxy. `ContainerProxy::ref(entity)` (non-templated) returns `EntityRef<EntityContainer>` via the underlying container.

### GCC and incomplete types

GCC checks non-dependent return types at definition time, even inside templates. A `template<int = 0>` wrapper does **not** defer checking of `EntityRef<EntityContainer>` because that type doesn't depend on the `int` parameter. Prefer including the defining header over template tricks.

### Threading

`exec()` runs serially unless worker threads are asked for: `COFFEE_ECS_THREADS=<n|auto>`, or `EntityContainer::set_worker_count()`. `COFFEE_ECS_WARMUP` (default 2) is how many frames run serially first, so a subsystem's undeclared access is on record before the schedule is trusted.

A subsystem leaves the main thread only if it opts in with `parallel_safe()` **and** the schedule finds it safe. It is kept on the main thread if it is `main_thread_only()`, mutates entity structure, or reached outside its manifest (`opaque`). A subsystem without a manifest counts as opaque — declaring nothing is not the same as touching nothing.

Three virtuals say what a subsystem is, and getting them right is what keeps the schedule from degenerating into a serial list:

| | Meaning | Default |
|---|---|---|
| `has_frame_work()` | runs anything at all | detected: `SubsystemBase`'s default hooks record that *they* were the ones that ran, so a state-only subsystem (cache, parameter block) is found automatically after the first frame and nothing is ordered against it. `comp_app::AppService` overrides the hooks, so it deduces the same thing from its `start_restricted`/`end_restricted` instead |
| `self_access()` | what the hooks do to state others read out of it | `write` when there is work. A service that is only ever asked questions returns `read`, and then its readers need not wait for it — `sdl2::GLFramebuffer` does, because the swap blocks for a vblank and must not fence the frame |
| `declares_access()` | `declared_*()` describe the hooks | false; true for `RestrictedSubsystem` and `comp_app::AppService`. A subsystem whose hook genuinely touches nothing in the container can say true with empty lists (`comp_app::FileWatcher`) |

Because it is detected rather than declared, a subsystem that grows a frame hook stops being state-only on its own. An override that chains to `SubsystemBase::start_frame` would break the detection — there is nothing in the base worth chaining to.

Ordering is whatever `sched::conflicts()` says it is: two subsystems that share a written component/subsystem/service, or that name each other, cannot run at once, and the one earlier in priority order runs first. `sched::build_windows()` widens an offloaded subsystem's batch into the span of batches it can cover, stopping after the batch of the last conflicting predecessor and before the batch of the first conflicting successor — so a data dependency holds *inside* the frame, not just at its edges. Undeclared dependencies (a raw reference taken in a constructor) are invisible to this; put them in the manifest.

Every job is joined inside the frame that submitted it, so visitors, frame-end callbacks and `advance_frame()` always see a quiet container. Worker threads are spawned once and park between frames. `COFFEE_ECS_SCHEDULE=<frame>` prints the batches, the offload windows and the measured costs.

Events crossing threads go through `comp_app::BasicEventBus`: `addQueuedEventFunction<SubEvent>()` returns a queue the consumer polls on its own thread, and `EventBus::inject()` is the thread-agnostic entry point (`process()` runs handlers on the caller's thread).

## Comments and commits

- When wanting to land changes in the user's checkout, it's fair to ask the user to make a commit of their changes to keep things cleaner, especially if it collides with the work to be committed
- Code comments are terse: one line saying what, a second only for a non-obvious why. Don't narrate what the code already shows, or how a bug was found.
- Commit messages are terse: a subject line, plus at most 1-2 short body lines when the subject isn't enough. A particularly hard issue (subtle root cause, non-obvious fix) can justify a longer message, but that is the exception, not the norm.
- When landing work in the user's checkout, make one commit per feature so it can be reviewed piece-wise, rather than leaving several features mixed in unstaged changes.
- Only stage your own hunks in said commits. Avoid pulling in unstaged or staged changes in the user's checkout. One way of doing this is to create the commit in your own worktree and cherry-picking it over if possible
- Co-author attribution is not considered too much
- Refrain from using ligatures for arrows, emdash and etc, as these are unsightly

## When working on Halo/BlamGraphics

There are additional instructions in `examples/blam/cblam-testing/BUILDING.md` and `examples/blam/cblam-testing/AGENTS.md`
