#!/usr/bin/env python3
"""BlamGraphics integration test runner, for CI and local runs.

Locally the engine runs through `cb run` (build first, or pass --build);
in CI, --artifacts points at a downloaded build and the sysroot and maps
are fetched as needed. Exit code: 0 pass, 1 fail, 2 could not run.

  run.py dummy-plug dummy_plug_core_33.json --artifacts DIR --out DIR
  run.py net --route relay --out DIR
  run.py scenarios [NAME[@NETWORK]...] --out DIR
  run.py pinned-cert --out DIR
  run.py web-client [BUNDLE] --server-role wasm --out DIR
  run.py nat-punch [BUNDLE] --out DIR      (GATEWAY_URL in the environment)
  run.py webgl [BUNDLE] --map xbox_ex/beavercreek.map --out DIR
  run.py pose-arms [BUNDLE] --out DIR
  run.py pose-webcam [BUNDLE] --out DIR

Browser tests run Playwright drivers from .github/tests/web; BUNDLE is the
bundle itself or a directory to search (default: multi_build).
"""

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from harness import engine, native, web  # noqa: E402
from harness.common import (  # noqa: E402
    Proc,
    SetupError,
    TestFailure,
    install_signal_cleanup,
    log,
)


def main():
    parser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter
    )
    sub = parser.add_subparsers(dest="command", required=True)

    def command(name, func, help, out, add=None, engine_args=True):
        p = sub.add_parser(name, help=help)
        p.set_defaults(func=func)
        p.add_argument("--out", type=Path, default=Path(out), help="output directory")
        if engine_args:
            engine.add_engine_args(p)
        else:
            p.add_argument("--cache", type=Path, default=engine.default_cache())
        if add:
            add(p)
        return p

    p = command("dummy-plug", native.run_dummy_plug,
                "one process driven by a dummy plug config", "/tmp/blam_test")
    p.add_argument("plug", help="config path, or name under .github/tests[/net]")
    p.add_argument("--journal-check", metavar="SCENARIO",
                   help="compare the host journal with its spawned client0's")
    p.add_argument("--no-audio", action="store_true", help="drop rendered audio")
    p.add_argument("--run-timeout", type=int, default=1800)
    p.set_defaults(map="beavercreek.map")

    command("net", native.run_net, "server and client process, optionally via a gateway",
            "/tmp/net_test", native.add_net_args)

    p = command("scenarios", native.run_scenarios, "every client/server scenario",
                "/tmp/net_scenarios", native.add_net_args)
    p.add_argument("names", nargs="*", help="SCENARIO or SCENARIO@NETWORK")

    p = command("pinned-cert", native.run_pinned_cert,
                "server certificate pinned by its Ed25519 key", "/tmp/pinned_cert_test")
    p.add_argument("--port", type=int, default=19701)
    native.add_timeouts(p)

    command("web-client", web.run_web_client, "wasm client through a gateway",
            "/tmp/webrtc_web_client_test", web.add_web_client_args)
    command("nat-punch", web.run_nat_punch, "wasm client via a remote gateway, server behind NAT",
            "/tmp/webrtc_nat_punch_test", web.add_nat_punch_args)
    command("webgl", web.run_webgl, "wasm build boots and renders (SwiftShader)",
            "/tmp/webgl_test", web.add_webgl_args, engine_args=False)
    command("pose-arms", web.run_pose_arms, "pose demo arm retargeting, synthetic poses",
            "/tmp/pose_demo_arms", web.add_pose_arms_args, engine_args=False)
    command("pose-webcam", web.run_pose_webcam, "pose demo fake webcam -> Kalidokit chain",
            "/tmp/pose_demo_webcam", web.add_pose_webcam_args, engine_args=False)

    args = parser.parse_args()
    install_signal_cleanup()
    try:
        args.func(args)
    except TestFailure as e:
        log(f"FAIL: {e}")
        return 1
    except SetupError as e:
        log(f"ERROR: {e}")
        return 2
    finally:
        Proc.kill_all()
    return 0


if __name__ == "__main__":
    sys.exit(main())
