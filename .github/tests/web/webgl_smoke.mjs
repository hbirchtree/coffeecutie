#!/usr/bin/env node
// Headless software-rendered WebGL smoke test for the emscripten BlamGraphics build.
//
// Mirrors what Test_x86_64_mesa does for the desktop GL/GLES builds (xvfb + llvmpipe),
// but for the wasm/WebGL2 build: it serves the emscripten bundle, loads it in headless
// Chromium backed by SwiftShader (pure-software GL), and verifies that the app actually
// boots, creates a WebGL2 context and drives its render loop without aborting.
//
// It writes a canvas screenshot (<name>.jpg) and a console log (output.log) into OUT_DIR,
// laid out so .github/tests/test_page.py can fold the result into the same HTML report
// used by the desktop software-render tests.
//
// Tier 2 (scripted scenes via dummy_plug) is stubbed: see installDummyPlug() below. The
// engine currently compiles screenshots and dummy_plug out on emscripten (bundle.cpp), so
// that path needs C++ changes before it can be wired up here.

import { chromium } from 'playwright';
import { createServer } from 'node:http';
import { readFile, mkdir, writeFile, rename } from 'node:fs/promises';
import { existsSync } from 'node:fs';
import { join, normalize, extname } from 'node:path';

const cfg = {
  bundleDir: process.env.BUNDLE_DIR,
  page: process.env.PAGE || 'BlamGraphics.html',
  outDir: process.env.OUT_DIR || '/tmp/webgl_test',
  name: process.env.SCREENSHOT_NAME || 'webgl2_swiftshader',
  // Seconds to keep the render loop running after the app reports "Running...".
  runSeconds: Number(process.env.RUN_SECONDS || 12),
  // How long to wait for the app to boot and produce its first frames.
  bootTimeoutMs: Number(process.env.BOOT_TIMEOUT_MS || 45000),
  minFrames: Number(process.env.MIN_FRAMES || 5),
  screenshotQuality: Number(process.env.SCREENSHOT_QUALITY || 80),
  // Tier 2: path to a dummy_plug JSON config. Parsed now, acted on once the engine
  // supports dummy_plug on emscripten.
  dummyPlug: process.env.DUMMY_PLUG || '',
  // Record a (silent) webm of the page into OUT_DIR. On by default; VIDEO=0 disables.
  video: process.env.VIDEO !== '0',
  viewport: { width: 1280, height: 720 },
  args: process.env.ARGS || '',
};

const MIME = {
  '.html': 'text/html; charset=utf-8',
  '.js': 'text/javascript; charset=utf-8',
  '.mjs': 'text/javascript; charset=utf-8',
  '.wasm': 'application/wasm',
  '.data': 'application/octet-stream',
  '.json': 'application/json; charset=utf-8',
  '.ico': 'image/x-icon',
  '.css': 'text/css; charset=utf-8',
};

// Static server with the cross-origin isolation headers emscripten needs for
// SharedArrayBuffer / threaded builds. Setting them here avoids the coi-serviceworker
// reload dance.
function startServer(rootDir) {
  const root = normalize(rootDir);
  const server = createServer(async (req, res) => {
    try {
      const urlPath = decodeURIComponent((req.url || '/').split('?')[0]);
      const filePath = normalize(join(root, urlPath === '/' ? cfg.page : urlPath));
      if (!filePath.startsWith(root)) {
        res.writeHead(403).end('forbidden');
        return;
      }
      const body = await readFile(filePath);
      res.writeHead(200, {
        'Content-Type': MIME[extname(filePath)] || 'application/octet-stream',
        'Content-Length': body.length,
        'Cross-Origin-Opener-Policy': 'same-origin',
        'Cross-Origin-Embedder-Policy': 'require-corp',
        'Cross-Origin-Resource-Policy': 'cross-origin',
        'Cache-Control': 'no-store',
      });
      res.end(body);
    } catch {
      res.writeHead(404).end('not found');
    }
  });
  return new Promise((resolve) => {
    server.listen(0, '127.0.0.1', () => resolve({ server, port: server.address().port }));
  });
}

