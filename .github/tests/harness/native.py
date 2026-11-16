"""Tests that only run native BlamGraphics processes (plus the gateway)."""

import argparse
import atexit
import base64
import contextlib
import json
import shutil
import subprocess
import sys
import tempfile
import time
from pathlib import Path

from . import engine as eng
from .common import (
    NET_DIR,
    SRC_DIR,
    TESTS_DIR,
    Proc,
    SetupError,
    TestFailure,
    dump,
    dump_matching,
    group,
    log,
    log_has,
    run_checked,
    run_tee,
    wait_for_log,
    wait_for_port,
)

SCENARIOS = NET_DIR / "scenarios.json"
COMPARE = NET_DIR / "compare_journals.py"
DEFAULT_GATEWAY = SRC_DIR / "tools" / "webrtc-gateway" / "gateway"

# route -> (via gateway, server transport, client relay only, server relay only)
ROUTES = {
    "no-gateway": (False, "udp", False, False),
    "direct": (True, "udp", False, False),
    "relay": (True, "udp", True, False),
    "webrtc-hosted": (True, "webrtc", False, False),
    "relay-only-server": (True, "udp", False, True),
    "relay-only-webrtc-server": (True, "webrtc", False, True),
}


def add_timeouts(parser, boot=120, run=150):
    parser.add_argument(
        "--boot-timeout", type=int, default=boot, help="seconds to wait for each boot signal"
    )
    parser.add_argument(
        "--run-timeout", type=int, default=run, help="cap on each engine process"
    )


def add_gateway_args(parser):
    parser.add_argument("--gateway-bin", type=Path, default=DEFAULT_GATEWAY)
    parser.add_argument(
        "--build-gateway", action="store_true", help="go build the gateway first"
    )
    parser.add_argument("--gateway-port", type=int, default=8098)
    parser.add_argument("--gateway-args", default="", help="extra gateway flags")
    parser.add_argument("--server-udp-port", type=int, default=19601)
    parser.add_argument("--server-id", default="test")


def plug_path(name):
    p = Path(name)
    if p.is_file():
        return p.resolve()
    for base in (TESTS_DIR, NET_DIR):
        if (base / name).is_file():
            return base / name
    raise SetupError(f"no dummy plug config {name}")


def fresh_dir(path):
    path = Path(path).resolve()
    path.mkdir(parents=True, exist_ok=True)
    return path


# -- dummy-plug ---------------------------------------------------------------


def run_dummy_plug(args):
    engine = eng.engine_from_args(args)
    plug = plug_path(args.plug)
    map_path = eng.fetch_maps(args, [args.map, "beavercreek.map", "bloodgulch.map"])
    out = fresh_dir(args.out)

    log(f"Plug   : {plug}")
    log(f"Map    : {map_path}")
    log(f"Output : {out}")
    argv = engine.argv(map_path)
    with group("Running BlamGraphics"):
        rc = run_tee(argv, engine.environ(out, plug), out / "output.txt", args.run_timeout, engine.cwd)
        log(f"Return code: {rc}")

    _convert_audio(out, keep=not args.no_audio)
    if rc != 0:
        raise TestFailure(f"BlamGraphics exited with {rc}")
    if args.journal_check:
        if not compare_journals(
            out / "journal.jsonl", out / "client0" / "journal.jsonl", args.journal_check
        ):
            raise TestFailure("journal comparison failed")


def _convert_audio(out, keep):
    for raw in out.rglob("rendered_audio"):
        if keep and shutil.which("ffmpeg"):
            subprocess.run(
                ["ffmpeg", "-loglevel", "error", "-y", "-f", "f32le", "-ar", "48k",
                 "-ac", "2", "-i", raw, "-c:a", "copy", raw.with_suffix(".wav")],
                check=False,
            )
        if not keep or raw.with_suffix(".wav").exists():
            raw.unlink()


# -- shared networking plumbing ---------------------------------------------


def find_gateway(args):
    if args.build_gateway:
        go = shutil.which("go") or str(Path.home() / "local/go/bin/go")
        with group("Building webrtc-gateway"):
            run_checked([go, "build", "-o", "gateway", "."], "gateway build",
                        cwd=DEFAULT_GATEWAY.parent)
    gw = args.gateway_bin.resolve()
    if not gw.is_file():
        raise SetupError(f"no gateway binary at {gw} (pass --gateway-bin or --build-gateway)")
    gw.chmod(gw.stat().st_mode | 0o111)
    return gw


