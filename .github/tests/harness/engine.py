"""How BlamGraphics is launched: a local build via `cb run`, or a CI artifact.

CI artifacts run through the loader of the cross-toolchain sysroot, so the
whole stack (libc, Mesa, EGL, the llvmpipe driver) comes from one consistent
place; the runner's own Mesa gives degenerate EGL configs.
"""

import os
import shutil
import stat
import subprocess
import urllib.request
from pathlib import Path

from .common import SRC_DIR, SetupError, group, log, run_checked

DEFAULT_TARGET = "desktop:x86_64-buildroot-linux-gnu:multi/BlamGraphics"
SYSROOT_REPO = "hbirchtree/coffeecutie-automation-tools"
SYSROOT_RELEASE = "v2.3"
SYSROOT_PATTERN = "desktop_x86_64-buildroot-linux-gnu_*_target.tar.xz"
MAP_SERVER = "https://maps.speen.dev"
LOCAL_MAP_ROOT = Path("/mnt/blam")
APPDIR = "blam_graphics.AppDir"


def default_cache():
    return Path(os.environ.get("RUNNER_TEMP", "/tmp")) / "blamtest-cache"


def add_engine_args(parser):
    g = parser.add_argument_group("engine")
    g.add_argument(
        "--artifacts",
        type=Path,
        help="downloaded CI build artifact to run (default: local build via cb run)",
    )
    g.add_argument("--target", default=DEFAULT_TARGET, help="cb run target")
    g.add_argument("--resource-dir", type=Path, help="assets directory")
    g.add_argument(
        "--sysroot", type=Path, help="extracted sysroot (fetched when omitted)"
    )
    g.add_argument(
        "--no-sysroot",
        action="store_true",
        help="run artifacts with the host loader and libraries",
    )
    g.add_argument("--build", action="store_true", help="cb build the target first")
    g.add_argument("--cache", type=Path, default=default_cache())
    g.add_argument("--map-dir", default="xbox_ex", help="map server directory")
    g.add_argument(
        "--map", default="bloodgulch.map", help="map name in --map-dir, or a path"
    )


class Engine:
    def __init__(self, argv, env, resource_dir, cwd=None):
        self.prefix = argv
        self.env = env
        self.resource_dir = resource_dir
        self.cwd = cwd

    def argv(self, map_path, *args):
        return [*self.prefix, self.resource_dir, map_path, *args]

    def environ(self, tmpdir, dummy_plug=None, **extra):
        env = dict(self.env)
        env["TMPDIR"] = str(tmpdir)
        if dummy_plug:
            env["DUMMY_PLUG_CONFIG"] = str(dummy_plug)
        env.update({k: str(v) for k, v in extra.items()})
        return env


def engine_from_args(args):
    base = dict(os.environ, COFFEE_DISABLE_PROFILER="1")
    base.pop("DUMMY_PLUG_CONFIG", None)
    if args.artifacts:
        return _artifact_engine(args, base)
    return _local_engine(args, base)


def _local_engine(args, env):
    if args.build:
        with group(f"Building {args.target}"):
            run_checked(
                [SRC_DIR / "cb", "build", args.target],
                "cb build",
                cwd=SRC_DIR,
                env=dict(env, BUILD_HOST_TOOLS="0"),
            )
    preset = args.target.split("/")[0].replace(":", "-")
    resource_dir = args.resource_dir or (
        SRC_DIR / "multi_build" / preset / "examples/blam/cblam-testing/assets"
    )
    return Engine(
        [SRC_DIR / "cb", "run", args.target, "--"],
        env,
        f"{resource_dir}/",
        cwd=SRC_DIR,
    )


def _find_appdir(artifacts):
    for appdir in sorted(Path(artifacts).rglob(APPDIR)):
        if (appdir / "BlamGraphics").is_file():
            return appdir.resolve()
    raise SetupError(f"no {APPDIR}/BlamGraphics under {artifacts}")


def _make_executable(path):
    path.chmod(path.stat().st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)


