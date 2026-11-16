"""Wasm clients in headless Chromium, through a webrtc-gateway.

The wasm build has no dummy plug or journal, so the Playwright driver
decides pass/fail from the connect/join/roster-sync console markers.
"""

import contextlib
import os
import re
import subprocess
from pathlib import Path

from . import engine as eng
from .common import (
    IN_CI,
    SRC_DIR,
    WEB_DIR,
    Proc,
    SetupError,
    TestFailure,
    dump,
    dump_matching,
    group,
    log,
    log_has,
    run_checked,
)
from .native import (
    add_gateway_args,
    add_timeouts,
    fresh_dir,
    plug_path,
    start_engine,
    start_gateway,
    wait_server_boot,
)

SMOKE = WEB_DIR / "webrtc_client_smoke.mjs"
SERVER_PLUG = "dummy_plug_net_webrtc_server_for_web_client.json"
SERVER_LINES = "gateway_fleet_registration|Connection info|Player joined|state change|Error|error"


def add_web_args(parser):
    parser.add_argument("bundle", nargs="?", type=Path, help="BlamGraphics.bundle directory")
    parser.add_argument("--boot-timeout-ms", type=int, default=60000)
    parser.add_argument("--connect-timeout-ms", type=int, default=90000)
    parser.add_argument("--server-plug", default=SERVER_PLUG)
    add_timeouts(parser, boot=60, run=150)


def find_bundle(path):
    """The bundle itself, or the first one under a directory (default: multi_build)."""
    if path and (Path(path) / "BlamGraphics.html").is_file():
        return Path(path).resolve()
    root = Path(path) if path else SRC_DIR / "multi_build"
    for bundle in sorted(root.rglob("BlamGraphics.bundle")) if root.is_dir() else []:
        if ".web_scratch" not in str(bundle) and (bundle / "BlamGraphics.html").is_file():
            return bundle.resolve()
    raise SetupError(f"no BlamGraphics.bundle under {root}")


def link_maps(map_path, bundle):
    """The page fetches maps by bare name from the bundle's own server."""
    for m in map_path.parent.glob("*.map"):
        link = bundle / m.name
        link.unlink(missing_ok=True)
        link.symlink_to(m)


def install_playwright():
    with group("Installing Playwright"):
        if not (WEB_DIR / "node_modules" / "playwright").is_dir():
            run_checked(["npm", "install", "--no-audit", "--no-fund"], "npm install", cwd=WEB_DIR)
        run_checked(["npx", "playwright", "install", *(["--with-deps"] if IN_CI else []), "chromium"],
                    "playwright install", cwd=WEB_DIR)


def run_wasm_client(args, bundle, out, server_url, **env):
    client_out = fresh_dir(out / "client")
    log(f"Browser console -> {client_out / 'console.log'}")
    proc = Proc(
        "wasm client",
        ["node", SMOKE],
        client_out / "console.log",
        env=dict(
            os.environ,
            BUNDLE_DIR=str(bundle),
            OUT_DIR=str(client_out),
            SERVER_URL=server_url,
            BOOT_TIMEOUT_MS=str(args.boot_timeout_ms),
            CONNECT_TIMEOUT_MS=str(args.connect_timeout_ms),
            **{k: str(v) for k, v in env.items()},
        ),
        timeout=1800,
    )
    return proc, proc.wait()


# -- web-client -----------------------------------------------------------------


def add_web_client_args(parser):
    add_web_args(parser)
    add_gateway_args(parser)
    parser.add_argument("--server-role", choices=("native", "wasm"), default="native",
                        help="wasm: a second page hosts, the gateway bridges two DataChannels")
    parser.add_argument("--gateway-url", help="use a running gateway instead of a local one")
    parser.add_argument("--host-ready-timeout-ms", type=int, default=180000)


def run_web_client(args):
    bundle = find_bundle(args.bundle)
    map_path = eng.fetch_maps(args, [args.map])
    out = fresh_dir(args.out)
    native = args.server_role == "native"
    engine = eng.engine_from_args(args) if native else None
    link_maps(map_path, bundle)
    install_playwright()
    log(f"Bundle : {bundle}")
    log(f"Server : {args.server_role}, serverId={args.server_id}")

    with contextlib.ExitStack() as stack:
        stack.callback(Proc.kill_all)
        gateway = None
        if args.gateway_url:
            gw_url = args.gateway_url
        else:
            gateway, gw_url = start_gateway(args, out, args.boot_timeout)
        target = f"{gw_url}#{args.server_id}"

        server = None
        host_env = {}
        if native:
            server = start_engine(
                "server", engine, map_path, out, plug_path(args.server_plug), args.run_timeout,
                "--listen", f"127.0.0.1:{args.server_udp_port}",
                "--gateway-register", gw_url, "--gateway-server-id", args.server_id)
            try:
                wait_server_boot(server, f"registration active for serverId={args.server_id}",
                                 args.boot_timeout)
            except TestFailure:
                if gateway:
                    dump("gateway.log", gateway.log_path)
                raise
        else:
            # The host is a second page; graphics.cpp takes --map as a filename
            host_env = dict(HOST_URL=target, HOST_MAP=map_path.name,
                            HOST_READY_TIMEOUT_MS=args.host_ready_timeout_ms)

        log(f"Starting wasm client -> #{args.server_id}")
        client, client_rc = run_wasm_client(args, bundle, out, target, **host_env)

        # The server's end_time is sized for a slow boot; don't wait it out
        server_rc = 0
        if server and not server.alive():
            server_rc = server.wait()
        log(f"server exit={server_rc}  client exit={client_rc}")

        if client_rc or server_rc == Proc.TIMED_OUT:
            if gateway:
                dump("gateway.log", gateway.log_path)
            if server:
                dump_matching("server.log", server.log_path, SERVER_LINES)
            dump("client (browser console)", client.log_path)
        if client_rc:
            raise TestFailure("wasm client did not complete the connect/join/roster-sync sequence")
        if server_rc == Proc.TIMED_OUT:
            raise TestFailure(f"server hit the {args.run_timeout}s timeout")
    log(f"PASS  (logs in {out})")


