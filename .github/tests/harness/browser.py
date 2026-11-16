"""Static bundle server, Playwright setup and driver invocation.

Drivers in .github/tests/web take one JSON config and write result.json; every
setting and every check lives on this side.
"""

import contextlib
import http.server
import json
import os
import re
import shutil
import threading
from pathlib import Path
from urllib.parse import unquote

from .common import IN_CI, SRC_DIR, WEB_DIR, SetupError, TestFailure, group, log, run_checked, run_tee

SWIFTSHADER = [
    "--use-gl=angle", "--use-angle=swiftshader", "--enable-unsafe-swiftshader",
    "--ignore-gpu-blocklist", "--enable-webgl", "--disable-dev-shm-usage",
    "--disable-gpu-watchdog",
]
GPU = [
    "--use-gl=angle", "--use-angle=gl", "--ignore-gpu-blocklist", "--enable-webgl",
    "--disable-dev-shm-usage",
]
# Signaling rides the frame loop, so a throttled page stops answering the gateway
WEBRTC = [
    "--force-webrtc-ip-handling-policy=default",
    "--disable-background-timer-throttling",
    "--disable-backgrounding-occluded-windows",
    "--disable-renderer-backgrounding",
]

# A wasm trap, as opposed to page errors a map-less run legitimately produces
FATAL_PATTERNS = [
    r"[Aa]bort\(", r"\bAborted\(", r"RuntimeError", r"Cannot enlarge memory",
    r"memory access out of bounds", r"\bOOM\b", r"[Mm]aximum call stack",
]

MIME = {
    ".html": "text/html; charset=utf-8",
    ".js": "text/javascript; charset=utf-8",
    ".mjs": "text/javascript; charset=utf-8",
    ".wasm": "application/wasm",
    ".json": "application/json; charset=utf-8",
    ".css": "text/css; charset=utf-8",
    ".ico": "image/x-icon",
}


def browser_profile(args, headless=True):
    return {"headless": headless, "args": list(args), "profile_dir": None}


def viewport(spec):
    m = re.fullmatch(r"(\d+)x(\d+)", spec)
    if not m:
        raise SetupError(f"viewport must be WxH, got {spec}")
    return {"width": int(m[1]), "height": int(m[2])}


def find_bundle(path, name="BlamGraphics"):
    """The bundle itself, or the first one under a directory (default: multi_build)."""
    page = f"{name}.html"
    if path and (Path(path) / page).is_file():
        return Path(path).resolve()
    root = Path(path) if path else SRC_DIR / "multi_build"
    for bundle in sorted(root.rglob(f"{name}.bundle")) if root.is_dir() else []:
        if ".web_scratch" not in str(bundle) and (bundle / page).is_file():
            return bundle.resolve()
    raise SetupError(f"no {name}.bundle under {root}")