def _artifact_engine(args, env):
    appdir = _find_appdir(args.artifacts)
    binary = appdir / "BlamGraphics"
    # upload-artifact drops the executable bit
    for name in ("BlamGraphics", "AppRun", "CrashRecovery"):
        if (appdir / name).exists():
            _make_executable(appdir / name)

    lib_dirs = [appdir / "lib"]
    loader = None
    if not args.no_sysroot:
        sysroot = (args.sysroot or fetch_sysroot(args.cache)).resolve()
        loader = sysroot / "lib" / "ld-linux-x86-64.so.2"
        lib_dirs += [sysroot / "usr" / "lib", sysroot / "lib"]
        dri = sysroot / "usr" / "lib" / "dri"
        env.update(
            LIBGL_ALWAYS_SOFTWARE="1",
            GALLIUM_DRIVER="llvmpipe",
            LIBGL_DRIVERS_PATH=str(dri),
            MESA_LOADER_DRIVER_PATH=str(dri),
            LIBGL_DRI3_DISABLE="1",
            EGL_PLATFORM="surfaceless",
            # glvnd otherwise looks for the host's vendor files, absent on a bare runner
            __EGL_VENDOR_LIBRARY_DIRS=str(sysroot / "usr" / "share" / "glvnd" / "egl_vendor.d"),
        )
    lib_path = ":".join(str(d) for d in lib_dirs if d.is_dir())
    # Inherited by the dummy plug's re-exec of itself through the loader
    env["LD_LIBRARY_PATH"] = lib_path

    prefix = [loader, "--library-path", lib_path, binary] if loader else [binary]
    check_libraries(prefix, binary, env)
    resource_dir = args.resource_dir or appdir / "assets"
    return Engine(prefix, env, f"{resource_dir}/")


def check_libraries(prefix, binary, env):
    """Fails before any test runs if the binary has unresolved libraries."""
    if len(prefix) > 1:
        argv = [*prefix[:3], "--list", binary]
    else:
        argv = ["ldd", binary]
    res = subprocess.run(
        [str(a) for a in argv], env=env, capture_output=True, text=True
    )
    listing = res.stdout + res.stderr
    missing = sorted(
        {line.split()[0] for line in listing.splitlines() if "not found" in line}
    )
    if missing or res.returncode != 0:
        with group("Library resolution"):
            log(listing)
        raise SetupError(
            f"{binary.name} cannot resolve: {', '.join(missing) or 'see listing'}"
        )


def fetch_sysroot(cache):
    sysroot = cache / "sysroot"
    done = sysroot / ".extracted"
    if done.exists():
        return sysroot
    shutil.rmtree(sysroot, ignore_errors=True)
    sysroot.mkdir(parents=True)
    with group(f"Fetching sysroot {SYSROOT_RELEASE}"):
        run_checked(
            ["gh", "release", "-R", SYSROOT_REPO, "download", SYSROOT_RELEASE,
             "-p", SYSROOT_PATTERN, "-D", sysroot],
            "sysroot download",
        )
        for tarball in sysroot.glob("*.tar.xz"):
            run_checked(["tar", "xf", tarball, "-C", sysroot], "sysroot extract")
            tarball.unlink()
    done.touch()
    return sysroot


def fetch_maps(args, names):
    """Resolves map names to a path, downloading what isn't on disk.

    Every map lands in one directory, since a map switch loads by name from
    the directory of the current map.
    """
    if Path(args.map).is_file():
        return Path(args.map).resolve()
    names = list(dict.fromkeys(Path(n).name for n in names))
    return locate_maps(args.map_dir, names, args.cache) / names[0]


def locate_maps(map_dir, names, cache):
    """A directory holding all of names: /mnt/blam's, or the download cache."""
    names = list(names)
    if map_dir in ("pc", "custom"):
        names += ["bitmaps.map", "sounds.map"]
    local = LOCAL_MAP_ROOT / map_dir
    if all((local / n).is_file() for n in names):
        return local

    out = cache / "maps" / map_dir
    out.mkdir(parents=True, exist_ok=True)
    token = os.environ.get("MAP_ACCESS_TOKEN")
    for name in dict.fromkeys(names):
        dest = out / name
        if dest.is_file() and dest.stat().st_size > 0:
            continue
        if not token:
            raise SetupError(f"{name} is not in {local} and MAP_ACCESS_TOKEN is unset")
        log(f"Downloading {map_dir}/{name}")
        req = urllib.request.Request(
            f"{MAP_SERVER}/{map_dir}/{name}", headers={"Authorization": token}
        )
        tmp = dest.with_suffix(".part")
        with urllib.request.urlopen(req) as res, open(tmp, "wb") as f:
            shutil.copyfileobj(res, f)
        tmp.rename(dest)
    return out
