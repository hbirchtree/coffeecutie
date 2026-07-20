// Drives testpage.html in a real headless browser (WebRTC has no
// existence outside one) through webrtc-gateway, round-tripping a payload
// via a UDP echo server standing in for the eventual native GNS server.
// See WEBRTC_TRANSPORT.md's Phase 1 verification step.
//
// Usage: node run_phase1_test.mjs [gatewayWsUrl] [message]
import { chromium } from 'playwright';
import { fileURLToPath, pathToFileURL } from 'node:url';
import path from 'node:path';

const here = path.dirname(fileURLToPath(import.meta.url));
const gatewayUrl = process.argv[2] || 'ws://127.0.0.1:8088/signal';
const message = process.argv[3] || 'hello-through-the-gateway';

const browser = await chromium.launch({
    args: [
        // Headless Chrome defaults to a fake UDP-mux-only ICE candidate
        // policy in some configurations; force real host candidates and
        // disable mDNS obfuscation so the local test (loopback) actually
        // negotiates instead of only gathering unusable mdns candidates.
        '--force-webrtc-ip-handling-policy=default',
    ],
});
const page = await browser.newPage();
page.on('console', (msg) => console.log('[page]', msg.text()));
page.on('pageerror', (err) => console.error('[page error]', err));

await page.goto(pathToFileURL(path.join(here, 'testpage.html')).href);

const result = await page.evaluate(
    ([url, msg]) => window.__runGatewayTest(url, msg),
    [gatewayUrl, message],
);

await browser.close();

const expected = 'echo:' + message;
if (result.status === 'ok' && result.echoed === expected) {
    console.log(`PASS: received ${JSON.stringify(result.echoed)}`);
    process.exit(0);
} else {
    console.error(`FAIL: ${JSON.stringify(result)} (expected echoed=${JSON.stringify(expected)})`);
    process.exit(1);
}