def start_gateway(args, out, timeout):
    gw = find_gateway(args)
    log(f"Starting gateway on :{args.gateway_port}")
    # -admin-port :0 so a stray gateway from an earlier run can't block the bind;
    # its generated admin host key stays out of the checkout and the artifacts
    key = Path(tempfile.mkdtemp(prefix="blamtest-gw-")) / "admin_key"
    atexit.register(shutil.rmtree, key.parent, True)
    proc = Proc(
        "gateway",
        [gw, "-listen", f":{args.gateway_port}", "-admin-port", ":0",
         "-admin-private-key", key, *args.gateway_args.split()],
        out / "gateway.log",
    )
    try:
        wait_for_port(proc, args.gateway_port, timeout)
    except TestFailure:
        dump("gateway.log (tail)", proc.log_path, 40)
        raise
    return proc, f"ws://127.0.0.1:{args.gateway_port}"


def start_engine(name, engine, map_path, out, plug, timeout, *extra):
    tmp = fresh_dir(out / f"{name}_tmp")
    (tmp / "journal.jsonl").unlink(missing_ok=True)
    return Proc(
        name,
        engine.argv(map_path, *extra),
        out / f"{name}.log",
        env=engine.environ(tmp, plug),
        timeout=timeout,
        cwd=engine.cwd,
    )


def wait_server_boot(server, needle, timeout):
    try:
        wait_for_log(server, needle, timeout)
    except TestFailure:
        dump("server.log (tail)", server.log_path, 60)
        raise
    log("Server is up.")


def compare_journals(server_journal, client_journal, scenario=None, network=None):
    for journal in (server_journal, client_journal):
        if not Path(journal).is_file() or Path(journal).stat().st_size == 0:
            log(f"FAIL: {journal} was never written (process died at startup?)")
            return False
    opts = []
    if scenario:
        opts += ["--scenario", scenario]
    if network:
        opts += ["--network", network]
    with group("Journal comparison"):
        ok = _run_logged([sys.executable, COMPARE, *opts, server_journal, client_journal]) == 0
    if not ok:
        with group("Merged journal timeline"):
            _run_logged([sys.executable, COMPARE, "--timeline", server_journal, client_journal])
    return ok


def _run_logged(argv):
    """Runs into whatever sys.stdout is now, so scenario logs capture it."""
    sys.stdout.flush()
    return subprocess.run(argv, stdout=sys.stdout, stderr=subprocess.STDOUT).returncode


def load_manifest():
    return json.loads(SCENARIOS.read_text())


def network_args(name):
    profile = load_manifest()["networks"].get(name)
    if profile is None:
        raise SetupError(f"no network profile {name} in {SCENARIOS}")
    out = []
    for key, flag in (("lag_ms", "--net-lag"), ("jitter_ms", "--net-jitter"),
                      ("loss_pct", "--net-loss")):
        if key in profile:
            out += [flag, str(profile[key])]
    return out


# -- net ------------------------------------------------------------------------


def add_net_args(parser):
    parser.add_argument("--route", choices=ROUTES, default="no-gateway")
    parser.add_argument("--scenario", help="scenario from net/scenarios.json")
    parser.add_argument("--network", default="lan", help="network profile from net/scenarios.json")
    parser.add_argument("--port", type=int, default=27105, help="no-gateway listen port")
    parser.add_argument("--client-delay", type=int, default=0,
                        help="seconds between server listening and client start")
    parser.add_argument("--client-args", default="", help="extra client arguments")
    parser.add_argument("--server-plug")
    parser.add_argument("--client-plug")
    add_gateway_args(parser)
    add_timeouts(parser)


