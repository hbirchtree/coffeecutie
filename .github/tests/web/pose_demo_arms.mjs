#!/usr/bin/env node
// Drives BlamPoseDemo's arm retargeting with synthetic poses instead of a webcam.
//
// pose_demo_kalidokit_smoke.mjs proves the webcam -> MediaPipe -> Kalidokit ->
// ccall chain is live. It cannot say whether a raised arm produces a raised
// arm: the input is a video of a person, and the only readout is how the frame
// looks. This one injects exact quaternions into coffee_dummy_plug_event and
// reads the resulting bone positions back out of the engine with dump_pose, so
// each case is a known input checked against a measured output. A mirrored axis
// shows up as a sign, not as an impression of a screenshot.
//
// Two things are pinned to make the measurement mean anything: the idle
// animation is frozen (see FROZEN_FPS) so a bone only moves when a pose moves
// it, and smoothing is switched off so one event lands in full.
//
// The camera is denied on purpose — with no webcam the shell's pose loop never
// runs, so nothing competes with the injected poses.
//
// PASS criteria: the app boots and draws, apply_pose reports no missing bone,
// and every case moves the hand it should, in the direction it should, without
// moving the other arm.
//
// Run under xvfb with a real GL backend; SwiftShader loses the context on this
// bundle. See run_pose_demo_arms.sh.

import { chromium } from 'playwright';
import { createServer } from 'node:http';
import { readFile, mkdir, writeFile } from 'node:fs/promises';
import { existsSync } from 'node:fs';
import { join, normalize, extname } from 'node:path';

const cfg = {
  bundleDir: process.env.BUNDLE_DIR,
  page: process.env.PAGE || 'BlamPoseDemo.html',
  outDir: process.env.OUT_DIR || '/tmp/pose_demo_arms',
  bootTimeoutMs: Number(process.env.BOOT_TIMEOUT_MS || 180000),
  settleSeconds: Number(process.env.SETTLE_SECONDS || 20),
  caseSeconds: Number(process.env.CASE_SECONDS || 2),
  viewport: {
    width: Number(process.env.VIEW_W || 900),
    height: Number(process.env.VIEW_H || 700),
  },
};

const MIME = {
  '.html': 'text/html; charset=utf-8',
  '.js': 'text/javascript; charset=utf-8',
  '.json': 'application/json; charset=utf-8',
  '.wasm': 'application/wasm',
  '.data': 'application/octet-stream',
  '.map': 'application/octet-stream',
  '.tflite': 'application/octet-stream',
  '.binarypb': 'application/octet-stream',
  '.ico': 'image/x-icon',
};

/* apply_pose picks its frame as (elapsed * fps) % frame_count, and
 * pose_config rejects fps <= 0, so the smallest positive rate is how the idle
 * animation gets held on frame 0 for the length of a run. */
const FROZEN_FPS = 0.0001;

const L_HAND = 'bip01 l hand';
const R_HAND = 'bip01 r hand';

/* The arm entries run in "aim" mode, so they are driven by limb directions
 * taken from MediaPipe's world landmarks, not by Kalidokit's rotations. Cases
 * are written as directions for that reason: a rotation-driven case would move
 * nothing and pass for the wrong reason.
 *
 * MediaPipe world space: +x image-right, +y DOWN, +z away from camera. The
 * subject faces the camera, so their anatomical left is at +x.
 *
 * Kalidokit's Left is solved from landmarks 12/14/16 — the subject's
 * anatomical RIGHT — and drives bip01 l. So `LeftUpperArm` below carries the
 * subject's right-arm direction. Keep that in mind reading the cases: it is
 * the convention, not a mistake. */
const SUBJ_RIGHT = [-1, 0, 0];   // subject's right arm points image-left
const SUBJ_LEFT = [1, 0, 0];
const DOWN = [0, 1, 0];
const UP = [0, -1, 0];
const FWD = [0, 0, -1];          // toward the camera
const UP_FWD = [0, -0.707, -0.707];   // 45 degrees up, toward the camera

const arms = (lu, ll, ru, rl) => ({
  LeftUpperArm: lu, LeftLowerArm: ll, RightUpperArm: ru, RightLowerArm: rl,
});

/* blam model space is X forward, Y left, Z up. bip01 l sits at +Y and bip01 r
 * at -Y, so "did the arm go to its own side" is a question about the sign of
 * the hand's Y — which is what catches an inverted lateral axis. A clip where
 * the arms stay near the midline cannot catch it at all, which is how it got
 * through the first time. */