// Runs before any page script. Instruments getContext/draw calls/rAF so we can observe,
// from the outside, whether the app got a real WebGL2 context and is actually drawing.
function instrument() {
  const h = (window.__harness = {
    contextType: null,
    glRenderer: null,
    glVersion: null,
    frames: 0,
    drawCalls: 0,
    contextLost: false,
    glError: false,
  });
  // Context-lost normally pops an alert() which would block headless; capture instead.
  window.alert = () => { h.contextLost = true; };

  const origGetContext = HTMLCanvasElement.prototype.getContext;
  HTMLCanvasElement.prototype.getContext = function (type, ...rest) {
    const ctx = origGetContext.call(this, type, ...rest);
    if (ctx && /webgl/i.test(String(type))) {
      h.contextType = String(type);
      try {
        const dbg = ctx.getExtension('WEBGL_debug_renderer_info');
        h.glRenderer = dbg
          ? ctx.getParameter(dbg.UNMASKED_RENDERER_WEBGL)
          : ctx.getParameter(ctx.RENDERER);
        h.glVersion = ctx.getParameter(ctx.VERSION);
      } catch { /* ignore */ }
      this.addEventListener('webglcontextlost', () => { h.contextLost = true; }, false);
      for (const m of ['drawArrays', 'drawElements', 'drawArraysInstanced', 'drawElementsInstanced']) {
        const orig = ctx[m];
        if (typeof orig === 'function') {
          ctx[m] = function (...a) { h.drawCalls++; return orig.apply(this, a); };
        }
      }
    }
    return ctx;
  };

  const origRAF = window.requestAnimationFrame;
  window.requestAnimationFrame = function (cb) {
    return origRAF.call(window, (t) => { h.frames++; return cb(t); });
  };

  // Surface uncaught exceptions with their full stack. emscripten's generated
  // HTML installs its own window.onerror that only logs a one-line message, and
  // a C++ exception reaching JS often arrives as a bare value with no JS stack.
  // addEventListener coexists with that onerror; route the richest detail we can
  // get (Error.stack, or the emscripten exception message) through console.error
  // so it lands in the captured output.log. getExceptionMessage is exported by
  // emscripten when built with exception support.
  const describe = (e) => {
    if (!e) return '<no error object>';
    try {
      const M = window.Module;
      if (typeof e === 'number' && M && typeof M.getExceptionMessage === 'function') {
        return 'C++ exception: ' + M.getExceptionMessage(e).join(': ');
      }
    } catch (inner) { /* fall through to generic */ }
    return e.stack || e.message || String(e);
  };
  window.addEventListener('error', (ev) => {
    console.error('[uncaught error] ' + describe(ev.error !== undefined ? ev.error : ev.message));
  });
  window.addEventListener('unhandledrejection', (ev) => {
    console.error('[unhandledrejection] ' + describe(ev.reason));
  });
}

// Tier 2 hook: preload the dummy_plug config into MEMFS and point the app at it via
// ENV.DUMMY_PLUG_CONFIG, exactly like the desktop tests do with the env var. Inert until
// the engine enables dummy_plug + screenshot capture on emscripten.
function installDummyPlug(configText) {
  const Module = (window.Module = window.Module || {});
  Module.preRun = Module.preRun || [];
  Module.preRun.push(function () {
    try {
      Module.ENV = Module.ENV || {};
      Module.ENV.DUMMY_PLUG_CONFIG = '/dummy_plug.json';
      Module.FS.writeFile('/dummy_plug.json', configText);
    } catch (e) {
      console.error('dummy_plug preload failed:', e);
    }
  });
}

