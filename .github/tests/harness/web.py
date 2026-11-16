"""Browser tests: wasm builds driven by Playwright.

run.py serves the bundle, configures the driver and judges its result.json;
the drivers in .github/tests/web only operate the page.
"""

import contextlib
import json
import os
import re
import shutil
import subprocess
from pathlib import Path
from urllib.parse import quote

from . import engine as eng
from .browser import (
    GPU,
    SWIFTSHADER,
    WEBRTC,
    BundleServer,
    browser_profile,
    find_bundle,
    install_playwright,
    run_driver,
    verdict,
    viewport,
)
from .common import (
    WEB_DIR,
    Proc,
    SetupError,
    TestFailure,
    dump,
    dump_matching,
    group,
    log,
    log_has,
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

SERVER_PLUG = "dummy_plug_net_webrtc_server_for_web_client.json"
SERVER_LINES = "gateway_fleet_registration|Connection info|Player joined|state change|Error|error"


def q(value):
    return quote(str(value), safe="")


def map_files(args, rel_maps):
    """Bundle-relative map paths ("xbox_ex/x.map") -> files on disk, or None."""
    files = {}
    for rel in rel_maps:
        map_dir, name = rel.split("/", 1)
        try:
            found = eng.locate_maps(map_dir, [name], args.cache)
        except SetupError as e:
            log(f"no map: {e}")
            return None
        for f in [name] + (["bitmaps.map", "sounds.map"] if map_dir in ("pc", "custom") else []):
            files[f"{map_dir}/{f}"] = found / f
    return files


def summarize(result, keys):
    return {k: result.get(k) for k in keys}


# -- webgl ------------------------------------------------------------------------


def add_webgl_args(p):
    p.add_argument("bundle", nargs="?", type=Path, help="bundle, or a directory to search")
    p.add_argument("--page", default="BlamGraphics.html")
    p.add_argument("--name", default="webgl2_swiftshader", help="screenshot/video base name")
    p.add_argument("--map", dest="web_map", help="map under the map server, e.g. xbox_ex/beavercreek.map")
    p.add_argument("--query", default="", help="extra page parameters, a=b&c=d")
    p.add_argument("--run-seconds", type=int, default=12)
    p.add_argument("--min-frames", type=int, default=5)
    p.add_argument("--boot-timeout-ms", type=int, default=45000)
    p.add_argument("--viewport", default="1280x720")
    p.add_argument("--strict", action="store_true", help="also fail on any page error")
    p.add_argument("--dummy-plug", type=Path, help="stream this plug's events into the page")
    p.add_argument("--no-video", action="store_true")
    p.add_argument("--gpu", action="store_true", help="headful on the real GPU instead of SwiftShader")
    p.add_argument("--gl-trace", action="store_true", help="dump GL call/error counts at the end")
    p.add_argument("--profile-dir", type=Path, help="persistent browser profile (keeps OPFS)")
    p.add_argument("--port", type=int, default=0, help="pin the origin, e.g. for --profile-dir")
    p.add_argument("--no-range", action="store_true", help="serve without Range support")


def run_webgl(args):
    """Proves the wasm build runs: WebGL2, a live render loop, no trap."""
    bundle = find_bundle(args.bundle, Path(args.page).stem)
    out = fresh_dir(args.out)
    query = [args.query] if args.query else []
    files = {}
    if args.web_map:
        files = map_files(args, [args.web_map])
        if files is None:
            log("running without a map")
            files = {}
        else:
            query.append(f"map={args.web_map}")
    events = []
    if args.dummy_plug:
        plug = json.loads(plug_path(args.dummy_plug).read_text())
        events = sorted((e for e in plug.get("events", []) if e.get("type")),
                        key=lambda e: e.get("time", 0))

    browser = browser_profile(GPU + ["--enable-gpu-rasterization", "--disable-software-rasterizer"]
                              if args.gpu else SWIFTSHADER, headless=not args.gpu)
    if args.profile_dir:
        browser["profile_dir"] = str(args.profile_dir.resolve())
    scripts = ["instrument.js"] + (["dummy_plug.js"] if events else []) \
        + (["gl_tracer.js"] if args.gl_trace else [])

    install_playwright()
    with BundleServer(bundle, args.port, not args.no_range, files) as server:
        url = f"{server.url}/{args.page}" + (f"?{'&'.join(query)}" if query else "")
        result = run_driver("smoke.mjs", {
            "url": url, "browser": browser, "viewport": viewport(args.viewport),
            "init_scripts": scripts, "boot_timeout_ms": args.boot_timeout_ms,
            "min_frames": args.min_frames, "run_seconds": args.run_seconds,
            "dummy_events": events,
            "screenshot": {"file": f"{args.name}.jpg", "type": "jpeg", "quality": 80,
                           "selector": "#canvas"},
            "video": None if args.no_video else f"{args.name}.webm",
        }, out, display=args.gpu)

    if result.get("glTrace"):
        dump_text("GL trace", result["glTrace"])
    h = result.get("harness") or {}
    checks = [
        ("WebGL context created", bool(h.get("contextType"))),
        ("context is webgl2", h.get("contextType") == "webgl2"),
        (f">= {args.min_frames} frames rendered (got {h.get('frames', 0)})",
         h.get("frames", 0) >= args.min_frames),
        (f"draw calls issued (got {h.get('drawCalls', 0)})", h.get("drawCalls", 0) > 0),
        ("no wasm abort/trap", not result.get("fatal")),
        ("no WebGL context loss", not h.get("contextLost")),
        ("did not time out booting", not result.get("bootTimedOut")),
        ("driver completed", not result.get("error")),
    ]
    if args.strict:
        checks.append((f"no page errors (got {result.get('pageErrors')})", not result.get("pageErrors")))
    verdict("WebGL smoke result", checks, [
        f"renderer  : {h.get('glRenderer')}",
        f"gl version: {h.get('glVersion')}",
        f"status    : {result.get('status')}",
        f"page errors: {result.get('pageErrors')} (non-fatal unless --strict)",
    ])
    log(f"PASS  (output in {out})")


def dump_text(title, text):
    with group(title):
        for line in str(text).splitlines():
            log(line)


# -- pose demo -------------------------------------------------------------------

# Idle animation frame is (elapsed * fps) % count and fps must be > 0, so the
# smallest positive rate holds it on frame 0 for a whole run
FROZEN_FPS = 0.0001
L_HAND, R_HAND = "bip01 l hand", "bip01 r hand"
L_FOREARM = "bip01 l forearm"

# MediaPipe world space: +x image-right, +y DOWN, +z away from the camera. The
# subject faces the camera, so their left is +x. Kalidokit's Left is solved
# from the subject's RIGHT arm and drives bip01 l: LeftUpperArm below carries
# the subject's right-arm direction. Arms run in "aim" mode, so cases are
# directions; a rotation would move nothing and pass for the wrong reason.
SUBJ_RIGHT, SUBJ_LEFT = [-1, 0, 0], [1, 0, 0]
DOWN, UP, FWD = [0, 1, 0], [0, -1, 0], [0, 0, -1]
UP_FWD = [0, -0.707, -0.707]


def arms(lu, ll, ru, rl):
    return {"LeftUpperArm": lu, "LeftLowerArm": ll, "RightUpperArm": ru, "RightLowerArm": rl}


# blam model space is X forward, Y left, Z up; bip01 l sits at +Y. Each case
# returns (pass, detail) from the bone positions (and the left elbow).
POSE_CASES = [
    # Arms straight out: each hand clear on its OWN side
    ("t_pose", arms(SUBJ_RIGHT, SUBJ_RIGHT, SUBJ_LEFT, SUBJ_LEFT),
     lambda p, e: (p[L_HAND][1] > 0.15 and p[R_HAND][1] < -0.15,
                   f"l_hand y={p[L_HAND][1]:.3f} (want > +0.15), r_hand y={p[R_HAND][1]:.3f} (want < -0.15)")),
    # The inverse of t_pose; a flipped lateral axis would only swap which passes
    ("arms_crossed", arms(SUBJ_LEFT, SUBJ_LEFT, SUBJ_RIGHT, SUBJ_RIGHT),
     lambda p, e: (p[L_HAND][1] < 0 and p[R_HAND][1] > 0,
                   f"l_hand y={p[L_HAND][1]:.3f} (want < 0), r_hand y={p[R_HAND][1]:.3f} (want > 0)")),
    ("arms_down", arms(DOWN, DOWN, DOWN, DOWN),
     lambda p, e: (p[L_HAND][2] < 0.42 and abs(p[L_HAND][1]) < 0.16,
                   f"l_hand z={p[L_HAND][2]:.3f} (want < 0.42), |y|={abs(p[L_HAND][1]):.3f} (want < 0.16)")),
    ("arms_up", arms(UP, UP, UP, UP),
     lambda p, e: (p[L_HAND][2] > 0.7 and p[R_HAND][2] > 0.7,
                   f"l_hand z={p[L_HAND][2]:.3f}, r_hand z={p[R_HAND][2]:.3f} (want both > 0.7)")),
    ("arms_forward", arms(FWD, FWD, FWD, FWD),
     lambda p, e: (p[L_HAND][0] > 0.15 and p[R_HAND][0] > 0.15,
                   f"l_hand x={p[L_HAND][0]:.3f}, r_hand x={p[R_HAND][0]:.3f} (want both > 0.15)")),
    # Forearms tilted up and forward: level would leave "above the elbow" to rounding
    ("elbow_bend", arms(DOWN, UP_FWD, DOWN, UP_FWD),
     lambda p, e: (p[L_HAND][2] > e[2] + 0.03 and p[L_HAND][0] > e[0] + 0.03,
                   f"l_hand z={p[L_HAND][2]:.3f} vs elbow {e[2]:.3f}, "
                   f"x={p[L_HAND][0]:.3f} vs {e[0]:.3f} (want both > +0.03)")),
]


def add_pose_args(p, page_default="BlamPoseDemo.html"):
    p.add_argument("bundle", nargs="?", type=Path, help="bundle, or a directory to search")
    p.add_argument("--page", default=page_default)


def add_pose_arms_args(p):
    add_pose_args(p)
    p.add_argument("--settle-seconds", type=int, default=20)
    p.add_argument("--case-seconds", type=int, default=2)
    p.add_argument("--boot-timeout-ms", type=int, default=180000)
    p.add_argument("--viewport", default="900x700")
    p.add_argument("--camera-pos", help="JSON triple, e.g. [1.5,0,0.6]")


def run_pose_arms(args):
    """Known arm directions in, measured bone positions out.

    Runs headful on the real GPU: SwiftShader loses the context on this bundle.
    """
    bundle = find_bundle(args.bundle, Path(args.page).stem)
    out = fresh_dir(args.out)
    config = json.loads((bundle / "configs" / "cyborg.json").read_text())
    # One injected pose lands in full, and nothing else moves the model
    config["smoothing"] = {"alpha": 1.0}
    config["microphone"] = {"enabled": False}
    config["root_motion"] = {"enabled": False}
    config["animations"] = [{"biped": "*", "fps": FROZEN_FPS, "buttons": []}]
    if args.camera_pos:
        config["camera"]["position"] = json.loads(args.camera_pos)

    cases = [("rest", arms(DOWN, DOWN, DOWN, DOWN), None)] + POSE_CASES
    install_playwright()
    overrides = {"pose_arms_test.json": json.dumps(config).encode()}
    with BundleServer(bundle, overrides=overrides) as server:
        result = run_driver("pose_arms.mjs", {
            "url": f"{server.url}/{args.page}?dummy_plug&config=pose_arms_test.json",
            "browser": browser_profile(GPU, headless=False), "viewport": viewport(args.viewport),
            "init_scripts": ["instrument.js"], "module": "BlamModule",
            "boot_timeout_ms": args.boot_timeout_ms, "settle_seconds": args.settle_seconds,
            "case_seconds": args.case_seconds,
            "cases": [{"name": n, "dirs": d} for n, d, _ in cases],
        }, out, display=True)

    h = result.get("harness") or {}
    poses = result.get("poses") or {}
    missing = result.get("missingBones") or []
    rest = poses.get("rest", {})
    checks = [
        ("driver completed", not result.get("error")),
        ("WebGL context created", bool(h.get("contextType"))),
        ("draw calls issued", h.get("drawCalls", 0) > 0),
        ("no wasm abort/trap", not result.get("fatal")),
        ("no WebGL context loss", not h.get("contextLost")),
        (f"no missing bones ({', '.join(missing) or 'none'})", not missing),
        (f"dump_pose answers ({len(rest)} bones at rest)", L_HAND in rest and R_HAND in rest),
    ]
    for name, _, expect in POSE_CASES:
        p = poses.get(name, {})
        if L_HAND in p and R_HAND in p and L_FOREARM in p:
            ok, detail = expect(p, p[L_FOREARM])
        else:
            ok, detail = False, "no bone positions"
        checks.append((f"{name}: {detail}", ok))
    verdict("Pose demo arm retarget result", checks, [
        f"renderer: {h.get('glRenderer')}", f"frames={h.get('frames')} drawCalls={h.get('drawCalls')}",
    ])
    log(f"PASS  (output in {out})")


def add_pose_webcam_args(p):
    add_pose_args(p)
    p.add_argument("--fake-video", type=Path, default=WEB_DIR / "pose_demo_fake_input.y4m",
                   help="Y4M played as the webcam; made from --source-video when missing")
    p.add_argument("--source-video", type=Path, default=WEB_DIR / "5940726-hd_1280_720_25fps.mp4")
    p.add_argument("--run-seconds", type=int, default=20)
    p.add_argument("--min-frames", type=int, default=5)
    p.add_argument("--boot-timeout-ms", type=int, default=60000)


def run_pose_webcam(args):
    """Fake webcam -> MediaPipe -> Kalidokit -> engine: is the chain live at all.

    The fake device plays a video of a person; the default synthetic pattern
    would never produce landmarks.
    """
    bundle = find_bundle(args.bundle, Path(args.page).stem)
    out = fresh_dir(args.out)
    video = _fake_video(args)
    browser = browser_profile(SWIFTSHADER + [
        "--use-fake-device-for-media-stream", "--use-fake-ui-for-media-stream",
        f"--use-file-for-fake-video-capture={video}"])
    install_playwright()
    with BundleServer(bundle) as server:
        result = run_driver("smoke.mjs", {
            "url": f"{server.url}/{args.page}?dummy_plug", "browser": browser,
            "viewport": viewport("1280x720"), "init_scripts": ["instrument.js"],
            "permissions": ["camera", "microphone"],
            "boot_timeout_ms": args.boot_timeout_ms, "min_frames": args.min_frames,
            "run_seconds": args.run_seconds,
            "counters": [{"label": "payloads", "re": r"^\[pose_demo\] sending"},
                         {"label": "retargets", "re": r"apply_pose: retargeting"}],
            "screenshot": {"file": "pose_demo_kalidokit_smoke.jpg", "type": "jpeg", "quality": 80},
            "video": None,
        }, out)

    h = result.get("harness") or {}
    counts = result.get("counts") or {}
    for sample in (result.get("samples") or {}).get("payloads", []):
        log(f"  sample: {sample}")
    verdict("Pose demo webcam result", [
        ("driver completed", not result.get("error")),
        ("WebGL context created", bool(h.get("contextType"))),
        (f">= {args.min_frames} frames rendered (got {h.get('frames', 0)})",
         h.get("frames", 0) >= args.min_frames),
        ("draw calls issued", h.get("drawCalls", 0) > 0),
        ("no wasm abort/trap", not result.get("fatal")),
        ("no WebGL context loss", not h.get("contextLost")),
        ("did not time out booting", not result.get("bootTimedOut")),
        (f"pose payloads sent from JS (got {counts.get('payloads', 0)})", counts.get("payloads", 0) > 0),
    ], [
        f"renderer: {h.get('glRenderer')}",
        # 0 is fine: the biped may not be loaded yet; this test is about the JS side
        f"C++ retarget lines: {counts.get('retargets', 0)} (informational)",
    ])
    log(f"PASS  (output in {out})")


def _fake_video(args):
    if args.fake_video.is_file():
        return args.fake_video.resolve()
    dest = args.cache / "pose_demo_fake_input.y4m"
    if dest.is_file():
        return dest
    if not args.source_video.is_file() or not shutil.which("ffmpeg"):
        raise SetupError(f"no {args.fake_video}, and no {args.source_video} + ffmpeg to make it")
    dest.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(["ffmpeg", "-loglevel", "error", "-i", args.source_video, "-vf", "scale=640:360",
                    "-pix_fmt", "yuv420p", "-r", "25", dest], check=True)
    return dest


# -- webrtc ----------------------------------------------------------------------

# In connect-flow order. Connected alone is not enough: GNS can connect while
# server-side dispatch is broken, so the round trip must end in roster sync.
CLIENT_MARKERS = [
    ("WebRTC transport connected",
     r"P2P WebRTC vport \d+\]\s*connected|-> k_ESteamNetworkingConnectionState_Connected\b"),
    ("GameJoin received (map load)", r"Loading map .* as requested by server"),
    ("join confirmation received", r"Received join confirmation, player_id=\d+"),
    ("player roster received", r"Player roster received:"),
]
HOST_READY_MARKERS = [
    ("host started (WebRTC-hosted server)", r"Starting WebRTC-hosted server: gateway="),
    ("host registration active", r"registration active for serverId="),
]
# What separates a working host from a page that registered and idled
HOST_MARKERS = HOST_READY_MARKERS + [("host accepted a client", r"Player joined:")]


def markers(pairs):
    return [{"label": label, "re": re_} for label, re_ in pairs]


def add_wasm_client_args(parser):
    parser.add_argument("bundle", nargs="?", type=Path, help="bundle, or a directory to search")
    parser.add_argument("--boot-timeout-ms", type=int, default=60000)
    parser.add_argument("--connect-timeout-ms", type=int, default=90000)
    parser.add_argument("--viewport", help="client page size (default 640x480, host size for a wasm host)")
    parser.add_argument("--server-plug", default=SERVER_PLUG)
    add_timeouts(parser, boot=60, run=150)


def run_wasm_client(args, bundle, out, server_url, map_path, host=None):
    """Drives the client page (and the host page, for a wasm host)."""
    # The page fetches maps by bare name, the boot map and any it switches to
    files = {m.name: m for m in map_path.parent.glob("*.map")}
    with BundleServer(bundle, files=files) as server:
        page = f"{server.url}/BlamGraphics.html"
        cfg = {
            "browser": browser_profile(SWIFTSHADER + WEBRTC),
            "boot_timeout_ms": args.boot_timeout_ms,
            "connect_timeout_ms": args.connect_timeout_ms,
            # The id after # stays out of Referer headers and the page host's logs
            "client": {"url": f"{page}#server={q(server_url)}",
                       "viewport": viewport(args.viewport or ("160x120" if host else "640x480")),
                       "markers": markers(CLIENT_MARKERS)},
            "host": None,
        }
        if host:
            # The URL shim only makes --key value pairs, hence map= not a positional
            cfg["host"] = {
                "url": f"{page}?listen={q(host['url'])}&map={q(map_path.name)}",
                "viewport": viewport(host["viewport"]),
                "markers": markers(HOST_MARKERS),
                "ready_markers": [label for label, _ in HOST_READY_MARKERS],
                "ready_timeout_ms": host["ready_timeout_ms"],
                "warm": host["warm"],
            }
            cfg["stall_warn_ms"] = host["warm"]["gap_ms"]
        result = run_driver("webrtc_client.mjs", cfg, out / "client", log_name="console.log")

    checks = []
    for role, expected in (("host", HOST_MARKERS if host else []), ("client", CLIENT_MARKERS)):
        state = result.get(role) or {}
        for label, _ in expected:
            checks.append((f"[{role}] {label}", label in state.get("found", [])))
        if state:
            checks.append((f"[{role}] no wasm abort/trap", not state.get("fatal")))
            checks.append((f"[{role}] page loaded in time", not state.get("timedOut")))
    stalls = [f"{r} worst frame-loop stall: {(result[r] or {}).get('worstStallMs', 0) / 1000:.1f}s"
              for r in ("host", "client") if result.get(r)]
    if result.get("error"):
        checks.append((result["error"].splitlines()[0], False))
    return checks, stalls


def add_web_client_args(parser):
    add_wasm_client_args(parser)
    add_gateway_args(parser)
    parser.add_argument("--server-role", choices=("native", "wasm"), default="native",
                        help="wasm: a second page hosts, the gateway bridges two DataChannels")
    parser.add_argument("--gateway-url", help="use a running gateway instead of a local one")
    parser.add_argument("--host-ready-timeout-ms", type=int, default=180000)
    parser.add_argument("--host-viewport", default="160x120")
    parser.add_argument("--host-warm-frames", type=int, default=3)
    parser.add_argument("--host-warm-gap-ms", type=int, default=8000)
    parser.add_argument("--host-warm-timeout-ms", type=int, default=180000)


def run_web_client(args):
    bundle = find_bundle(args.bundle)
    map_path = eng.fetch_maps(args, [args.map])
    out = fresh_dir(args.out)
    native = args.server_role == "native"
    engine = eng.engine_from_args(args) if native else None
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

        server = host = None
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
            host = {"url": target, "viewport": args.host_viewport,
                    "ready_timeout_ms": args.host_ready_timeout_ms,
                    "warm": {"frames": args.host_warm_frames, "gap_ms": args.host_warm_gap_ms,
                             "timeout_ms": args.host_warm_timeout_ms}}

        checks, info = run_wasm_client(args, bundle, out, target, map_path, host)

        # The server's end_time is sized for a slow boot; don't wait it out
        if server and not server.alive():
            rc = server.wait()
            checks.append((f"server still up or exited cleanly (exit {rc})", rc == 0))
        if not all(ok for _, ok in checks):
            if gateway:
                dump("gateway.log", gateway.log_path)
            if server:
                dump_matching("server.log", server.log_path, SERVER_LINES)
        verdict("WebRTC client result", checks, info)
    log(f"PASS  (logs in {out})")


# -- nat-punch -------------------------------------------------------------------


def add_nat_punch_args(parser):
    add_wasm_client_args(parser)
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

        checks, info = run_wasm_client(args, bundle, out, f"{gw_url}#{args.server_id}", map_path)
        # Our side of the NAT: the gateway asked for a per-client relay punch
        marker = "relay punch started for"
        checks.append(("server punched a per-client relay port", log_has(server.log_path, marker)))
        dump_matching("server.log", server.log_path, SERVER_LINES + "|" + re.escape(marker))
        verdict("NAT punch result", checks, info)
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
    lines = [line for line in (res.stdout + res.stderr).replace("\r", "").splitlines()
             if re.match(r"(?i)^(HTTP/|server:|curl:)", line)]
    # The gateway URL is a secret; keep it out of the job log
    log(f"  runner probe of the upgrade: {' '.join(lines).replace(host, '<gateway>') or 'no response'}")
    for line in [line for line in text.splitlines() if "libdatachannel:" in line][:5]:
        log("  " + line.replace(host, "<gateway>"))