class BundleServer:
    """Serves a bundle with the isolation headers threaded wasm needs.

    Range requests are served because the wasm build reads big cache files by
    slice. files maps extra paths (maps) to files outside the bundle, and
    overrides answers paths with generated content.
    """

    def __init__(self, root, port=0, ranges=True, files=None, overrides=None):
        root = Path(root).resolve()
        files = {k: Path(v) for k, v in (files or {}).items()}
        overrides = overrides or {}

        class Handler(http.server.BaseHTTPRequestHandler):
            def log_message(self, *_):
                pass

            def do_HEAD(self):
                self.do_GET(head=True)

            def do_GET(self, head=False):
                rel = self.path.split("?")[0].split("#")[0].lstrip("/")
                rel = unquote(rel)
                if rel in overrides:
                    return self._send(200, overrides[rel], "application/json", head)
                path = files.get(rel) or Path(os.path.normpath(root / rel))
                if (rel not in files and root not in path.parents) or not path.is_file():
                    return self._send(404, b"not found", "text/plain", head)
                size = path.stat().st_size
                start, end, status = 0, size - 1, 200
                m = ranges and re.fullmatch(r"bytes=(\d+)-(\d*)", self.headers.get("Range", ""))
                if m:
                    start = int(m[1])
                    end = min(int(m[2]), size - 1) if m[2] else size - 1
                    if start > end:
                        self.send_response(416)
                        self.send_header("Content-Range", f"bytes */{size}")
                        self.end_headers()
                        return
                    status = 206
                self.send_response(status)
                self._headers(MIME.get(path.suffix, "application/octet-stream"), end - start + 1)
                if status == 206:
                    self.send_header("Content-Range", f"bytes {start}-{end}/{size}")
                self.end_headers()
                if head:
                    return
                with open(path, "rb") as f, contextlib.suppress(BrokenPipeError, ConnectionResetError):
                    f.seek(start)
                    left = end - start + 1
                    while left > 0:
                        chunk = f.read(min(left, 1 << 20))
                        if not chunk:
                            break
                        self.wfile.write(chunk)
                        left -= len(chunk)

            def _headers(self, ctype, length):
                self.send_header("Content-Type", ctype)
                self.send_header("Content-Length", str(length))
                if ranges:
                    self.send_header("Accept-Ranges", "bytes")
                self.send_header("Cross-Origin-Opener-Policy", "same-origin")
                self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
                self.send_header("Cross-Origin-Resource-Policy", "cross-origin")
                self.send_header("Cache-Control", "no-store")

            def _send(self, status, body, ctype, head):
                self.send_response(status)
                self._headers(ctype, len(body))
                self.end_headers()
                if not head:
                    self.wfile.write(body)

        self.httpd = http.server.ThreadingHTTPServer(("127.0.0.1", port), Handler)
        self.httpd.daemon_threads = True
        self.url = f"http://127.0.0.1:{self.httpd.server_address[1]}"

    def __enter__(self):
        threading.Thread(target=self.httpd.serve_forever, daemon=True).start()
        return self

    def __exit__(self, *_):
        self.httpd.shutdown()
        self.httpd.server_close()


def install_playwright():
    with group("Installing Playwright"):
        if not (WEB_DIR / "node_modules" / "playwright").is_dir():
            run_checked(["npm", "install", "--no-audit", "--no-fund"], "npm install", cwd=WEB_DIR)
        run_checked(["npx", "playwright", "install", *(["--with-deps"] if IN_CI else []), "chromium"],
                    "playwright install", cwd=WEB_DIR)


def run_driver(script, cfg, out, timeout=1800, display=False, log_name="output.log"):
    """Runs a driver, its console teed to out/<log_name>; returns result.json."""
    cfg = {"fatal_patterns": FATAL_PATTERNS, "init_scripts": [], **cfg, "out_dir": str(out)}
    out.mkdir(parents=True, exist_ok=True)
    cfg_path = out / f"{Path(script).stem}_config.json"
    cfg_path.write_text(json.dumps(cfg, indent=1))
    (out / "result.json").unlink(missing_ok=True)
    argv = ["node", WEB_DIR / script, cfg_path]
    # Headful runs need an X server; give them a private one when there is none
    if display and not os.environ.get("DISPLAY"):
        if not shutil.which("xvfb-run"):
            raise SetupError(f"{script} needs a display: set DISPLAY or install xvfb")
        argv = ["xvfb-run", "-a", "-s", "-screen 0 1280x1024x24", *argv]
    rc = run_tee(argv, dict(os.environ), out / log_name, timeout, WEB_DIR)
    try:
        result = json.loads((out / "result.json").read_text())
    except (OSError, ValueError):
        raise TestFailure(f"{script} wrote no result (exit {rc})")
    if result.get("error"):
        log(f"driver error: {result['error'].splitlines()[0]}")
    return result


def verdict(title, checks, info=()):
    """Prints a check table; raises if any check failed."""
    log()
    log(f"================ {title} ================")
    for line in info:
        log(line)
    for label, ok in checks:
        log(f"  {'PASS' if ok else 'FAIL'}  {label}")
    log("=" * (len(title) + 34))
    failed = [label for label, ok in checks if not ok]
    if failed:
        raise TestFailure(f"{len(failed)} check(s) failed")
