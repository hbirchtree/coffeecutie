#!/usr/bin/env node
// Injects exact arm directions into BlamPoseDemo as pose_apply events and
// reads back where the bones went with dump_pose. run.py holds the cases and
// judges the positions.

import { join } from 'node:path';
import { log, run, newContext, openPage, harness, sleep } from './lib.mjs';

const ARM_BONES = ['LeftUpperArm', 'RightUpperArm', 'LeftLowerArm', 'RightLowerArm'];

run(async (cfg) => {
  const context = await newContext(cfg.browser, { viewport: cfg.viewport });
  // Denied on purpose: without a webcam nothing competes with injected poses
  await context.clearPermissions();

  const missingBones = new Set();
  let collecting = null;
  const { page, state } = await openPage(context, cfg, {
    onConsole: (text) => {
      const missing = text.match(/apply_pose: bone '([^']+)' not found/);
      if (missing) missingBones.add(missing[1]);
      // "bone_pos <name> <x> <y> <z>"; names contain spaces, so parse from the end
      const pos = text.match(/bone_pos (.+) (-?[\d.e+-]+) (-?[\d.e+-]+) (-?[\d.e+-]+)\s*$/);
      if (pos && collecting) {
        collecting[pos[1].trim()] = [Number(pos[2]), Number(pos[3]), Number(pos[4])];
        return false;
      }
      return /apply_pose|pose_demo: |spawn/i.test(text);
    },
  });

  log(`Opening ${cfg.url}`);
  await page.goto(cfg.url, { waitUntil: 'load', timeout: cfg.boot_timeout_ms });
  await page.waitForFunction(
    (mod) => {
      const h = window.__harness || {};
      if (!h.contextType || h.frames < 5) return false;
      try { return window[mod].ccall('coffee_app_loaded', 'number', [], []) === 1; } catch { return false; }
    },
    cfg.module,
    { timeout: cfg.boot_timeout_ms, polling: 500 },
  );
  log(`app loaded; settling ${cfg.settle_seconds}s for the biped to spawn`);
  await sleep(cfg.settle_seconds * 1000);

  const send = (payload) => page.evaluate(([mod, p]) => {
    window[mod].ccall('coffee_dummy_plug_event', null, ['string'], [JSON.stringify(p)]);
  }, [cfg.module, payload]);

  // Every arm bone each time, so none keeps the previous case's direction
  const poses = {};
  for (const c of cfg.cases) {
    await send({ type: 'pose_apply', bones: ARM_BONES.map((name) => ({ name, direction: c.dirs[name] })) });
    await sleep(cfg.case_seconds * 1000);
    await page.screenshot({ path: join(cfg.out_dir, `${c.name}.png`), type: 'png' });
    collecting = {};
    await send({ type: 'dump_pose' });
    await sleep(500);
    poses[c.name] = collecting;
    collecting = null;
    log(`case ${c.name}: ${Object.keys(poses[c.name]).length} bones`);
  }

  return { harness: await harness(page), fatal: state.fatal, missingBones: [...missingBones], poses };
});
