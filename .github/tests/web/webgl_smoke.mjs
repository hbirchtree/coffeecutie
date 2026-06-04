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
import { readFile, mkdir, writeFile } from 'node:fs/promises';
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
  viewport: { width: 1280, height: 720 },
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

async function main() {
  if (!cfg.bundleDir || !existsSync(join(cfg.bundleDir, cfg.page))) {
    console.error(`ERROR: bundle page not found: ${join(cfg.bundleDir || '<unset>', cfg.page)}`);
    console.error('Set BUNDLE_DIR to the directory containing', cfg.page);
    process.exit(2);
  }
  await mkdir(cfg.outDir, { recursive: true });

  const { server, port } = await startServer(cfg.bundleDir);
  const url = `http://127.0.0.1:${port}/${cfg.page}`;
  console.log(`Serving ${cfg.bundleDir} at ${url}`);

  const browser = await chromium.launch({
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

  const logLines = [];
  const record = (s) => { logLines.push(s); console.log(s); };
  let sawFatal = false;
  let pageErrors = 0;
  const scanFatal = (text) => {
    if (FATAL_PATTERNS.some((re) => re.test(text))) sawFatal = true;
  };

  const page = await browser.newPage({ viewport: cfg.viewport });
  await page.addInitScript(instrument);
  if (cfg.dummyPlug && existsSync(cfg.dummyPlug)) {
    const configText = await readFile(cfg.dummyPlug, 'utf8');
    await page.addInitScript(installDummyPlug, configText);
    record(`dummy_plug config staged: ${cfg.dummyPlug} (Tier 2 — engine support pending)`);
  }

  page.on('console', (msg) => { const t = `[${msg.type()}] ${msg.text()}`; record(t); scanFatal(t); });
  // Page-level JS errors: counted and logged, but only fatal if they're a wasm trap.
  // emscripten's window.onerror flips the status banner to "Exception thrown" on the
  // first of these, which is a false positive for a no-map smoke run.
  page.on('pageerror', (err) => { pageErrors++; const t = `[pageerror] ${err.message}`; record(t); scanFatal(t); });
  page.on('crash', () => { record('[crash] page crashed'); sawFatal = true; });

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
    record(`[harness] boot wait timed out: ${e.message}`);
  }

  // Let the render loop run for a while so we accumulate frames and a stable image.
  await page.waitForTimeout(cfg.runSeconds * 1000);

  const h = await page.evaluate(() => window.__harness);
  const status = await page.evaluate(() => (document.getElementById('status') || {}).innerHTML || '');

  const shotPath = join(cfg.outDir, `${cfg.name}.jpg`);
  try {
    await page.locator('#canvas').screenshot({ path: shotPath, type: 'jpeg', quality: cfg.screenshotQuality });
    record(`[harness] screenshot written: ${shotPath}`);
  } catch (e) {
    record(`[harness] screenshot failed: ${e.message}`);
  }

  await browser.close();
  server.close();

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

  await writeFile(join(cfg.outDir, 'output.log'), logLines.join('\n') + '\n');

  const failed = checks.filter((c) => !c.pass);
  if (failed.length) {
    console.error(`\n${failed.length} check(s) failed.`);
    process.exit(1);
  }
  console.log('\nAll checks passed.');
}

main().catch((e) => { console.error(e); process.exit(1); });
