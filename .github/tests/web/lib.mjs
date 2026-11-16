// Shared plumbing for the Playwright drivers. run.py owns the configuration,
// the static server and the verdict; a driver only drives the page and
// writes down what it saw (result.json).

import { chromium } from 'playwright';
import { readFileSync, writeFileSync, renameSync } from 'node:fs';
import { join } from 'node:path';

export const PAGE_SCRIPTS = join(import.meta.dirname, 'page');

export function loadConfig() {
  return JSON.parse(readFileSync(process.argv[2], 'utf8'));
}

export function writeResult(cfg, result) {
  writeFileSync(join(cfg.out_dir, 'result.json'), JSON.stringify(result, null, 1));
}

// Emscripten traces repeat, carry full URLs and half their frames are the
// logging machinery; this keeps the function names and a short locator.
let lastStackSig = null;
const STACK_PLUMBING =
  /\b(jsStackTrace|getCallstack|__emscripten_log_formatted|emscripten_log|Coffee::Logging::(?:detail::)?log)\b/;
function prettyStack(text) {
  if (typeof text !== 'string' || !/wasm-function|\.(?:wasm|js):\d/.test(text)) return text;
  const head = [];
  const frames = [];
  for (const raw of text.split('\n')) {
    let line = raw
      .replace(/https?:\/\/[^/]+\//g, '')
      .replace(/\w+\.wasm:wasm-function\[(\d+)\]:0x[0-9a-f]+/g, 'wasm[$1]')
      .replace(/std::__2::/g, 'std::')
      .replace(/\[abi:[^\]]+\]/g, '')
      .replace(/\s+/g, ' ')
      .trimEnd();
    if (/^\s*at\s/.test(raw)) {
      if (STACK_PLUMBING.test(line)) continue;
      const m = line.match(/^(at .*?)\s*\((wasm\[\d+\]|[^)]*)\)\s*$/);
      if (m && m[1].length > 140) line = `${m[1].slice(0, 137)}...) (${m[2]})`;
      frames.push('    ' + line);
    } else if (line) {
      head.push(line);
    }
  }
  if (!frames.length) return head.join('\n');
  const sig = frames.join('|');
  if (sig === lastStackSig) return [...head, '    <stack identical to previous>'].join('\n');
  lastStackSig = sig;
  return [...head, ...frames].join('\n');
}

export function log(text, level = 'DEBG') {
  const ts = new Date().toTimeString().slice(0, 8);
  console.log(`${level.padEnd(4)}:${ts}: ${prettyStack(text)}`);
}

export const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

export async function waitFor(predicate, timeoutMs, pollMs = 250) {
  const deadline = Date.now() + timeoutMs;
  while (Date.now() < deadline) {
    if (predicate()) return true;
    await sleep(pollMs);
  }
  return predicate();
}

const browsers = [];

// Returns a context; a persistent profile keeps OPFS/IndexedDB between runs.
export async function newContext(browserCfg, contextOpts) {
  const launch = { headless: browserCfg.headless, args: browserCfg.args };
  if (browserCfg.profile_dir) {
    const ctx = await chromium.launchPersistentContext(browserCfg.profile_dir, { ...launch, ...contextOpts });
    browsers.push(ctx);
    return ctx;
  }
  const browser = await chromium.launch(launch);
  browsers.push(browser);
  return browser.newContext(contextOpts);
}

export async function closeAll() {
  await Promise.all(browsers.splice(0).map((b) => b.close().catch(() => {})));
}

const compile = (list) => (list || []).map((m) => ({ label: m.label, re: new RegExp(m.re) }));

// Opens a page with the init scripts and console wiring every driver needs.
// The returned state fills in as the page logs: fatal traps, page errors,
// marker hits, counter totals and frame-loop liveness from the FPS line.
export async function openPage(context, cfg, { role = '', markers = [], counters = [], onConsole } = {}) {
  const tag = role ? `[${role}] ` : '';
  const fatal = (cfg.fatal_patterns || []).map((re) => new RegExp(re));
  const marks = compile(markers);
  const counts = compile(counters);
  const state = {
    fatal: false, pageErrors: 0, found: [], counts: {}, samples: {},
    lastFrameAt: 0, framesSinceLoad: 0, mapLoaded: false, worstStallMs: 0,
  };
  for (const c of counts) { state.counts[c.label] = 0; state.samples[c.label] = []; }

  const scan = (text) => {
    if (fatal.some((re) => re.test(text))) state.fatal = true;
    if (/\bFPS:\s*\d+/.test(text)) {
      const now = Date.now();
      const gap = state.lastFrameAt ? now - state.lastFrameAt : 0;
      state.worstStallMs = Math.max(state.worstStallMs, gap);
      if (cfg.stall_warn_ms && gap >= cfg.stall_warn_ms) {
        log(`${tag}frame-loop stall: ${(gap / 1000).toFixed(1)}s with no frame`, 'WARN');
      }
      state.lastFrameAt = now;
      if (state.mapLoaded) state.framesSinceLoad++;
    }
    if (!state.mapLoaded && /Load finish signalled/.test(text)) state.mapLoaded = true;
    for (const m of marks) {
      if (!state.found.includes(m.label) && m.re.test(text)) {
        state.found.push(m.label);
        log(`${tag}marker hit: ${m.label}`);
      }
    }
    for (const c of counts) {
      if (c.re.test(text) && state.counts[c.label]++ < 5) state.samples[c.label].push(text);
    }
  };

  const page = await context.newPage();
  for (const script of cfg.init_scripts || []) {
    await page.addInitScript({ path: join(PAGE_SCRIPTS, script) });
  }
  page.on('console', (msg) => {
    const text = msg.text();
    const level = { error: 'ERR', warning: 'WARN' }[msg.type()] || 'DEBG';
    if (!onConsole || onConsole(text) !== false) log(tag + text, level);
    scan(text);
  });
  page.on('pageerror', (err) => {
    const text = err.stack || err.message || String(err);
    state.pageErrors++;
    log(tag + text, 'ERR');
    scan(text);
  });
  page.on('requestfailed', (req) => {
    const err = req.failure();
    log(`${tag}request failed: ${req.method()} ${req.url()} - ${err ? err.errorText : '?'}`, 'ERR');
  });
  page.on('crash', () => { log(`${tag}page crashed`, 'ERR'); state.fatal = true; });
  return { page, state };
}

// Window state the instrument init script keeps.
export const harness = (page) => page.evaluate(() => window.__harness || {}).catch(() => ({}));

export const statusText = (page) =>
  page.evaluate(() => (document.getElementById('status') || {}).innerHTML || '').catch(() => '');

export async function saveVideo(page, dest) {
  const video = page.video();
  if (!video) return null;
  await page.context().close();
  try {
    renameSync(await video.path(), dest);
    return dest;
  } catch (e) {
    log(`video save failed: ${e.message}`, 'ERR');
    return null;
  }
}

// Runs a driver body, always writing result.json and closing browsers.
export async function run(body) {
  const cfg = loadConfig();
  const result = { error: null };
  try {
    Object.assign(result, await body(cfg));
  } catch (e) {
    result.error = e.stack || e.message || String(e);
    log(result.error, 'ERR');
  } finally {
    await closeAll();
    writeResult(cfg, result);
  }
}
