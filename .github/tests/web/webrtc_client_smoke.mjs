#!/usr/bin/env node
// Headless wasm-client-through-the-gateway smoke test.
//
// Boots the emscripten BlamGraphics bundle in headless Chromium, points it
// at a webrtc-gateway (tools/webrtc-gateway) via a ?server=ws://... URL
// query param (see examples/blam/cblam-testing/WEBRTC_TRANSPORT.md's
// Phase 3 section), and verifies the connection actually completes end to
// end against a native GNS server on the other side of the gateway --
// the wasm-client / gateway / native-server scenario this whole effort
// has been aimed at.
//
// Unlike webgl_smoke.mjs, this does NOT check rendering (WebGL context,
// frames, draw calls) -- it only cares whether the network handshake
// completes. It also can't use dummy_plug/Journal for verification the
// way the native-to-native test (.github/tests/net/run_webrtc_client_server_test.sh)
// does: comp_app::dummy_plug is compiled out entirely on emscripten (see
// .github/tests/web/README.md's "Tier 2" section) -- there's no
// dump_state/state.json on this side. Verification here is
// console-log-marker-based instead: Networking's own cDebug() calls
// (unconditional, not gated behind dummy_plug/journal) already print the
// same lines this session used to manually verify the native test by
// hand -- "Player joined", "Received join confirmation", "roster
// received" -- so scan the captured browser console output for those.
//
// Orchestration (gateways + native server) lives in the sibling shell
// script run_webrtc_web_client_test.sh, which starts this after the
// server is registered and passes BUNDLE_DIR/SERVER_URL via env.

import { chromium } from 'playwright';
import { createServer } from 'node:http';
import { readFile, mkdir, writeFile } from 'node:fs/promises';
import { existsSync } from 'node:fs';
import { join, normalize, extname } from 'node:path';

