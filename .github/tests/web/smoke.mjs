#!/usr/bin/env node
// Boots a bundle page, optionally streams dummy plug events into it, lets it
// run, then screenshots it. Checks are run.py's; this only reports.

import { join } from 'node:path';
import { log, run, newContext, openPage, harness, statusText, saveVideo, sleep } from './lib.mjs';

run(async (cfg) => {
  const context = await newContext(cfg.browser, {
    viewport: cfg.viewport,
    recordVideo: cfg.video ? { dir: cfg.out_dir, size: cfg.viewport } : undefined,
  });
  if (cfg.permissions) {
    await context.grantPermissions(cfg.permissions, { origin: new URL(cfg.url).origin });
  } else {
    await context.clearPermissions();
  }
  const { page, state } = await openPage(context, cfg, { counters: cfg.counters });

  log(`Opening ${cfg.url}`);
  let bootTimedOut = false;
  try {
    await page.goto(cfg.url, { waitUntil: 'load', timeout: cfg.boot_timeout_ms });
    await page.waitForFunction(
      (min) => {
        const h = window.__harness || {};
        const status = (document.getElementById('status') || {}).innerHTML || '';
        return (h.contextType && h.frames >= min) || /Running|Exception/.test(status);
      },
      cfg.min_frames,
      { timeout: cfg.boot_timeout_ms, polling: 250 },
    );
  } catch (e) {
    bootTimedOut = true;
    log(`boot wait timed out: ${e.message}`, 'WARN');
  }

  let runMs = cfg.run_seconds * 1000;
  const events = cfg.dummy_events || [];
  if (!state.fatal && events.length) {
    await page.waitForFunction(() => window.__coffeeAppLoaded && window.__coffeeAppLoaded(),
      undefined, { timeout: cfg.boot_timeout_ms, polling: 100 })
      .then(() => log('dummy plug: app loaded, streaming events'))
      .catch(() => log('dummy plug: app never reported loaded, streaming anyway', 'WARN'));
    for (const e of events) {
      setTimeout(() => {
        page.evaluate((j) => window.__coffeeDummyEvent(j), JSON.stringify(e))
          .catch((err) => log(`event inject failed: ${err.message}`, 'WARN'));
      }, e.time);
    }
    runMs = Math.max(runMs, events[events.length - 1].time + 2000);
  }

  // Only a wasm trap cuts this short: a failed map fetch flips the status
  // banner to "Exception thrown" while the render loop carries on.
  if (!state.fatal) {
    log(`Running for ${runMs / 1000}s...`);
    await sleep(runMs);
  }

  const h = await harness(page);
  const status = await statusText(page);
  const glTrace = cfg.init_scripts.includes('gl_tracer.js')
    ? await page.evaluate(() => window.__glTraceDump()).catch((e) => `(trace failed: ${e.message})`)
    : null;

  let screenshot = null;
  if (cfg.screenshot) {
    const path = join(cfg.out_dir, cfg.screenshot.file);
    const target = cfg.screenshot.selector ? page.locator(cfg.screenshot.selector) : page;
    await target.screenshot({ path, type: cfg.screenshot.type, quality: cfg.screenshot.quality })
      .then(() => { screenshot = path; })
      .catch((e) => log(`screenshot failed: ${e.message}`, 'ERR'));
  }
  const video = cfg.video ? await saveVideo(page, join(cfg.out_dir, cfg.video)) : null;

  return {
    harness: h, status, bootTimedOut, screenshot, video, glTrace,
    fatal: state.fatal, pageErrors: state.pageErrors, counts: state.counts, samples: state.samples,
  };
});