// Patterns that mean the wasm module itself trapped/died (hard failure). Page-level JS
// errors are tracked separately because a no-map smoke run legitimately produces some
// (failed map fetch, etc.) while the render loop keeps going.
const FATAL_PATTERNS = [
  /abort\(/i, /\bAborted\(/, /RuntimeError/, /Cannot enlarge memory/,
  /memory access out of bounds/i, /\bOOM\b/, /maximum call stack/i,
];

// Emscripten/wasm stack traces are extremely noisy: every frame is a full
// `at <demangled C++ signature> (http://host:port/BlamGraphics.wasm:wasm-function[N]:0xADDR)`
// line, the same trace is often logged several times in a row, and half the
// frames are the logging machinery itself. prettyStack compresses them:
//   - drops the localhost URL, keeps a short `wasm[N]` / `file:line` locator
//   - removes the jsStackTrace/emscripten_log/Coffee::Logging plumbing frames
//   - simplifies `std::__2::` spelling and trims overlong signatures
//   - collapses a trace identical to the previous one to a single note
let lastStackSig = null;
const STACK_PLUMBING =
  /\b(jsStackTrace|getCallstack|__emscripten_log_formatted|emscripten_log|Coffee::Logging::(?:detail::)?log)\b/;
function prettyStack(text) {
  if (typeof text !== 'string' || !/wasm-function|BlamGraphics\.(?:wasm|js)/.test(text)) {
    return text;
  }
  const head = [];
  const frames = [];
  for (const raw of text.split('\n')) {
    let line = raw
      .replace(/https?:\/\/[^/]+\//g, '')
      .replace(/BlamGraphics\.wasm:wasm-function\[(\d+)\]:0x[0-9a-f]+/g, 'wasm[$1]')
      .replace(/std::__2::/g, 'std::')
      .replace(/\[abi:[^\]]+\]/g, '')
      .replace(/\s+/g, ' ')
      .trimEnd();
    if (/^\s*at\s/.test(raw)) {
      if (STACK_PLUMBING.test(line)) continue; // drop logging plumbing frames
      // Trim runaway template signatures while keeping the function name + locator.
      const m = line.match(/^(at .*?)\s*\((wasm\[\d+\]|[^)]*)\)\s*$/);
      if (m && m[1].length > 140) line = `${m[1].slice(0, 137)}…) (${m[2]})`;
      frames.push('    ' + line.replace(/^at /, 'at '));
    } else if (line) {
      head.push(line);
    }
  }
  if (!frames.length) return head.join('\n');
  const sig = frames.join('|');
  if (sig === lastStackSig) {
    return [...head, '    <stack identical to previous>'].join('\n');
  }
  lastStackSig = sig;
  return [...head, ...frames].join('\n');
}

const logLines = [];
const record = (s, level = 'DEBG') => {
  const d = new Date();
  const ts = [d.getHours(), d.getMinutes(), d.getSeconds()].map(v => String(v).padStart(2, '0')).join(':');
  const msg = `${level.padEnd(4)}:${ts}: ${prettyStack(s)}`;
  logLines.push(msg);
  console.log(msg);
};

async function main() {
  let browser, server;
  try {
    if (!cfg.bundleDir || !existsSync(join(cfg.bundleDir, cfg.page))) {
      throw new Error(`bundle page not found: ${join(cfg.bundleDir || '<unset>', cfg.page)}`);
    }
    await mkdir(cfg.outDir, { recursive: true });

    const serverData = await startServer(cfg.bundleDir);
    server = serverData.server;
    const port = serverData.port;
    
    let url = `http://127.0.0.1:${port}/${cfg.page}`;
    if (cfg.args) {
      url += `?${cfg.args}`;
    }
    record(`Serving ${cfg.bundleDir} at ${url}`);

    browser = await chromium.launch({
      headless: true,
      args: [
        // SwiftShader: pure-software OpenGL ES via ANGLE. Recent Chromium gates this
        // behind --enable-unsafe-swiftshader for WebGL.
        '--use-gl=angle',
        '--use-angle=swiftshader',
        '--enable-unsafe-swiftshader',
        '--ignore-gpu-blocklist',
        '--enable-webgl',
        '--disable-dev-shm-usage',
      ],
    });

    let sawFatal = false;
    let pageErrors = 0;
    const scanFatal = (text) => {
      if (FATAL_PATTERNS.some((re) => re.test(text))) sawFatal = true;
    };

    const context = await browser.newContext({
      viewport: cfg.viewport,
      recordVideo: cfg.video ? { dir: cfg.outDir, size: cfg.viewport } : undefined,
    });
    const page = await context.newPage();
    await page.addInitScript(instrument);
    if (cfg.dummyPlug && existsSync(cfg.dummyPlug)) {
      const configText = await readFile(cfg.dummyPlug, 'utf8');
      await page.addInitScript(installDummyPlug, configText);
      record(`dummy_plug config staged: ${cfg.dummyPlug} (Tier 2 — engine support pending)`);
    }

    page.on('console', (msg) => {
      const type = msg.type();
      let level = 'DEBG';
      if (type === 'error') level = 'ERR';
      else if (type === 'warning') level = 'WARN';
      
      const text = msg.text();
      record(text, level);
      scanFatal(text);
      
      // If it's an error, also try to log the stack trace if Playwright captured it
      if (type === 'error' && msg.location()) {
        const loc = msg.location();
        record(`  at ${loc.url}:${loc.lineNumber}:${loc.columnNumber}`, 'ERR');
      }
    });
    // Page-level JS errors: counted and logged with stack trace if available.
    page.on('pageerror', (err) => {
      pageErrors++;
      const msg = err.stack || err.message || String(err);
      record(msg, 'ERR');
      scanFatal(msg);
    });
    page.on('requestfailed', (req) => {
      const err = req.failure();
      const msg = `Request failed: ${req.method()} ${req.url()} - ${err ? err.errorText : 'unknown error'}`;
      record(msg, 'ERR');
      scanFatal(msg);
    });
    page.on('crash', () => { record('page crashed', 'ERR'); sawFatal = true; });

    let timedOut = false;
    try {
      await page.goto(url, { waitUntil: 'load', timeout: cfg.bootTimeoutMs });
      // Wait for the app to boot: either it reports "Running..." or we observe frames+context.
      await page.waitForFunction(
        (min) => {
          const h = window.__harness || {};
          const status = (document.getElementById('status') || {}).innerHTML || '';
          return (h.contextType && h.frames >= min) || /Running/.test(status) || /Exception/.test(status);
        },
        cfg.minFrames,
        { timeout: cfg.bootTimeoutMs, polling: 250 },
      );
    } catch (e) {
      timedOut = true;
      record(`boot wait timed out: ${e.message}`, 'WARN');
    }

    // Run the render loop to accumulate frames and a stable image. Only a real
    // wasm trap (sawFatal) cuts this short. emscripten's window.onerror flips the
    // status banner to "Exception thrown" on the first page-level JS error (e.g. a
    // failed map fetch), which is a false positive for the smoke run — it must NOT
    // skip the run wait, or we never accumulate enough frames to pass.
    if (!sawFatal) {
      record(`Running render loop for ${cfg.runSeconds}s...`);
      await page.waitForTimeout(cfg.runSeconds * 1000);
    } else {
      record('Fatal wasm error detected during boot, skipping run wait.', 'WARN');
    }

    const h = await page.evaluate(() => window.__harness);
    const status = await page.evaluate(() => (document.getElementById('status') || {}).innerHTML || '');

    const shotPath = join(cfg.outDir, `${cfg.name}.jpg`);
    try {
      await page.locator('#canvas').screenshot({ path: shotPath, type: 'jpeg', quality: cfg.screenshotQuality });
      record(`screenshot written: ${shotPath}`);
    } catch (e) {
      record(`screenshot failed: ${e.message}`, 'ERR');
    }

    // Video is only flushed when the context closes; grab the handle first, then rename
    // it to a stable name in OUT_DIR so it rides along in the uploaded artifact.
    const video = page.video();
    await context.close();
    if (video) {
      try {
        const dest = join(cfg.outDir, `${cfg.name}.webm`);
        await rename(await video.path(), dest);
        record(`video written: ${dest}`);
      } catch (e) {
        record(`video save failed: ${e.message}`, 'ERR');
      }
    }

    // Verdict. STRICT=1 additionally fails on any page-level JS error (use once a map is
    // fed in and the app is expected to be error-clean).
    const strict = process.env.STRICT === '1';
    const checks = [];
    const ok = (label, pass) => { checks.push({ label, pass }); return pass; };
    ok('WebGL context created', !!h.contextType);
    ok('context is webgl2', h.contextType === 'webgl2');
    ok(`>= ${cfg.minFrames} frames rendered (got ${h.frames})`, h.frames >= cfg.minFrames);
    ok(`draw calls issued (got ${h.drawCalls})`, h.drawCalls > 0);
    ok('no wasm abort/trap', !sawFatal);
    ok('no WebGL context loss', !h.contextLost);
    ok('did not time out booting', !timedOut);
    if (strict) ok(`no page errors (got ${pageErrors})`, pageErrors === 0);

    record('');
    record('================ WebGL smoke result ================');
    record(`renderer : ${h.glRenderer}`);
    record(`gl version: ${h.glVersion}`);
    record(`status    : ${status}`);
    record(`frames=${h.frames} drawCalls=${h.drawCalls} contextLost=${h.contextLost} pageErrors=${pageErrors}`);
    const swiftshader = /swiftshader/i.test(String(h.glRenderer || ''));
    record(`software renderer (SwiftShader): ${swiftshader ? 'yes' : 'NO (check --use-angle=swiftshader)'}`);
    if (pageErrors > 0 && !strict) {
      record(`note: ${pageErrors} page error(s) seen (non-fatal; expected without a map fed in)`);
    }
    for (const c of checks) record(`  ${c.pass ? 'PASS' : 'FAIL'}  ${c.label}`);
    record('====================================================');

    const failed = checks.filter((c) => !c.pass);
    if (failed.length) {
      record(`${failed.length} check(s) failed.`, 'ERR');
      process.exit(1);
    }
    record('\nAll checks passed.');
  } catch (e) {
    record(e.stack || e.message || String(e), 'ERR');
    process.exit(1);
  } finally {
    if (browser) await browser.close().catch(() => {});
    if (server) server.close();
    try {
      await writeFile(join(cfg.outDir, 'output.log'), logLines.join('\n') + '\n');
    } catch (e) {
      console.error('Failed to write output.log:', e);
    }
  }
}

main();