const cfg = {
  bundleDir: process.env.BUNDLE_DIR,
  page: process.env.PAGE || 'BlamGraphics.html',
  outDir: process.env.OUT_DIR || '/tmp/webrtc_client_smoke',
  serverUrl: process.env.SERVER_URL, // e.g. ws://127.0.0.1:8098
  bootTimeoutMs: Number(process.env.BOOT_TIMEOUT_MS || 45000),
  // How long to wait, after boot, for the connection to actually
  // complete (SDP/ICE + GNS rendezvous handshake all take real
  // wall-clock time -- see WEBRTC_TRANSPORT.md).
  connectTimeoutMs: Number(process.env.CONNECT_TIMEOUT_MS || 60000),
  viewport: { width: 640, height: 480 },
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

// Same COOP/COEP-header static server as webgl_smoke.mjs -- needed for
// SharedArrayBuffer/threaded wasm (this build always uses -pthread, see
// WEBRTC_TRANSPORT.md's Phase 3 threading note), independent of WebRTC.
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

const FATAL_PATTERNS = [
  /abort\(/i, /\bAborted\(/, /RuntimeError/, /Cannot enlarge memory/,
  /memory access out of bounds/i, /\bOOM\b/, /maximum call stack/i,
];

// Markers this test actually cares about, in the order the connect flow
// produces them -- see networking.cpp's cDebug() calls at each step.
// Matching any one of the "connected" markers isn't enough on its own
// (GNS can reach Connected without the server-side dispatch fix ever
// running -- that was the actual bug this session spent the most time
// on); PASS requires the full round trip, ending in roster sync.
const MARKERS = [
  { label: 'WebRTC transport connected', re: /P2P WebRTC vport \d+\]\s*connected|-> k_ESteamNetworkingConnectionState_Connected\b/ },
  { label: 'GameJoin received (map load)', re: /Loading map .* as requested by server/ },
  { label: 'PlayerJoin sent, join confirmation received', re: /Received join confirmation, player_id=\d+/ },
  { label: 'Player roster received', re: /Player roster received:/ },
];

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
    if (!cfg.serverUrl) {
      throw new Error('SERVER_URL is required (e.g. ws://127.0.0.1:8098)');
    }
    await mkdir(cfg.outDir, { recursive: true });

    const { server: srv, port } = await startServer(cfg.bundleDir);
    server = srv;

    // ?server=ws://... becomes argv ["--server", "ws://..."] via
    // Coffee::CoffeeInit's query-param shim (src/coffee/application/private/coffee.cpp)
    // -- identical to the native --server CLI flag from this same file's
    // examples/blam/cblam-testing/graphics.cpp cxxopts parsing.
    const url = `http://127.0.0.1:${port}/${cfg.page}?server=${encodeURIComponent(cfg.serverUrl)}`;
    record(`Serving ${cfg.bundleDir} at ${url}`);

    browser = await chromium.launch({
      headless: true,
      args: [
        '--use-gl=angle',
        '--use-angle=swiftshader',
        '--enable-unsafe-swiftshader',
        '--ignore-gpu-blocklist',
        '--enable-webgl',
        '--disable-dev-shm-usage',
        // Headless Chrome defaults to a UDP-mux-only/mDNS-obfuscated ICE
        // policy in some configs; force real host candidates so loopback
        // ICE against the gateway actually negotiates instead of only
        // gathering unusable mdns candidates (see
        // tools/webrtc-gateway/testing/run_phase1_test.mjs, the earlier
        // Phase 1 validation of this same flag).
        '--force-webrtc-ip-handling-policy=default',
      ],
    });

    let sawFatal = false;
    const scanFatal = (text) => {
      if (FATAL_PATTERNS.some((re) => re.test(text))) sawFatal = true;
    };

    const foundMarkers = new Set();
    const scanMarkers = (text) => {
      for (const m of MARKERS) {
        if (!foundMarkers.has(m.label) && m.re.test(text)) {
          foundMarkers.add(m.label);
          record(`marker hit: ${m.label}`);
        }
      }
    };

    const page = await browser.newPage({ viewport: cfg.viewport });
    page.on('console', (msg) => {
      const text = msg.text();
      record(text, msg.type() === 'error' ? 'ERR' : msg.type() === 'warning' ? 'WARN' : 'DEBG');
      scanFatal(text);
      scanMarkers(text);
    });
    page.on('pageerror', (err) => {
      const msg = err.stack || err.message || String(err);
      record(msg, 'ERR');
      scanFatal(msg);
    });
    page.on('crash', () => { record('page crashed', 'ERR'); sawFatal = true; });

    let timedOut = false;
    try {
      await page.goto(url, { waitUntil: 'load', timeout: cfg.bootTimeoutMs });
    } catch (e) {
      timedOut = true;
      record(`page load timed out: ${e.message}`, 'WARN');
    }

    if (!timedOut && !sawFatal) {
      record(`Waiting up to ${cfg.connectTimeoutMs}ms for the full connect+roster-sync sequence...`);
      const deadline = Date.now() + cfg.connectTimeoutMs;
      while (Date.now() < deadline && foundMarkers.size < MARKERS.length && !sawFatal) {
        await new Promise((r) => setTimeout(r, 250));
      }
    }

    await browser.close().catch(() => {});
    browser = undefined;

    record('');
    record('================ WebRTC client smoke result ================');
    for (const m of MARKERS) {
      record(`  ${foundMarkers.has(m.label) ? 'PASS' : 'FAIL'}  ${m.label}`);
    }
    record(`no wasm abort/trap: ${!sawFatal}`);
    record(`did not time out booting: ${!timedOut}`);
    record('===============================================================');

    const ok = !timedOut && !sawFatal && foundMarkers.size === MARKERS.length;
    if (!ok) {
      record('FAILED.', 'ERR');
      process.exitCode = 1;
    } else {
      record('All checks passed.');
    }
  } catch (e) {
    record(e.stack || e.message || String(e), 'ERR');
    process.exitCode = 1;
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