const CASES = [
  {
    name: 't_pose',
    dirs: arms(SUBJ_RIGHT, SUBJ_RIGHT, SUBJ_LEFT, SUBJ_LEFT),
    /* Arms straight out: each hand must be well clear on its OWN side. */
    expect: (p) => ({
      pass: p[L_HAND][1] > 0.15 && p[R_HAND][1] < -0.15,
      detail: `l_hand y=${p[L_HAND][1].toFixed(3)} (want > +0.15), ` +
              `r_hand y=${p[R_HAND][1].toFixed(3)} (want < -0.15)`,
    }),
  },
  {
    name: 'arms_crossed',
    dirs: arms(SUBJ_LEFT, SUBJ_LEFT, SUBJ_RIGHT, SUBJ_RIGHT),
    /* The exact inverse of t_pose. Both must hold, or a flipped lateral axis
     * simply swaps which of the two passes. */
    expect: (p) => ({
      pass: p[L_HAND][1] < 0 && p[R_HAND][1] > 0,
      detail: `l_hand y=${p[L_HAND][1].toFixed(3)} (want < 0), ` +
              `r_hand y=${p[R_HAND][1].toFixed(3)} (want > 0)`,
    }),
  },
  {
    name: 'arms_down',
    dirs: arms(DOWN, DOWN, DOWN, DOWN),
    expect: (p) => ({
      pass: p[L_HAND][2] < 0.42 && Math.abs(p[L_HAND][1]) < 0.16,
      detail: `l_hand z=${p[L_HAND][2].toFixed(3)} (want < 0.42), ` +
              `|y|=${Math.abs(p[L_HAND][1]).toFixed(3)} (want < 0.16)`,
    }),
  },
  {
    name: 'arms_up',
    dirs: arms(UP, UP, UP, UP),
    expect: (p) => ({
      pass: p[L_HAND][2] > 0.7 && p[R_HAND][2] > 0.7,
      detail: `l_hand z=${p[L_HAND][2].toFixed(3)}, ` +
              `r_hand z=${p[R_HAND][2].toFixed(3)} (want both > 0.7)`,
    }),
  },
  {
    name: 'arms_forward',
    dirs: arms(FWD, FWD, FWD, FWD),
    expect: (p) => ({
      pass: p[L_HAND][0] > 0.15 && p[R_HAND][0] > 0.15,
      detail: `l_hand x=${p[L_HAND][0].toFixed(3)}, ` +
              `r_hand x=${p[R_HAND][0].toFixed(3)} (want both > 0.15)`,
    }),
  },
  {
    name: 'elbow_bend',
    /* Upper arms hanging, forearms brought up and forward — roughly the
     * posture the seated test clip holds. Deliberately tilted rather than
     * level: a forearm aimed straight forward off a hanging upper arm is
     * exactly horizontal, so "is the hand above the elbow" would then be
     * decided by rounding. */
    dirs: arms(DOWN, UP_FWD, DOWN, UP_FWD),
    expect: (p, rest, elbows) => ({
      pass: p[L_HAND][2] > elbows.l[2] + 0.03 && p[L_HAND][0] > elbows.l[0] + 0.03,
      detail: `l_hand z=${p[L_HAND][2].toFixed(3)} vs elbow ` +
              `${elbows.l[2].toFixed(3)} (want > +0.03), x=${p[L_HAND][0].toFixed(3)} ` +
              `vs ${elbows.l[0].toFixed(3)} (want > +0.03)`,
    }),
  },
];

const L_FOREARM = 'bip01 l forearm';
const R_FOREARM = 'bip01 r forearm';

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
}

