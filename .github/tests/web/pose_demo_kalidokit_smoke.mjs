#!/usr/bin/env node
// Playwright smoke test for BlamPoseDemo's webcam -> MediaPipe Pose ->
// Kalidokit -> dummy_plug bridge pipeline (examples/blam/cblam-pose-demo).
//
// Chrome's fake video device streams a real person (5940726-hd_1280_720_25fps.mp4,
// converted to Y4M) instead of the default synthetic test pattern — MediaPipe Pose
// needs an actual human figure to produce landmarks, so the default fake device
// (scrolling bars) would never exercise Kalidokit at all.
//
// PASS criteria: WebGL2 context created and drawing, no wasm abort, and at least
// one real (JS-side, per-frame-varying) pose payload observed in the console —
// proof the full webcam -> MediaPipe -> Kalidokit -> ccall chain is live, not
// just wired up on paper. Bone-level retargeting confirmation (the C++
// "apply_pose: retargeting ..." lines) is logged but not required to pass,
// since that also depends on the (large, separately-fetched) map finishing
// load — a slow map fetch shouldn't fail a test that's really about the JS
// pipeline.

import { chromium } from 'playwright';
import { createServer } from 'node:http';
import { readFile, mkdir, writeFile } from 'node:fs/promises';
import { existsSync } from 'node:fs';
import { join, normalize, extname } from 'node:path';

const cfg = {
  bundleDir: process.env.BUNDLE_DIR,
  page: process.env.PAGE || 'BlamPoseDemo.html',
  outDir: process.env.OUT_DIR || '/tmp/pose_demo_kalidokit_test',
  fakeVideo: process.env.FAKE_VIDEO || join(import.meta.dirname, 'pose_demo_fake_input.y4m'),
  bootTimeoutMs: Number(process.env.BOOT_TIMEOUT_MS || 60000),
  runSeconds: Number(process.env.RUN_SECONDS || 20),
  minFrames: Number(process.env.MIN_FRAMES || 5),
  viewport: { width: 1280, height: 720 },
};

const MIME = {
  '.html': 'text/html; charset=utf-8',
  '.js': 'text/javascript; charset=utf-8',
  '.wasm': 'application/wasm',
  '.data': 'application/octet-stream',
  '.map': 'application/octet-stream',
  '.tflite': 'application/octet-stream',
  '.binarypb': 'application/octet-stream',
  '.ico': 'image/x-icon',
};

