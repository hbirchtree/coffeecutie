# Browser test drivers

Playwright drivers for the wasm builds. They are run by `.github/tests/run.py`,
which serves the bundle, picks the Chromium flags, writes the driver a JSON
config and judges the `result.json` it leaves behind. A driver only operates the
page and reports what it saw.

| run.py command | Driver | What it checks |
|----------------|--------|----------------|
| `webgl` | `smoke.mjs` | BlamGraphics boots on SwiftShader: WebGL2 context, live render loop, draw calls, no wasm trap. Screenshot, `.webm` and `output.log` land in `--out` for `test_page.py` |
| `web-client` | `webrtc_client.mjs` | A wasm client through a webrtc-gateway reaches connect, join and roster sync, against a native server or (`--server-role wasm`) a second wasm page |
| `nat-punch` | `webrtc_client.mjs` | The same, against an internet gateway with the native server behind NAT (`GATEWAY_URL`) |
| `pose-webcam` | `smoke.mjs` | BlamPoseDemo: a fake webcam playing a person drives MediaPipe and Kalidokit into pose payloads |
| `pose-arms` | `pose_arms.mjs` | BlamPoseDemo: exact arm directions in, bone positions out (headful, real GPU; SwiftShader loses the context on this bundle) |

```bash
.github/tests/run.py webgl [BUNDLE] --map xbox_ex/beavercreek.map
.github/tests/run.py web-client [BUNDLE] --server-role wasm
.github/tests/run.py pose-arms [BUNDLE]
```

`BUNDLE` is the `.bundle` directory or a directory to search; by default the
first one under `multi_build`. Maps come from `/mnt/blam` when present, else the
map server (`MAP_ACCESS_TOKEN`); `webgl` runs without a map when neither has it.
`run.py <command> --help` lists the knobs.

`webgl` also covers the debugging variants: `--gpu` (headful on the real GPU),
`--gl-trace` (GL call and error counts), `--profile-dir` with `--port` (a
persistent profile that keeps OPFS), `--no-range`, and `--dummy-plug` to stream
a plug's events into the page through `coffee_dummy_plug_event`.

## Layout

- `lib.mjs`: config, logging, browser launch, console capture (fatal patterns,
  markers, counters, frame-loop stalls), `result.json`
- `page/`: init scripts injected before the page runs: `instrument.js` (context,
  frames, draws), `dummy_plug.js`, `gl_tracer.js`
- `gl_trace_*.mjs`: the GL trace collector and viewer, see `WEBGL_TRACE.md`

No map, no problem: without one the app boots its idle scene, a failed fetch
flips the status banner to "Exception thrown", and page errors are counted but
only fail the run with `--strict`.