const FATAL_PATTERNS = [/abort\(/i, /\bAborted\(/, /RuntimeError/, /memory access out of bounds/i];

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
    if (!cfg.bundleDir || !existsSync(join(cfg.bundleDir, cfg.page)))
      throw new Error(`bundle page not found: ${join(cfg.bundleDir || '<unset>', cfg.page)}`);
    await mkdir(cfg.outDir, { recursive: true });

    const { server: srv, port } = await startServer(cfg.bundleDir);
    server = srv;

    /* Served through page.route rather than written into the bundle, so the
     * test owns its settings and leaves the shipped configs alone. */
    const testConfig = JSON.parse(await readFile(join(cfg.bundleDir, 'configs/cyborg.json'), 'utf8'));
    /* alpha 1 disables the EMA, so one injected pose lands in full. */
    testConfig.smoothing = { alpha: 1.0 };
    /* Both move the model on their own and would show up as bone motion. */
    testConfig.microphone = { enabled: false };
    testConfig.root_motion = { enabled: false };
    testConfig.animations = [{ biped: '*', fps: FROZEN_FPS, buttons: [] }];
    if (process.env.CAMERA_POS) testConfig.camera.position = JSON.parse(process.env.CAMERA_POS);

    const url = `http://127.0.0.1:${port}/${cfg.page}?dummy_plug&config=pose_arms_test.json`;
    record(`Serving ${cfg.bundleDir} at ${url}`);

    browser = await chromium.launch({
      headless: false,
      args: [
        '--use-gl=angle',
        '--use-angle=gl',
        '--ignore-gpu-blocklist',
        '--enable-webgl',
        '--disable-dev-shm-usage',
      ],
    });

    let sawFatal = false;
    const missingBones = new Set();
    let bonePositions = null;   // set while a dump_pose is being collected

    const context = await browser.newContext({ viewport: cfg.viewport });
    await context.clearPermissions();
    const page = await context.newPage();
    await page.addInitScript(instrument);
    await page.route('**/pose_arms_test.json', (route) =>
      route.fulfill({ status: 200, contentType: 'application/json', body: JSON.stringify(testConfig) }));

    page.on('console', (msg) => {
      const text = msg.text();
      if (FATAL_PATTERNS.some((re) => re.test(text))) sawFatal = true;
      const missing = text.match(/apply_pose: bone '([^']+)' not found/);
      if (missing) missingBones.add(missing[1]);
      /* "bone_pos <name> <x> <y> <z>" — bone names contain spaces, so the
       * numbers are taken from the end and the name is whatever is left. */
      const pos = text.match(/bone_pos (.+) (-?[\d.e+-]+) (-?[\d.e+-]+) (-?[\d.e+-]+)\s*$/);
      if (pos && bonePositions)
        bonePositions.set(pos[1].trim(), [Number(pos[2]), Number(pos[3]), Number(pos[4])]);
      else if (/apply_pose|pose_demo: (?!bone_pos)|spawn/i.test(text))
        record(text, msg.type() === 'error' ? 'ERR' : 'DEBG');
    });
    page.on('pageerror', (err) => {
      const msg = err.stack || err.message || String(err);
      record(msg, 'ERR');
      if (FATAL_PATTERNS.some((re) => re.test(msg))) sawFatal = true;
    });
    page.on('crash', () => { record('page crashed', 'ERR'); sawFatal = true; });

    await page.goto(url, { waitUntil: 'load', timeout: cfg.bootTimeoutMs });
    await page.waitForFunction(
      () => {
        const h = window.__harness || {};
        if (!h.contextType || h.frames < 5) return false;
        try { return window.BlamModule.ccall('coffee_app_loaded', 'number', [], []) === 1; }
        catch { return false; }
      },
      undefined,
      { timeout: cfg.bootTimeoutMs, polling: 500 },
    );
    record(`app loaded; settling ${cfg.settleSeconds}s for the biped to spawn`);
    await page.waitForTimeout(cfg.settleSeconds * 1000);

    const send = (payload) => page.evaluate((p) => {
      window.BlamModule.ccall('coffee_dummy_plug_event', null, ['string'], [JSON.stringify(p)]);
    }, payload);

    /* Sends the pose, screenshots it, then asks the engine where the bones
     * ended up. Every arm bone is sent every time, so none is left holding the
     * previous case's direction. */
    async function runCase(name, dirs) {
      const payload = ['LeftUpperArm', 'RightUpperArm', 'LeftLowerArm', 'RightLowerArm'].map((b) => ({
        name: b,
        direction: dirs[b],
      }));
      await send({ type: 'pose_apply', bones: payload });
      await page.waitForTimeout(cfg.caseSeconds * 1000);
      await page.screenshot({ path: join(cfg.outDir, `${name}.png`), type: 'png' });

      bonePositions = new Map();
      await send({ type: 'dump_pose' });
      await page.waitForTimeout(500);
      const out = bonePositions;
      bonePositions = null;
      return out;
    }

    const checks = [];
    const ok = (label, pass) => { checks.push({ label, pass }); return pass; };

    /* A neutral pose to read the resting geometry off, and to prove dump_pose
     * is answering at all before any case is judged by it. */
    const rest = await runCase('rest', {
      LeftUpperArm: DOWN, LeftLowerArm: DOWN, RightUpperArm: DOWN, RightLowerArm: DOWN,
    });
    if (!rest.has(L_HAND) || !rest.has(R_HAND))
      throw new Error(`dump_pose returned no hand bones (got ${rest.size} bones)`);
    record(`rest ${L_HAND} = [${rest.get(L_HAND).map((v) => v.toFixed(4))}]`);
    record(`rest ${R_HAND} = [${rest.get(R_HAND).map((v) => v.toFixed(4))}]`);

    for (const c of CASES) {
      const pos = await runCase(c.name, c.dirs);
      const p = Object.fromEntries([...pos.entries()]);
      const elbows = { l: pos.get(L_FOREARM), r: pos.get(R_FOREARM) };
      const r = c.expect(p, rest, elbows);
      record(`case ${c.name.padEnd(13)} ${r.detail}`);
      ok(`${c.name}: ${r.detail}`, r.pass);
    }

    const h = await page.evaluate(() => window.__harness);
    await context.close();

    checks.unshift(
      { label: 'WebGL context created', pass: !!h.contextType },
      { label: 'draw calls issued', pass: h.drawCalls > 0 },
      { label: 'no wasm abort/trap', pass: !sawFatal },
      { label: 'no WebGL context loss', pass: !h.contextLost },
      { label: `no missing bones (${[...missingBones].join(', ') || 'none'})`, pass: missingBones.size === 0 },
    );

    record('');
    record('========== Pose Demo arm retarget result ==========');
    record(`renderer: ${h.glRenderer}`);
    record(`frames=${h.frames} drawCalls=${h.drawCalls}`);
    for (const c of checks) record(`  ${c.pass ? 'PASS' : 'FAIL'}  ${c.label}`);
    record('===================================================');

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