function startServer(rootDir) {
  const root = normalize(rootDir);
  const server = createServer(async (req, res) => {
    try {
      const urlPath = decodeURIComponent((req.url || '/').split('?')[0]);
      const filePath = normalize(join(root, urlPath === '/' ? cfg.page : urlPath));
      if (!filePath.startsWith(root)) { res.writeHead(403).end('forbidden'); return; }
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

function instrument() {
  const h = (window.__harness = { contextType: null, glRenderer: null, frames: 0, drawCalls: 0, contextLost: false });
  window.alert = () => { h.contextLost = true; };
  const origGetContext = HTMLCanvasElement.prototype.getContext;
  HTMLCanvasElement.prototype.getContext = function (type, ...rest) {
    const ctx = origGetContext.call(this, type, ...rest);
    if (ctx && /webgl/i.test(String(type))) {
      h.contextType = String(type);
      try {
        const dbg = ctx.getExtension('WEBGL_debug_renderer_info');
        h.glRenderer = dbg ? ctx.getParameter(dbg.UNMASKED_RENDERER_WEBGL) : ctx.getParameter(ctx.RENDERER);
      } catch { /* ignore */ }
      this.addEventListener('webglcontextlost', () => { h.contextLost = true; }, false);
      for (const m of ['drawArrays', 'drawElements', 'drawArraysInstanced', 'drawElementsInstanced']) {
        const orig = ctx[m];
        if (typeof orig === 'function') ctx[m] = function (...a) { h.drawCalls++; return orig.apply(this, a); };
      }
    }
    return ctx;
  };
  const origRAF = window.requestAnimationFrame;
  window.requestAnimationFrame = function (cb) {
    return origRAF.call(window, (t) => { h.frames++; return cb(t); });
  };
  window.addEventListener('unhandledrejection', (ev) => {
    const r = ev.reason;
    console.error('[unhandledrejection] ' + ((r && (r.stack || r.message)) || String(r)));
  });
}

const FATAL_PATTERNS = [/abort\(/i, /\bAborted\(/, /RuntimeError/, /memory access out of bounds/i, /\bOOM\b/];

const logLines = [];
const record = (s, level = 'DEBG') => {
  const d = new Date();
  const ts = [d.getHours(), d.getMinutes(), d.getSeconds()].map((v) => String(v).padStart(2, '0')).join(':');
  const msg = `${level.padEnd(4)}:${ts}: ${s}`;
  logLines.push(msg);
  console.log(msg);
};

async function main() {
  let browser, server;
  try {
    if (!cfg.bundleDir || !existsSync(join(cfg.bundleDir, cfg.page))) {
      throw new Error(`bundle page not found: ${join(cfg.bundleDir || '<unset>', cfg.page)}`);
    }
    if (!existsSync(cfg.fakeVideo)) {
      throw new Error(
        `fake video not found: ${cfg.fakeVideo} — generate it with: ` +
        `ffmpeg -i 5940726-hd_1280_720_25fps.mp4 -vf scale=640:360 -pix_fmt yuv420p -r 25 pose_demo_fake_input.y4m`,
      );
    }
    await mkdir(cfg.outDir, { recursive: true });

    const { server: srv, port } = await startServer(cfg.bundleDir);
    server = srv;
    const url = `http://127.0.0.1:${port}/${cfg.page}?dummy_plug`;
    record(`Serving ${cfg.bundleDir} at ${url}`);
    record(`Fake video: ${cfg.fakeVideo}`);

    browser = await chromium.launch({
      headless: true,
      args: [
        '--use-gl=angle',
        '--use-angle=swiftshader',
        '--enable-unsafe-swiftshader',
        '--ignore-gpu-blocklist',
        '--enable-webgl',
        '--disable-dev-shm-usage',
        '--use-fake-device-for-media-stream',
        '--use-fake-ui-for-media-stream',
        `--use-file-for-fake-video-capture=${cfg.fakeVideo}`,
      ],
    });

    let sawFatal = false;
    const scanFatal = (text) => { if (FATAL_PATTERNS.some((re) => re.test(text))) sawFatal = true; };

    const context = await browser.newContext({ viewport: cfg.viewport });
    await context.grantPermissions(['camera', 'microphone'], { origin: `http://127.0.0.1:${port}` });
    const page = await context.newPage();
    await page.addInitScript(instrument);

    let posePayloadsSeen = 0;
    let retargetLinesSeen = 0;
    const seenPayloads = [];
    page.on('console', (msg) => {
      const text = msg.text();
      record(text, msg.type() === 'error' ? 'ERR' : msg.type() === 'warning' ? 'WARN' : 'DEBG');
      scanFatal(text);
      if (text.startsWith('[pose_demo] sending')) {
        posePayloadsSeen++;
        if (seenPayloads.length < 5) seenPayloads.push(text);
      }
      if (text.includes('apply_pose: retargeting')) retargetLinesSeen++;
    });
    page.on('pageerror', (err) => {
      const msg = err.stack || err.message || String(err);
      record(msg, 'ERR');
      scanFatal(msg);
    });
    page.on('crash', () => { record('page crashed', 'ERR'); sawFatal = true; });
    page.on('requestfailed', (req) => {
      if (!/vendor\/pose|vendor\/kalidokit/.test(req.url())) return;
      const err = req.failure();
      record(`vendor request FAILED: ${req.method()} ${req.url()} - ${err ? err.errorText : 'unknown'}`, 'ERR');
    });
    page.on('response', (res) => {
      if (!/vendor\/pose|vendor\/kalidokit/.test(res.url())) return;
      record(`vendor request: ${res.status()} ${res.url()}`);
    });

    let timedOut = false;
    try {
      await page.goto(url, { waitUntil: 'load', timeout: cfg.bootTimeoutMs });
      await page.waitForFunction(
        (min) => {
          const h = window.__harness || {};
          return h.contextType && h.frames >= min;
        },
        cfg.minFrames,
        { timeout: cfg.bootTimeoutMs, polling: 250 },
      );
    } catch (e) {
      timedOut = true;
      record(`boot wait timed out: ${e.message}`, 'WARN');
    }

    if (!sawFatal) {
      record(`Running for ${cfg.runSeconds}s to let webcam/MediaPipe/Kalidokit spin up and stream...`);
      await page.waitForTimeout(cfg.runSeconds * 1000);
    }

    const h = await page.evaluate(() => window.__harness);

    const shotPath = join(cfg.outDir, 'pose_demo_kalidokit_smoke.jpg');
    try {
      await page.screenshot({ path: shotPath, type: 'jpeg', quality: 80 });
      record(`screenshot written: ${shotPath}`);
    } catch (e) {
      record(`screenshot failed: ${e.message}`, 'ERR');
    }

    await context.close();

    const checks = [];
    const ok = (label, pass) => { checks.push({ label, pass }); return pass; };
    ok('WebGL context created', !!h.contextType);
    ok(`>= ${cfg.minFrames} frames rendered (got ${h.frames})`, h.frames >= cfg.minFrames);
    ok('draw calls issued', h.drawCalls > 0);
    ok('no wasm abort/trap', !sawFatal);
    ok('no WebGL context loss', !h.contextLost);
    ok('did not time out booting', !timedOut);
    ok(`real pose payload(s) observed from JS (got ${posePayloadsSeen})`, posePayloadsSeen > 0);

    record('');
    record('========== Pose Demo Kalidokit smoke result ==========');
    record(`renderer: ${h.glRenderer}`);
    record(`frames=${h.frames} drawCalls=${h.drawCalls}`);
    record(`JS pose payloads observed: ${posePayloadsSeen}`);
    record(`C++ "apply_pose: retargeting" lines observed: ${retargetLinesSeen} (0 is OK — means map/biped wasn't ready yet, not that the JS pipeline failed)`);
    for (const p of seenPayloads) record(`  sample: ${p}`);
    for (const c of checks) record(`  ${c.pass ? 'PASS' : 'FAIL'}  ${c.label}`);
    record('========================================================');

    const failed = checks.filter((c) => !c.pass);
    if (failed.length) {
      record(`${failed.length} check(s) failed.`, 'ERR');
      process.exit(1);
    }
    record('All checks passed.');
  } catch (e) {
    record(e.stack || e.message || String(e), 'ERR');
    process.exit(1);
  } finally {
    if (browser) await browser.close().catch(() => {});
    if (server) server.close();
    try { await writeFile(join(cfg.outDir, 'output.log'), logLines.join('\n') + '\n'); } catch {}
  }
}

main();
