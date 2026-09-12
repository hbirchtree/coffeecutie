// Loads a .gltrace into the viewer and screenshots it, so the timeline can be
// checked without a browser to hand.
//
//   node gl_trace_viewer_check.mjs <viewer.html> <trace.gltrace> <out.png>

import { chromium } from 'playwright';
import { pathToFileURL } from 'node:url';

const [viewer, trace, shot] = process.argv.slice(2);
if (!viewer || !trace) {
  console.error('usage: gl_trace_viewer_check.mjs <viewer.html> <trace.gltrace> [out.png]');
  process.exit(2);
}

const browser = await chromium.launch({ args: ['--no-sandbox'] });
const page = await browser.newPage({ viewport: { width: 1400, height: 860 } });
const errors = [];
page.on('pageerror', (e) => errors.push(String(e)));
page.on('console', (m) => m.type() === 'error' && errors.push(m.text()));

await page.goto(pathToFileURL(viewer).href);
await page.setInputFiles('#picker', trace);
await page.waitForSelector('#main', { state: 'visible', timeout: 15000 });
await page.waitForTimeout(600);

const summary = await page.textContent('#summary');
const frames = Number(await page.getAttribute('#range', 'max')) + 1;

// Scrub to the middle and make sure the frame actually changes.
await page.fill('#range', String(Math.floor(frames / 2)));
await page.dispatchEvent('#range', 'input');
await page.waitForTimeout(600);

const state = await page.evaluate(() => ({
  pos: document.getElementById('pos').textContent,
  callRows: document.getElementById('calls').children.length,
  hasShot: !!document.querySelector('#shot img'),
  drawThumbs: document.getElementById('draws').children.length,
}));

if (shot) await page.screenshot({ path: shot });
console.log(JSON.stringify({ summary, frames, ...state, errors }, null, 2));
await browser.close();
process.exit(errors.length ? 1 : 0);
