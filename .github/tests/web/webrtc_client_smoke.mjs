#!/usr/bin/env node
// Headless wasm-client-through-the-gateway smoke test.
//
// Boots the emscripten BlamGraphics bundle in headless Chromium, points it
// at a webrtc-gateway (tools/webrtc-gateway) via a ?server=ws://... URL
// query param (see examples/blam/cblam-testing/WEBRTC_TRANSPORT.md's
// Phase 3 section), and verifies the connection actually completes end to
// end against the GNS server on the other side of the gateway.
//
// This does NOT check rendering (WebGL context, frames, draw calls) --
// only whether the network handshake completes. It also can't use
// dummy_plug/Journal for verification the way the native tests do:
// comp_app::dummy_plug is compiled out entirely on emscripten, so there
// is no dump_state/state.json on this side. Verification is
// console-log-marker-based instead -- Networking's own cDebug() calls are
// unconditional, so scan the captured browser console for them.
//
// Orchestration (gateway + server) lives in the sibling shell script,
// which starts this once the server is registered and passes
// BUNDLE_DIR/SERVER_URL (plus HOST_URL for wasm<->wasm) via env.

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

  // Setting HOST_URL makes this a wasm<->wasm run: a second browser page
  // is booted first as a WebRTC-hosted server (--listen ws://gw#id, see
  // networking.cpp's create_server_webrtc) and the client is pointed at
  // it. Both halves are then DataChannel peers of the gateway, which
  // bridges them (transportWebRTC) -- no UDP anywhere in the path. Unset
  // means the original single-page run against a native server.
  hostUrl: process.env.HOST_URL || '',
  // Filename, extension included: graphics.cpp passes --map to MkUrl as-is.
  hostMap: process.env.HOST_MAP || 'bloodgulch.map',
  // The host has to boot AND get its registration accepted before the
  // client may dial it; a cold wasm boot on a CI runner is the slow part.
  hostReadyTimeoutMs: Number(process.env.HOST_READY_TIMEOUT_MS || 120000),
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

// COOP/COEP headers are required for SharedArrayBuffer/threaded wasm
// (this build always uses -pthread), independent of WebRTC.
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

// Markers this test cares about, in the order the connect flow produces
// them. A "connected" marker alone is not enough: GNS can reach Connected
// while the server-side dispatch is broken, so PASS requires the full
// round trip, ending in roster sync.
const MARKERS = [
  { label: 'WebRTC transport connected', re: /P2P WebRTC vport \d+\]\s*connected|-> k_ESteamNetworkingConnectionState_Connected\b/ },
  { label: 'GameJoin received (map load)', re: /Loading map .* as requested by server/ },
  { label: 'PlayerJoin sent, join confirmation received', re: /Received join confirmation, player_id=\d+/ },
  { label: 'Player roster received', re: /Player roster received:/ },
];

// Host-side markers, wasm<->wasm runs only. The first two gate the client
// (nothing may dial a server that has not been accepted by the gateway);
// 'accepted a client' is checked at the end, and is what separates a real
// wasm host from a page that merely registered and then did nothing.
const HOST_READY_MARKERS = [
  { label: 'host started (WebRTC-hosted server)', re: /Starting WebRTC-hosted server: gateway=/ },
  { label: 'host registration active', re: /registration active for serverId=/ },
];
const HOST_MARKERS = [
  ...HOST_READY_MARKERS,
  { label: 'host accepted a client', re: /Player joined:/ },
];

const logLines = [];
const record = (s, level = 'DEBG') => {
  const d = new Date();
  const ts = [d.getHours(), d.getMinutes(), d.getSeconds()].map((v) => String(v).padStart(2, '0')).join(':');
  const msg = `${level.padEnd(4)}:${ts}: ${s}`;
  logLines.push(msg);
  console.log(msg);
};

const waitFor = async (predicate, timeoutMs) => {
  const deadline = Date.now() + timeoutMs;
  while (Date.now() < deadline) {
    if (predicate()) return true;
    await new Promise((r) => setTimeout(r, 250));
  }
  return predicate();
};

// Boots one BlamGraphics page and wires its console into the shared log,
// tagged with the role so a wasm<->wasm run's two pages stay readable.
// Returns the marker set it fills in, plus its own fatal/timeout state --
// both roles are scanned identically, only their marker lists differ.
const CHROMIUM_ARGS = [
  '--use-gl=angle',
  '--use-angle=swiftshader',
  '--enable-unsafe-swiftshader',
  '--ignore-gpu-blocklist',
  '--enable-webgl',
  '--disable-dev-shm-usage',
  // Headless Chrome defaults to a UDP-mux-only/mDNS-obfuscated ICE policy
  // in some configs; force real host candidates so loopback ICE against
  // the gateway negotiates instead of gathering unusable mdns candidates.
  '--force-webrtc-ip-handling-policy=default',
  // Everything in this engine is driven from the frame loop, signaling
  // included, so a throttled page stops heartbeating and stops answering
  // the gateway. These keep a non-foreground page running at full rate.
  '--disable-background-timer-throttling',
  '--disable-backgrounding-occluded-windows',
  '--disable-renderer-backgrounding',
];