def run_net(args, engine=None):
    engine = engine or eng.engine_from_args(args)
    map_path = eng.fetch_maps(args, [args.map])
    out = fresh_dir(args.out)
    via_gateway, transport, relay_only, server_relay_only = ROUTES[args.route]

    plugs = ("dummy_plug_net_webrtc_server.json", "dummy_plug_net_webrtc_client.json") \
        if via_gateway else ("dummy_plug_net_server.json", "dummy_plug_net_client.json")
    client_args = network_args(args.network) + args.client_args.split()
    delay = args.client_delay
    if args.scenario:
        sc = load_manifest()["scenarios"].get(args.scenario, {})
        if "server" not in sc or "client" not in sc:
            raise SetupError(f"{args.scenario} is not a client/server scenario")
        plugs = (sc["server"], sc["client"])
        client_args += sc.get("client_args", [])
        delay = int(sc.get("client_delay", delay))
    server_plug = plug_path(args.server_plug or plugs[0])
    client_plug = plug_path(args.client_plug or plugs[1])

    run_timeout = args.run_timeout
    if delay > 0:
        server_plug = _delayed_plug(server_plug, delay, out)
        run_timeout = max(run_timeout, delay + 25)

    log(f"Route  : {args.route}  scenario={args.scenario or '-'}  network={args.network}")
    log(f"Output : {out}")

    with contextlib.ExitStack() as stack:
        stack.callback(Proc.kill_all)
        if via_gateway:
            gateway, gw_url = start_gateway(args, out, args.boot_timeout)
            target = f"{gw_url}#{args.server_id}"
            route = ["--relay-only"] if server_relay_only else []
            if transport == "webrtc":
                server_args = ["--listen", target, *route]
            else:
                server_args = ["--listen", f"127.0.0.1:{args.server_udp_port}",
                               "--gateway-register", gw_url,
                               "--gateway-server-id", args.server_id, *route]
            boot_marker = f"registration active for serverId={args.server_id}"
            if relay_only:
                client_args.append("--relay-only")
        else:
            gateway = None
            target = f"127.0.0.1:{args.port}"
            server_args = ["--listen", target]
            boot_marker = "Started server on"

        server = start_engine("server", engine, map_path, out, server_plug,
                              run_timeout, *server_args)
        try:
            wait_server_boot(server, boot_marker, args.boot_timeout)
        except TestFailure:
            if gateway:
                dump("gateway.log", gateway.log_path)
            raise
        if delay:
            log(f"Waiting {delay}s before starting the client")
            time.sleep(delay)
        client = start_engine("client", engine, map_path, out, client_plug,
                              run_timeout, "--server", target, *client_args)

        log("Waiting for both processes (dummy plug end_time closes them)...")
        client_rc = client.wait()
        server_rc = server.wait()
        log(f"server exit={server_rc}  client exit={client_rc}")

        failures = []
        if Proc.TIMED_OUT in (server_rc, client_rc):
            failures.append(f"a process hit the {run_timeout}s timeout")
        if not compare_journals(out / "server_tmp" / "journal.jsonl",
                                out / "client_tmp" / "journal.jsonl",
                                args.scenario, args.network):
            failures.append("journal comparison failed")
        if gateway:
            # The server proved its key and its metadata verified against it
            for marker in ("registration active (challenge passed: signature verified",
                           f"server metadata verified for {args.server_id}"):
                if not log_has(gateway.log_path, marker):
                    failures.append(f'gateway log lacks "{marker}"')

        if failures or server_rc or client_rc:
            if gateway:
                dump("gateway.log", gateway.log_path)
            dump_matching("server.log (connection + state-dump lines)", server.log_path,
                          "State dumped|Started server|registration active|"
                          "gateway_fleet_registration|Connection info|Player joined|"
                          "Controller|Error|error|terminate|what\\(\\)")
            dump_matching("client.log (connection + state-dump lines)", client.log_path,
                          "State dumped|join confirmation|roster received|WebRTC|"
                          "Error|error|terminate|what\\(\\)")
        if failures:
            raise TestFailure("; ".join(failures))
    log(f"PASS  (logs in {out})")


def _delayed_plug(src, delay, out):
    """Shifts the server's schedule so it outlives a delayed client."""
    cfg = json.loads(src.read_text())
    for event in cfg["events"]:
        event["time"] += delay * 1000
    cfg["end_time"] = delay * 1000 + 15000
    dst = out / "server_plug_delayed.json"
    dst.write_text(json.dumps(cfg, indent=1))
    return dst


# -- scenarios ------------------------------------------------------------------