# -- nat-punch ------------------------------------------------------------------


def add_nat_punch_args(parser):
    add_web_args(parser)
    parser.add_argument("--server-udp-port", type=int, default=19601)
    parser.add_argument("--server-id",
                        default=f"nat-{os.environ.get('GITHUB_RUN_ID', f'local{os.getpid()}')}"
                                f"-{os.environ.get('GITHUB_RUN_ATTEMPT', '1')}",
                        help="must be unique per concurrent run; the gateway is shared")


def run_nat_punch(args):
    """Native server behind the runner's NAT, gateway out on the internet.

    Registration and relay can only work if the server's own UDP punches
    opened the mappings; nothing is port-forwarded.
    """
    gw_url = os.environ.get("GATEWAY_URL")
    if not gw_url:
        raise SetupError("GATEWAY_URL is required (CI: the WEBRTC_GATEWAY_SERVER secret)")
    bundle = find_bundle(args.bundle)
    map_path = eng.fetch_maps(args, [args.map])
    out = fresh_dir(args.out)
    engine = eng.engine_from_args(args)
    link_maps(map_path, bundle)
    install_playwright()
    log(f"Server : udp 0.0.0.0:{args.server_udp_port} (not forwarded), serverId={args.server_id}")

    with contextlib.ExitStack() as stack:
        stack.callback(Proc.kill_all)
        server = start_engine(
            "server", engine, map_path, out, plug_path(args.server_plug), args.run_timeout,
            "--listen", f"0.0.0.0:{args.server_udp_port}",
            "--gateway-register", gw_url, "--gateway-server-id", args.server_id)
        try:
            wait_server_boot(server, f"registration active for serverId={args.server_id}",
                             args.boot_timeout)
        except TestFailure:
            _explain_registration_failure(server.log_path, gw_url)
            raise

        client, client_rc = run_wasm_client(args, bundle, out, f"{gw_url}#{args.server_id}")

        # Our side of the NAT: the gateway asked for a per-client relay punch
        marker = "relay punch started for"
        punched = log_has(server.log_path, marker)
        if punched:
            dump_matching("per-client relay punches", server.log_path, re.escape(marker))
        if client_rc or not punched:
            dump_matching("server.log", server.log_path, SERVER_LINES)
            dump("client (browser console)", client.log_path)
        if client_rc:
            raise TestFailure("wasm client did not complete the connect/join/roster-sync sequence")
        if not punched:
            raise TestFailure("server never punched a per-client relay port")
    log(f"PASS  (logs in {out})")


def _explain_registration_failure(server_log, gw_url):
    text = server_log.read_text(errors="replace") if server_log.exists() else ""
    if "websocket error" not in text:
        log("  (the gateway never got our punch, or its reply never came back)")
        return
    log("  (the signaling websocket never connected: the remote gateway is down or unreachable)")
    probe = re.sub(r"^ws(s?)://", r"http\1://", gw_url.split("#")[0]) + "/server-signal"
    host = re.sub(r"^https?://", "", probe).split("/")[0].split(":")[0]
    res = subprocess.run(
        ["curl", "-sS", "-o", "/dev/null", "-D", "-", "--http1.1", "--max-time", "10",
         "-H", "Connection: Upgrade", "-H", "Upgrade: websocket",
         "-H", "Sec-WebSocket-Version: 13", "-H", "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==",
         probe],
        capture_output=True, text=True,
    )
    lines = [l for l in (res.stdout + res.stderr).replace("\r", "").splitlines()
             if re.match(r"(?i)^(HTTP/|server:|curl:)", l)]
    # The gateway URL is a secret; keep it out of the job log
    log(f"  runner probe of the upgrade: {' '.join(lines).replace(host, '<gateway>') or 'no response'}")
    for line in [l for l in text.splitlines() if "libdatachannel:" in l][:5]:
        log("  " + line.replace(host, "<gateway>"))
