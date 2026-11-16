#!/usr/bin/env node
// Wasm client (and optionally a wasm host) through a webrtc-gateway. Wasm has
// no dummy plug, so run.py judges the console markers this records.

import { log, run, newContext, openPage, waitFor } from './lib.mjs';

// One browser per role: a second page in one browser is a background tab,
// pinned to 1 fps, which stalls the host's signaling until it expires.
async function boot(cfg, role, spec) {
  const context = await newContext(cfg.browser, { viewport: spec.viewport });
  const { page, state } = await openPage(context, cfg, { role, markers: spec.markers });
  state.timedOut = false;
  log(`[${role}] opening ${spec.url}`);
  try {
    await page.goto(spec.url, { waitUntil: 'load', timeout: cfg.boot_timeout_ms });
  } catch (e) {
    state.timedOut = true;
    log(`[${role}] page load timed out: ${e.message}`, 'WARN');
  }
  return state;
}

const summary = (s) => s && ({
  found: s.found, fatal: s.fatal, timedOut: s.timedOut, worstStallMs: s.worstStallMs,
  mapLoaded: s.mapLoaded, framesSinceLoad: s.framesSinceLoad,
});

run(async (cfg) => {
  let host = null;
  if (cfg.host) {
    const spec = cfg.host;
    host = await boot(cfg, 'host', spec);
    // Nothing may dial a server the gateway has not accepted yet
    const ready = await waitFor(
      () => spec.ready_markers.every((m) => host.found.includes(m)) || host.fatal, spec.ready_timeout_ms);
    if (!ready || host.fatal || host.timedOut) {
      return { host: summary(host), client: null, error: 'wasm host never reached an active registration' };
    }
    // The first frames on a loaded map compile shaders and block the loop
    // that answers the gateway; dial once it is ticking again
    const warm = await waitFor(
      () => (host.mapLoaded && host.framesSinceLoad >= spec.warm.frames
             && Date.now() - host.lastFrameAt < spec.warm.gap_ms) || host.fatal,
      spec.warm.timeout_ms);
    if (!warm) log('wasm host frame loop never settled; dialing regardless', 'WARN');
  }

  const client = await boot(cfg, 'client', cfg.client);
  if (!client.timedOut && !client.fatal) {
    log(`Waiting up to ${cfg.connect_timeout_ms}ms for connect + roster sync...`);
    await waitFor(
      () => (client.found.length === cfg.client.markers.length
             && (!host || host.found.length === cfg.host.markers.length))
            || client.fatal || (host && host.fatal),
      cfg.connect_timeout_ms);
  }
  return { host: summary(host), client: summary(client) };
});