def run_scenarios(args):
    scenarios = load_manifest()["scenarios"]
    wanted = args.names or [n for n, s in scenarios.items() if "server" in s and "client" in s]
    runs = []
    for want in wanted:
        name, _, network = want.partition("@")
        for net in [network] if network else scenarios.get(name, {}).get("networks", ["lan"]):
            runs.append((name, net))

    engine = eng.engine_from_args(args)
    args.build = False
    root = fresh_dir(args.out)
    failed = []
    for name, net in runs:
        label = f"{name}@{net}"
        log(f"=== {label}")
        run_args = _copy_args(args, scenario=name, network=net, out=root / label)
        log_path = root / f"{label}.log"
        try:
            with open(log_path, "w") as f, contextlib.redirect_stdout(f):
                run_net(run_args, engine)
            log(f"PASS {label}")
        except (TestFailure, SetupError) as e:
            log(f"FAIL {label}: {e} (log: {log_path})")
            failed.append(label)
        finally:
            Proc.kill_all()
    log()
    if failed:
        raise TestFailure(f"{len(failed)} of {len(runs)} failed: {' '.join(failed)}")
    log(f"All {len(runs)} runs passed")


def _copy_args(args, **changes):
    return argparse.Namespace(**{**vars(args), **changes})


# -- pinned-cert ----------------------------------------------------------------


def run_pinned_cert(args):
    """A server certificate pinned by its Ed25519 key, no gateway involved.

    All three outcomes are checked because any one passes for the wrong
    reason on its own: the right key connects authenticated, the wrong key
    is refused for its cert, and no key still connects unauthenticated.
    """
    engine = eng.engine_from_args(args)
    map_path = eng.fetch_maps(args, [args.map])
    out = fresh_dir(args.out)
    key, wrong = out / "server_identity.pem", out / "other_identity.pem"
    for k in (key, wrong):
        k.unlink(missing_ok=True)
        run_checked(["openssl", "genpkey", "-algorithm", "ed25519", "-out", k],
                    "Ed25519 key generation", capture_output=True)

    cases = [
        ("good", _pubkey(key), lambda r: r["trust"] == "authenticated"),
        ("wrong", _pubkey(wrong), lambda r: not r["connected"] and r["refused"]),
        ("nokey", None, lambda r: r["trust"] == "unauthenticated"),
    ]
    verdicts = []
    for label, pin, ok in cases:
        log(f"\n=== {label} ===")
        result = _pinned_pair(args, engine, map_path, out / label, key, pin)
        log(" ".join(f"{k}={v}" for k, v in result.items()))
        verdicts.append((label, ok(result), result))

    log("\n=========== pinned certificate result ===========")
    for label, ok, result in verdicts:
        log(f"  {'PASS' if ok else 'FAIL'}  {label}: {result}")
    if not all(ok for _, ok, _ in verdicts):
        for label in ("good", "wrong"):
            dump(f"{label}/client.log (tail)", out / label / "client.log", 40)
        raise TestFailure("pinned certificate checks failed")
    log(f"PASS  (logs in {out})")


def _pubkey(pem):
    # Ed25519 SubjectPublicKeyInfo: the raw 32-byte key is the tail
    der = subprocess.run(["openssl", "pkey", "-in", pem, "-pubout", "-outform", "DER"],
                         capture_output=True, check=True).stdout
    return base64.b64encode(der[-32:]).decode()


def _pinned_pair(args, engine, map_path, out, key, pin):
    out = fresh_dir(out)
    target = f"127.0.0.1:{args.port}"
    result = {"connected": False, "trust": "none", "refused": False}
    try:
        server = start_engine("server", engine, map_path, out,
                              plug_path("dummy_plug_net_server.json"), args.run_timeout,
                              "--listen", target, "--gateway-auth-key", key)
        try:
            wait_server_boot(server, "Started server on", args.boot_timeout)
        except TestFailure as e:
            result["error"] = str(e)
            return result
        client_args = ["--server", target] + (["--server-key", pin] if pin else [])
        client = start_engine("client", engine, map_path, out,
                              plug_path("dummy_plug_net_client.json"), args.run_timeout,
                              *client_args)
        client.wait()
    finally:
        Proc.kill_all()

    log_text = (out / "client.log").read_text(errors="replace")
    if "Connection to server/peer established" in log_text:
        result["connected"] = True
        if ", authenticated)" in log_text:
            result["trust"] = "authenticated"
        elif ", unauthenticated)" in log_text:
            result["trust"] = "unauthenticated"
    # Without this a client that never ran would pass the wrong-key case
    result["refused"] = "does not chain to the pinned root" in log_text
    return result
