# WebGL / GL call trace

Every generated `glw` wrapper can record its call, arguments, GL errors, upload
data, texture contents and framebuffer captures. Records are packed binary and
streamed out as they are produced. On web they go over a WebSocket to a Node
collector, so the trace survives a crashed page. On other platforms they go to
a file.

## 1. Build with tracing

Tracing is compiled out by default. Configure with:

```
-DBUILD_GL_TRACE=ON
```

This defines `GLW_ENABLE_TRACE`. On Emscripten it also links `-lwebsocket.js`.

## 2. Run it

### Easiest: `remote-run.py` (web devices)

```
./remote-run.py <web-device> <preset> --gleam-debug data [--serve] [-- app args]
```

- Starts `.github/tests/web/gl_trace_server.mjs` and stops it after the run.
- Adds `gleamDebug=<level>`, `gleamDebugFrames=frame` and
  `gleamDebugPort=<port>` to the page URL.
- Without `--serve`, the headless `webgl_smoke.mjs` run is traced. With
  `--serve`, it serves the bundle so you can open it in your own browser. The
  collector runs until you stop it.
- Output goes to `/tmp/remote-run-web/<preset>/gltrace/`, or to
  `<DIR>/gltrace/` with `--collect-profile DIR`.

| Flag | Meaning |
|---|---|
| `--gleam-debug calls\|errors\|data\|textures` | Trace level (see below) |
| `--gleam-debug-port PORT` | Collector port, default 8099 |
| `--dry-run` | Print the collector command without running anything |

Only the web runner acts on `--gleam-debug`. The Linux, Docker, Android and
Dolphin runners accept the flag and ignore it.

### Manually (web)

```
node .github/tests/web/gl_trace_server.mjs [--port 8099] [--out /tmp/gltrace]
```

Then open the page with query parameters. Setting the matching `window.*`
global before the runtime starts also works.

| URL parameter / global | Meaning | Default |
|---|---|---|
| `gleamDebug` | Trace level | off |
| `gleamDebugPort` | Collector port, on `127.0.0.1` | 8099 |
| `gleamDebugFrames` | `frame` = capture the colour buffer per frame, `draw` = per draw | off |
| `gleamDebugShot` | Longest edge of captured images, in pixels | 480 |
| `gleamDebugBytes` | Max data bytes kept per record (also caps strings, e.g. shader source) | 4096 |

`COFFEE_GL_TRACE_URL=ws://host:port` overrides the collector address entirely.

### Native (desktop, Android, …)

Set environment variables. The trace is written to a file, and the collector
is not used.

| Variable | Meaning | Default |
|---|---|---|
| `COFFEE_GL_TRACE` | Trace level | off |
| `COFFEE_GL_TRACE_FILE` | Output path | `gltrace.bin` |
| `COFFEE_GL_TRACE_FRAMES` | `frame` / `draw` capture | off |
| `COFFEE_GL_TRACE_SHOT` | Longest edge of captured images | 480 |
| `COFFEE_GL_TRACE_BYTES` | Max data bytes per record | 4096 |

`COFFEE_GL_TRACE*` variables also apply on web if set through `Module.ENV`, but
the URL parameters are the practical route there.

## Levels

Each level includes everything below it.

| Level | Adds |
|---|---|
| `calls` | Function names and arguments, debug groups, object labels |
| `errors` | `glGetError()` after every call. Forces a sync, so expect it to be slow |
| `data` | The data spans passed to uploads, capped by the bytes setting. The full declared size is still recorded |
| `textures` | The contents of each texture once, on its first bind. Up to 4 MiB each, at most 4 dumps per texture |

Framebuffer capture (`frame`/`draw`) is independent of the level. Each capture
is a full readback and a sync, and per-draw capture is much slower.

WebGL has no `KHR_debug`, so the engine's debug scopes, object names and debug
messages are sent to the trace instead.

## 3. Read the output

The collector writes into its `--out` directory:

| File | Contents |
|---|---|
| `trace.log` | Readable call log, indented by debug group |
| `trace.gltrace` | Single container with blobs, images, per-frame events and an index footer |
| `viewer.html` | Timeline viewer |
| `*.png`, blobs | Captures and data blobs, also written as loose files |

Open `viewer.html` in a browser and drop `trace.gltrace` onto it. It is read
through the File API, so you don't need a server. The viewer reads only the
footer index plus the frame on screen, so large traces scrub quickly.

Headless check of the viewer (needs Playwright):

```
node .github/tests/web/gl_trace_viewer_check.mjs <viewer.html> <trace.gltrace> out.png
```

## Source

- `src/coffee/graphics/apis/gleam/include/glw/trace.h` — API and levels
- `src/coffee/graphics/apis/gleam/private/glw_trace.cpp` — sink, configuration, record format
- `.github/tests/web/gl_trace_server.mjs` — collector
- `.github/tests/web/gl_trace_viewer.mjs` — viewer HTML
- `remote-run.py` — `start_gleam_debug_collector`, `run_web`