const browsers = [];
const closeBrowsers = async () => {
  await Promise.all(browsers.splice(0).map((b) => b.close().catch(() => {})));
};

// One browser per role. Two pages in ONE browser means one of them is a
// background tab, and Chrome pins those to 1fps -- which on CI stalled the
// host's signaling for minutes and expired its registration. Separate
// browsers give each page its own foreground tab.
async function launchRole({ url, role, markers }) {
  const state = { found: new Set(), sawFatal: false, timedOut: false };
  const browser = await chromium.launch({ headless: true, args: CHROMIUM_ARGS });
  browsers.push(browser);

  const scan = (text) => {
    if (FATAL_PATTERNS.some((re) => re.test(text))) state.sawFatal = true;
    for (const m of markers) {
      if (!state.found.has(m.label) && m.re.test(text)) {
        state.found.add(m.label);
        record(`[${role}] marker hit: ${m.label}`);
      }
    }
  };

  const page = await browser.newPage({ viewport: cfg.viewport });
  page.on('console', (msg) => {
    const text = msg.text();
    record(`[${role}] ${text}`, msg.type() === 'error' ? 'ERR' : msg.type() === 'warning' ? 'WARN' : 'DEBG');
    scan(text);
  });
  page.on('pageerror', (err) => {
    const msg = err.stack || err.message || String(err);
    record(`[${role}] ${msg}`, 'ERR');
    scan(msg);
  });
  page.on('crash', () => { record(`[${role}] page crashed`, 'ERR'); state.sawFatal = true; });

  try {
    await page.goto(url, { waitUntil: 'load', timeout: cfg.bootTimeoutMs });
  } catch (e) {
    state.timedOut = true;
    record(`[${role}] page load timed out: ${e.message}`, 'WARN');
  }
  return state;
}

async function main() {
  let server;
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


    // wasm<->wasm: the host page has to be registered and accepted by the
    // gateway before the client dials it, or the client's /signal gets
    // rejected outright with "unknown server".
    let host;
    if (cfg.hostUrl) {
      // --map, not a positional: the query-param shim only produces
      // "--key value" pairs, which is why graphics.cpp declares a map
      // option on platforms without a real command line.
      const hostPageUrl = `http://127.0.0.1:${port}/${cfg.page}`
        + `?listen=${encodeURIComponent(cfg.hostUrl)}&map=${encodeURIComponent(cfg.hostMap)}`;
      record(`Booting wasm host page at ${hostPageUrl}`);
      host = await launchRole({ url: hostPageUrl, role: 'host', markers: HOST_MARKERS });

      const hostReady = await waitFor(
        () => HOST_READY_MARKERS.every((m) => host.found.has(m.label)) || host.sawFatal,
        cfg.hostReadyTimeoutMs);
      if (!hostReady || host.sawFatal || host.timedOut) {
        throw new Error('wasm host never reached an active registration'
          + ` (fatal=${host.sawFatal} timedOut=${host.timedOut})`);
      }
      record('wasm host is registered and active; starting the client.');
    }

    const client = await launchRole({ url, role: 'client', markers: MARKERS });

    if (!client.timedOut && !client.sawFatal) {
      record(`Waiting up to ${cfg.connectTimeoutMs}ms for the full connect+roster-sync sequence...`);
      await waitFor(
        () => (client.found.size === MARKERS.length
               && (!host || host.found.size === HOST_MARKERS.length))
              || client.sawFatal || (host && host.sawFatal),
        cfg.connectTimeoutMs);
    }

    await closeBrowsers();

    const sawFatal = client.sawFatal || (host ? host.sawFatal : false);
    const timedOut = client.timedOut || (host ? host.timedOut : false);

    record('');
    record('================ WebRTC client smoke result ================');
    if (host) {
      for (const m of HOST_MARKERS) {
        record(`  ${host.found.has(m.label) ? 'PASS' : 'FAIL'}  [host]   ${m.label}`);
      }
    }
    for (const m of MARKERS) {
      record(`  ${client.found.has(m.label) ? 'PASS' : 'FAIL'}  [client] ${m.label}`);
    }
    record(`no wasm abort/trap: ${!sawFatal}`);
    record(`did not time out booting: ${!timedOut}`);
    record('===============================================================');

    const ok = !timedOut && !sawFatal
      && client.found.size === MARKERS.length
      && (!host || host.found.size === HOST_MARKERS.length);
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
    await closeBrowsers();
    if (server) server.close();
    try {
      await writeFile(join(cfg.outDir, 'output.log'), logLines.join('\n') + '\n');
    } catch (e) {
      console.error('Failed to write output.log:', e);
    }
  }
}

main();
