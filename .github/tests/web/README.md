# WebGL software-render smoke test

Headless, software-rendered execution test for the emscripten **WebGL2 BlamGraphics**
build. It is to the wasm build what `Test_x86_64_mesa` (xvfb + llvmpipe) is to the desktop
GL/GLES builds: it proves the binary actually *runs*, not just that it *compiles*.

Today the wasm build is only compiled and deployed to GitHub Pages — never executed in CI.
This harness closes that gap.

## What it does

1. Serves the emscripten bundle over HTTP with the cross-origin isolation headers wasm
   needs (`COOP: same-origin`, `COEP: require-corp`).
2. Launches headless Chromium backed by **SwiftShader** (pure-software GL via ANGLE) —
   no GPU required.
3. Loads `BlamGraphics.html`, then verifies, from the outside:
   - a **WebGL2** context was created,
   - the app's **render loop runs** (frames + draw calls accumulate),
   - the **renderer is SwiftShader** (software),
   - **no wasm abort/trap** and **no WebGL context loss**.
4. Writes `<name>.jpg` (canvas screenshot) and `output.log` into `OUT_DIR`, laid out so
   `.github/tests/test_page.py` can fold it into the same HTML report as the desktop
   software-render tests.

## Run locally

```bash
.github/tests/web/run_webgl_test.sh [BUNDLE_DIR] [OUT_DIR]
```

`BUNDLE_DIR` defaults to the first `BlamGraphics.bundle` found under
`multi_build/web-*emscripten*`. First run installs Playwright + Chromium
(`~/.cache/ms-playwright`).

Or drive `webgl_smoke.mjs` directly:

```bash
cd .github/tests/web && npm install && npx playwright install chromium
BUNDLE_DIR=.../BlamGraphics.bundle OUT_DIR=/tmp/webgl_test node webgl_smoke.mjs
```

### Knobs (env vars)

| Var | Default | Meaning |
|-----|---------|---------|
| `RUN_SECONDS` | `12` | how long to run the render loop after boot |
| `MIN_FRAMES` | `5` | minimum frames to consider it "rendering" |
| `BOOT_TIMEOUT_MS` | `45000` | boot wait timeout |
| `SCREENSHOT_NAME` | `webgl2_swiftshader` | screenshot base name |
| `SCREENSHOT_QUALITY` | `80` | JPEG quality |
| `STRICT` | unset | also fail on **any** page-level JS error |
| `DUMMY_PLUG` | unset | Tier 2: dummy_plug config path (see below) |

## No-map smoke vs. STRICT

With no map fed in, the app boots to its idle scene (radial backdrop + ImGui debug UI) and
logs expected, non-fatal errors (failed map fetch → `map_file_too_small`, 404s). These flip
emscripten's status banner to "Exception thrown" — a **false positive** for a no-map run, so
page errors are counted and reported but do not fail the run. Once a map is fed in and the
app is expected to be error-clean, run with `STRICT=1`.

## Tier 2 — scripted scenes via dummy_plug (not yet wired)

The desktop tests drive deterministic scenes via `dummy_plug_*.json` (`DUMMY_PLUG_CONFIG`
env → input/screenshot/custom events, deterministic frame stepping, self-close at
`end_time`). `webgl_smoke.mjs` already has the browser-side plumbing: `installDummyPlug()`
preloads the JSON into MEMFS and sets `Module.ENV.DUMMY_PLUG_CONFIG` in `preRun`.

It is inert until the **engine** supports it on emscripten:

- `bundle.cpp:125` — `enable_screenshots = !is_emscripten` compiles screenshot capture out
  on web; lift this so screenshots can be read back from MEMFS.
- `comp_app::dummy_plug` is currently disabled on web (the
  `FEATURE_ENABLE_ComponentBundleSetup_DummyPlug` feature). The multi-config path uses
  `fork()` (unavailable on emscripten), but the single-config path
  (`versions.size()==1 || swrender=="none"`) is fork-free and should work.

Once those land, pass `DUMMY_PLUG=.github/tests/dummy_plug_es_30.json` and read the captured
screenshot back from MEMFS for pixel comparison against the desktop GLES run.

## CI wiring (proposed)

Add a job to `.github/workflows/10-compile.yml` after `Linux`, mirroring `Test_x86_64_mesa`:
download the `web-wasm32-emscripten-webgl2` artifact, run `run_webgl_test.sh`, upload the
output dir as `dummy_plug_test_webgl2` so `Test_x86_64_mesa_gather` includes it in the HTML
report. Not added yet — see the chat thread for the decision to land the local harness first.
