#!/usr/bin/env python3
"""
building.py — Python replacement for toolchain/building.sh

Each build is represented as a BuildPlan: an ordered list of Steps that can
be printed (--list-steps) or executed.  Pass --dry-run to rehearse without
running any commands.
"""

from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
import sys
import platform as _platform
from contextlib import contextmanager
from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable


# ---------------------------------------------------------------------------
# Host detection
# ---------------------------------------------------------------------------

@dataclass
class HostInfo:
    os: str    # "linux" | "darwin"
    arch: str  # "x86_64" | "aarch64"

    @property
    def triplet(self) -> str:
        """e.g. x64-linux-native or x64-osx"""
        a = "x64" if self.arch == "x86_64" else "arm64"
        return f"{a}-linux-native" if self.os == "linux" else f"{a}-osx"

    @property
    def triplet_(self) -> str:
        """Triplet without the -native suffix, e.g. x64-linux or x64-osx"""
        a = "x64" if self.arch == "x86_64" else "arm64"
        return f"{a}-linux" if self.os == "linux" else f"{a}-osx"

    @staticmethod
    def detect() -> "HostInfo":
        sys_name = _platform.system().lower()
        machine = _platform.machine()
        if sys_name not in ("linux", "darwin"):
            sys.exit(f"::error::Unsupported host OS: {sys_name}")
        if machine not in ("x86_64", "aarch64", "arm64"):
            sys.exit(f"::error::Unsupported host architecture: {machine}")
        return HostInfo(
            os=sys_name,
            arch="aarch64" if machine == "arm64" else machine,
        )


# ---------------------------------------------------------------------------
# Target specification
# ---------------------------------------------------------------------------

@dataclass
class TargetSpec:
    platform: str
    architecture: str
    sysroot: str
    build_mode: str = "dbg"        # dbg | rel | reldeb
    cmake_target: str | None = None

    @staticmethod
    def parse(spec: str) -> "TargetSpec":
        """
        Parse 'platform:arch:sysroot[:mode][/cmake_target]'

        Examples:
          desktop:x86_64-buildroot-linux-gnu:multi
          desktop:x86_64-buildroot-linux-gnu:multi:rel/game
          web:wasm32:emscripten
        """
        cmake_target = None
        if "/" in spec:
            spec, cmake_target = spec.split("/", 1)
        parts = spec.split(":")
        if len(parts) < 3:
            sys.exit(f"::error::Invalid target '{spec}': expected platform:arch:sysroot")
        return TargetSpec(
            platform=parts[0],
            architecture=parts[1],
            sysroot=parts[2],
            build_mode=parts[3] if len(parts) > 3 else "dbg",
            cmake_target=cmake_target,
        )

    @property
    def preset(self) -> str:
        return f"{self.platform}-{self.architecture}-{self.sysroot}"

    @property
    def build_preset(self) -> str:
        mode = "rel" if self.build_mode == "reldeb" else self.build_mode
        return f"{self.preset}-{mode}"

    @property
    def cmake_build_type(self) -> str:
        return {"rel": "Release", "reldeb": "RelWithDebInfo"}.get(self.build_mode, "Debug")

    @property
    def is_linux(self) -> bool:
        return "linux" in self.architecture

    @property
    def is_windows(self) -> bool:
        return "windows" in self.architecture

    @property
    def is_macos(self) -> bool:
        return "osx" in self.architecture

    @property
    def is_downloadable(self) -> bool:
        return any(x in self.architecture for x in ("linux", "powerpc", "mingw32"))


# ---------------------------------------------------------------------------
# Step types
# ---------------------------------------------------------------------------

@dataclass
class Step:
    """A program invocation with explicit command, arguments, and environment."""
    name: str
    cmd: list[str]
    env: dict[str, str] = field(default_factory=dict)
    cwd: Path | None = None
    description: str = ""
    skip_if: Callable[[], bool] | None = None
    skip_reason: str = ""

    def display(self, index: int) -> None:
        desc = f" — {self.description}" if self.description else ""
        print(f"  [{index}] {self.name}{desc}")
        print(f"       CMD  : {' '.join(self.cmd)}")
        if self.env:
            items = list(self.env.items())
            print(f"       ENV  : {items[0][0]}={items[0][1]}")
            for k, v in items[1:]:
                print(f"              {k}={v}")
        if self.cwd:
            print(f"       CWD  : {self.cwd}")
        if self.skip_reason:
            print(f"       SKIP : {self.skip_reason}")


@dataclass
class SourceEnvStep:
    """Sources a shell script and merges its exports into the running environment."""
    name: str
    script: Path
    description: str = ""

    def display(self, index: int) -> None:
        desc = f" — {self.description}" if self.description else ""
        print(f"  [{index}] {self.name}{desc}")
        print(f"       SOURCE: {self.script}")


@dataclass
class PythonStep:
    """Runs a Python callable — for OS operations (mkdir, symlink, download, etc.)."""
    name: str
    fn: Callable[[], None]
    description: str = ""
    skip_if: Callable[[], bool] | None = None
    skip_reason: str = ""

    def display(self, index: int) -> None:
        desc = f" — {self.description}" if self.description else ""
        print(f"  [{index}] {self.name}{desc}")
        if self.skip_reason:
            print(f"       SKIP : {self.skip_reason}")


AnyStep = Step | SourceEnvStep | PythonStep


# ---------------------------------------------------------------------------
# BuildPlan
# ---------------------------------------------------------------------------

class BuildPlan:
    def __init__(self, name: str):
        self.name = name
        self.steps: list[AnyStep] = []

    def add(self, step: AnyStep) -> "BuildPlan":
        self.steps.append(step)
        return self

    def extend(self, other: "BuildPlan") -> "BuildPlan":
        self.steps.extend(other.steps)
        return self

    def print_steps(self) -> None:
        print(f"\nBuildPlan: {self.name}\n")
        for i, step in enumerate(self.steps, 1):
            step.display(i)
            print()

    def execute(self, dry_run: bool = False) -> None:
        Runner().run_plan(self, dry_run=dry_run)


# ---------------------------------------------------------------------------
# Runner
# ---------------------------------------------------------------------------

class Runner:
    def __init__(self, base_env: dict[str, str] | None = None):
        self.env: dict[str, str] = dict(base_env if base_env is not None else os.environ)

    def run_plan(self, plan: BuildPlan, dry_run: bool = False) -> None:
        if dry_run:
            plan.print_steps()
            return
        for step in plan.steps:
            # Evaluate skip_if once so the result can be used for both the
            # group title and the execution branch below.
            skipped = (
                not isinstance(step, SourceEnvStep)
                and step.skip_if is not None
                and step.skip_if()
            )

            if isinstance(step, SourceEnvStep):
                title = f"[source] {step.name}"
            elif isinstance(step, PythonStep):
                title = (
                    f"[skip]   {step.name}: {step.skip_reason}"
                    if skipped
                    else f"[python] {step.name}: {step.description}"
                )
            else:
                title = (
                    f"[skip]   {step.name}: {step.skip_reason}"
                    if skipped
                    else f"[run]    {step.name}: {' '.join(step.cmd)}"
                )

            with _ci_group(title):
                _banner(title)
                if isinstance(step, SourceEnvStep):
                    self._absorb_shell_env(step.script)
                elif isinstance(step, PythonStep):
                    if not skipped:
                        step.fn()
                else:
                    if skipped:
                        # Env side-effects still apply so later steps see the
                        # variables even when the step itself was skipped.
                        self.env.update(step.env)
                    else:
                        merged = {**self.env, **step.env}
                        subprocess.run(
                            step.cmd,
                            env=merged,
                            cwd=str(step.cwd or Path.cwd()),
                            check=True,
                        )
                        self.env.update(step.env)

    def _absorb_shell_env(self, script: Path) -> None:
        """Source a shell script and merge its environment into self.env."""
        result = subprocess.run(
            ["bash", "-c", f"source {script} && env -0"],
            capture_output=True,
            text=True,
            check=True,
        )
        for item in result.stdout.split("\0"):
            if "=" in item:
                k, v = item.split("=", 1)
                self.env[k] = v


def _banner(msg: str) -> None:
    print(f"\n:: {msg}", flush=True)


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def get_preset_value(
    base_dir: Path,
    preset: str,
    variable: str,
    extra_env: dict[str, str] | None = None,
) -> str:
    """Query a CMake preset for the value of an environment or cache variable.

    extra_env is merged on top of os.environ before invoking cmake.
    HOST_TOOLCHAIN_TRIPLET must be present for presets that include
    platform-specific sub-files.
    """
    env = {**os.environ, **(extra_env or {})}
    result = subprocess.run(
        ["cmake", "-S", str(base_dir), "--log-level", "DEBUG", "-N", "--preset", preset],
        capture_output=True,
        text=True,
        env=env,
    )
    for line in result.stdout.splitlines():
        if variable in line:
            parts = line.split('"')
            if len(parts) >= 2:
                return parts[1]
    return ""


def load_build_json(base_dir: Path) -> dict:
    path = base_dir / ".build.json"
    return json.loads(path.read_text()) if path.exists() else {}


def require_tool(name: str) -> str:
    path = shutil.which(name)
    if not path:
        sys.exit(f"::error::Required tool not found in PATH: {name}")
    return path


def check_programs(*names: str) -> None:
    """Check that all named programs exist on PATH.

    Reports every missing program in one message rather than failing on the
    first, so the user can install everything in one go.
    """
    missing = [n for n in names if shutil.which(n) is None]
    if missing:
        lines = "\n".join(f"  - {m}" for m in missing)
        sys.exit(f"::error::Required programs not found in PATH:\n{lines}")


def resolve_vcpkg_root() -> str:
    vcpkg = shutil.which("vcpkg")
    if not vcpkg:
        sys.exit("::error::vcpkg not found in PATH")
    return str(Path(os.path.realpath(vcpkg)).parent)


def host_tools_dir(host: HostInfo, base_dir: Path) -> Path:
    """Path to the vcpkg-installed tools directory for host tools."""
    if host.os == "darwin":
        return (
            base_dir
            / f"multi_build/{host.triplet}/vcpkg_installed/{host.triplet}/tools"
        )
    return base_dir / "multi_build/x64-linux-native/vcpkg_installed/x64-linux/tools"


def _is_ci() -> bool:
    return os.environ.get("CI", "").lower() in ("true", "1")


def _fmt_bytes(n: int) -> str:
    v = float(n)
    for unit in ("B", "KB", "MB", "GB"):
        if v < 1024:
            return f"{v:.1f} {unit}"
        v /= 1024
    return f"{v:.1f} TB"


def _dir_stats(path: Path) -> tuple[int, int]:
    """Return (file_count, total_bytes) for all files under path."""
    files = [f for f in path.rglob("*") if f.is_file()]
    return len(files), sum(f.stat().st_size for f in files)


@contextmanager
def _ci_group(title: str):
    if _is_ci():
        print(f"::group::{title}", flush=True)
    try:
        yield
    finally:
        if _is_ci():
            print("::endgroup::", flush=True)


# ---------------------------------------------------------------------------
# Plan: download host tools (clang-format, clang-tidy static binaries)
# ---------------------------------------------------------------------------

def download_host_tools_plan(host: HostInfo, base_dir: Path) -> BuildPlan:
    plan = BuildPlan("download-host-tools")
    bin_dir = base_dir / "multi_build/compilers/bin"
    clang_ver = 17

    suffix_map = {
        "x64-linux-native": "linux-amd64",
        "x64-osx": "macosx-amd64",
    }
    tool_suffix = suffix_map.get(host.triplet, "")

    plan.add(PythonStep(
        name="mkdir-compilers-bin",
        fn=lambda: bin_dir.mkdir(parents=True, exist_ok=True),
        description=f"Create {bin_dir}",
    ))

    base_url = (
        "https://github.com/muttleyxd/clang-tools-static-binaries"
        "/releases/download/master-f7f02c1d"
    )
    for tool in ("format", "tidy"):
        tool_path = bin_dir / f"clang-{tool}"
        url = f"{base_url}/clang-{tool}-{clang_ver}_{tool_suffix}"
        plan.add(Step(
            name=f"download-clang-{tool}",
            cmd=["wget", url, "-q", "-O", str(tool_path)],
            description=f"Download clang-{tool} v{clang_ver} ({tool_suffix})",
            skip_if=lambda p=tool_path: p.exists(),
            skip_reason=f"{tool_path} already present",
        ))
        plan.add(PythonStep(
            name=f"chmod-clang-{tool}",
            fn=lambda p=tool_path: p.chmod(p.stat().st_mode | 0o111),
            description=f"Make {tool_path} executable",
            skip_if=lambda p=tool_path: not p.exists(),
            skip_reason="tool was not downloaded",
        ))

    return plan


# ---------------------------------------------------------------------------
# Plan: build host tools
# ---------------------------------------------------------------------------

def host_tools_plan(host: HostInfo, base_dir: Path) -> BuildPlan:
    plan = BuildPlan(f"host-tools — {host.triplet}")

    if os.environ.get("BUILD_HOST_TOOLS", "1") != "1":
        if _is_ci():
            build_dir = base_dir / f"multi_build/host-{host.triplet}"

            def trim_host_build(d=build_dir):
                obj_files = [f for s in ("*.o", "*.a") for f in d.rglob(s)]
                obj_bytes = sum(f.stat().st_size for f in obj_files)
                for f in obj_files:
                    f.unlink(missing_ok=True)
                print(f"  Removed {len(obj_files)} object files ({_fmt_bytes(obj_bytes)} freed)")

                bins = [f for f in (d / "bin").iterdir() if f.is_file()]
                before = sum(f.stat().st_size for f in bins)
                for f in bins:
                    subprocess.run(["strip", str(f)], check=True)
                after = sum(f.stat().st_size for f in bins)
                print(f"  Stripped {len(bins)} binaries ({_fmt_bytes(before - after)} freed)")

            plan.add(PythonStep(
                name="trim-host-toolchain",
                fn=trim_host_build,
                description="Strip .o/.a files and binaries from host build dir",
            ))
        return plan

    ninja = require_tool("ninja")
    vcpkg_root = resolve_vcpkg_root()
    host_env = {"NINJA": ninja, "VCPKG_ROOT": vcpkg_root}

    plan.add(Step(
        name="host-tools-configure",
        cmd=["cmake", "--preset", f"host-{host.triplet}"],
        env=host_env,
        cwd=base_dir,
        description=f"Configure host tools (preset host-{host.triplet})",
    ))
    plan.add(Step(
        name="host-tools-build",
        cmd=["cmake", "--build", "--preset", f"host-{host.triplet}-rel"],
        env=host_env,
        cwd=base_dir,
        description="Build host tools in release mode",
    ))

    if _is_ci():
        bin_dir = base_dir / f"multi_build/host-{host.triplet}/bin"

        def strip_host_binaries(d=bin_dir):
            bins = [f for f in d.iterdir() if f.is_file()]
            before = sum(f.stat().st_size for f in bins)
            for f in bins:
                subprocess.run(["strip", str(f)], check=True)
            after = sum(f.stat().st_size for f in bins)
            print(f"  Stripped {len(bins)} binaries ({_fmt_bytes(before - after)} freed)")

        plan.add(PythonStep(
            name="strip-host-binaries",
            fn=strip_host_binaries,
            description=f"Strip debug symbols from host binaries in {bin_dir}",
        ))
    else:
        plan.extend(download_host_tools_plan(host, base_dir))

    return plan


# ---------------------------------------------------------------------------
# Plan: toolchain download and extraction
# ---------------------------------------------------------------------------

def toolchain_download_plan(
    base_dir: Path,
    toolchain_root: Path,
    toolchain_ver: str,
    download_name: str,
    gh_repo: str,
) -> BuildPlan:
    plan = BuildPlan(f"toolchain-download — {download_name} @ {toolchain_ver}")

    meta_dir = base_dir / "multi_build/compilers/meta"
    registry_file = meta_dir / f"{toolchain_ver}.json"

    plan.add(PythonStep(
        name="mkdir-meta",
        fn=lambda: meta_dir.mkdir(parents=True, exist_ok=True),
        description=f"Create {meta_dir}",
    ))
    plan.add(Step(
        name="download-registry",
        cmd=[
            "gh", "release", "download",
            "-R", gh_repo,
            "-D", str(meta_dir),
            toolchain_ver,
            "-p", "registry.json",
        ],
        description=f"Fetch release registry for {toolchain_ver} from {gh_repo}",
        skip_if=lambda: registry_file.exists(),
        skip_reason=f"{registry_file.name} already cached",
    ))

    def rename_registry(meta=meta_dir, dst=registry_file):
        src = meta / "registry.json"
        if src.exists() and not dst.exists():
            src.rename(dst)

    plan.add(PythonStep(
        name="rename-registry",
        fn=rename_registry,
        description=f"Rename registry.json → {registry_file.name}",
        skip_if=lambda: registry_file.exists(),
        skip_reason=f"{registry_file.name} already exists",
    ))

    # The base name used to look up the entry in the registry is the
    # download_name with any trailing _<sysroot> suffix removed, matching the
    # shell's ${1%_*} expansion.
    base_name = download_name.rsplit("_", 1)[0]

    def download_and_extract(
        root=toolchain_root,
        reg=registry_file,
        repo=gh_repo,
        ver=toolchain_ver,
        bn=base_name,
    ):
        registry = json.loads(reg.read_text())
        entry = next((e for e in registry if e.get("name") == bn), None)
        if not entry:
            sys.exit(f"::error::No toolchain entry for '{bn}' in {reg}")

        compiler_fn = entry.get("compiler", "")
        manifest_fn = entry.get("manifest", "")
        if not compiler_fn:
            sys.exit(f"::error::No compiler file listed for '{bn}' in registry")

        root.mkdir(parents=True, exist_ok=True)

        print(f"  Downloading {compiler_fn}")
        subprocess.run(
            ["gh", "release", "download", "-R", repo, ver, "-p", compiler_fn],
            cwd=str(root), check=True,
        )
        (root / compiler_fn).rename(root / "compiler.tar.xz")

        if manifest_fn:
            print(f"  Downloading {manifest_fn}")
            subprocess.run(
                ["gh", "release", "download", "-R", repo, ver, "-p", manifest_fn],
                cwd=str(root), check=True,
            )
            manifest_path = root / "compiler.manifest"
            (root / manifest_fn).rename(manifest_path)
            print(manifest_path.read_text())

        print("  Extracting compiler.tar.xz")
        old_umask = os.umask(0o022)
        try:
            subprocess.run(
                [
                    "tar", "x",
                    "--exclude=*/sysroot/dev",
                    "--no-same-owner",
                    "--no-same-permissions",
                    "--file=compiler.tar.xz",
                ],
                cwd=str(root), check=True,
            )
        finally:
            os.umask(old_umask)

        subprocess.run(["chmod", "-R", "u+w", str(root.resolve())], check=True)

    plan.add(PythonStep(
        name="toolchain-download-extract",
        fn=download_and_extract,
        description=f"Download + extract {download_name} into {toolchain_root}",
        skip_if=lambda: toolchain_root.is_dir(),
        skip_reason=f"{toolchain_root} already exists",
    ))

    return plan


# ---------------------------------------------------------------------------
# Plan: cmake configure + build  (shared core of every build type)
# ---------------------------------------------------------------------------

def configure_and_build_plan(
    target: TargetSpec,
    base_dir: Path,
    env: dict[str, str],
) -> BuildPlan:
    plan = BuildPlan(f"configure-and-build — {target.preset}")

    if _is_ci():
        plan.add(PythonStep(
            name="pre-configure-space",
            fn=lambda: subprocess.run(
                ["bash", "-c", f"df -h {base_dir}/.. && du -hd2 {base_dir}"],
                check=True,
            ),
            description="Disk space summary before configure",
        ))

    plan.add(Step(
        name="configure",
        cmd=["cmake", "--preset", target.preset],
        env=env,
        cwd=base_dir,
        description=f"CMake configure — preset {target.preset}",
    ))

    if _is_ci():
        vcpkg_root = env.get("VCPKG_ROOT", os.environ.get("VCPKG_ROOT", ""))
        vcpkg_triplet = get_preset_value(base_dir, target.preset, "VCPKG_TARGET_TRIPLET")
        build_dir = base_dir / "multi_build" / target.preset

        def trim_vcpkg(vr=vcpkg_root, vt=vcpkg_triplet, bd=build_dir):
            total_files, total_bytes = 0, 0

            buildtrees = Path(vr) / "buildtrees"
            if buildtrees.exists():
                n, b = _dir_stats(buildtrees)
                total_files += n
                total_bytes += b
                shutil.rmtree(buildtrees, ignore_errors=True)

            for triplet in filter(None, (vt, "x64-linux")):
                debug_dir = bd / "vcpkg_installed" / triplet / "debug"
                if debug_dir.exists():
                    n, b = _dir_stats(debug_dir)
                    total_files += n
                    total_bytes += b
                    shutil.rmtree(debug_dir, ignore_errors=True)

            print(f"  Removed {total_files} files ({_fmt_bytes(total_bytes)} freed)")

        plan.add(PythonStep(
            name="trim-vcpkg",
            fn=trim_vcpkg,
            description="Remove vcpkg buildtrees and debug libraries to save space",
        ))
        plan.add(PythonStep(
            name="post-configure-space",
            fn=lambda: subprocess.run(
                ["bash", "-c", f"df -h {base_dir}/.. && du -hd2 {base_dir}"],
                check=True,
            ),
            description="Disk space summary after configure",
        ))

    build_cmd = ["cmake", "--build", "--preset", target.build_preset]
    if target.cmake_target:
        build_cmd += ["--target", target.cmake_target]

    plan.add(Step(
        name="build",
        cmd=build_cmd,
        env={"BUILD_TYPE": target.cmake_build_type},
        cwd=base_dir,
        description=f"CMake build — preset {target.build_preset}",
    ))

    return plan


# ---------------------------------------------------------------------------
# Helpers shared by plan factories
# ---------------------------------------------------------------------------

def _base_env(target: TargetSpec, host: HostInfo, base_dir: Path) -> dict[str, str]:
    """Common environment variables needed by every cmake configure step."""
    td = host_tools_dir(host, base_dir)
    return {
        "BUILD_TYPE": target.cmake_build_type,
        "NINJA": require_tool("ninja"),
        "VCPKG_ROOT": resolve_vcpkg_root(),
        "PROTOC_PROGRAM": str(td / "protobuf/protoc"),
        "GLSLANG_PROGRAM": str(td / "glslang/glslang"),
    }


# ---------------------------------------------------------------------------
# Plan: native  (desktop, beaglebone, raspberry, console, generic, …)
# ---------------------------------------------------------------------------

def native_plan(
    target: TargetSpec,
    host: HostInfo,
    base_dir: Path,
    build_host_tools: bool = True,
) -> BuildPlan:
    plan = BuildPlan(
        f"native — {target.platform}:{target.architecture}:{target.sysroot}"
    )

    if build_host_tools:
        plan.extend(host_tools_plan(host, base_dir))

    toolchain_required = (
        get_preset_value(base_dir, target.preset, "TOOLCHAIN_REQUIRED") == "1"
    )
    toolchain_prefix = target.architecture

    is_gamecube = (
        f"{target.platform}-{target.architecture}".startswith("console-powerpc-eabi")
    )
    if is_gamecube:
        toolchain_prefix = "powerpc-eabi"

    env = _base_env(target, host, base_dir)
    env["TOOLCHAIN_PREFIX"] = toolchain_prefix

    if not toolchain_required:
        toolchain_ver = get_preset_value(base_dir, target.preset, "TOOLCHAIN_VERSION")
        if not toolchain_ver:
            sys.exit(
                "No TOOLCHAIN_VERSION found in the preset.\n"
                "Add it to the cmake preset or set TOOLCHAIN_ROOT manually."
            )

        if is_gamecube:
            default_root = (
                base_dir / "multi_build/compilers/gamecube-powerpc-eabi" / toolchain_ver
            )
        else:
            default_root = (
                base_dir
                / "multi_build/compilers"
                / f"{target.platform}-{target.architecture}"
                / toolchain_ver
            )

        toolchain_root = Path(os.environ.get("TOOLCHAIN_ROOT", str(default_root)))
        toolchain_sysroot = toolchain_root / toolchain_prefix / "sysroot"

        env["TOOLCHAIN_ROOT"] = str(toolchain_root)
        env["TOOLCHAIN_SYSROOT"] = str(toolchain_sysroot)
        env["PATH"] = f"{os.environ.get('PATH', '')}:{toolchain_root / 'bin'}"

        if target.is_downloadable:
            download_name = f"{target.platform}-{target.architecture}_{target.sysroot}"
            if is_gamecube:
                download_name = "gamecube-powerpc-eabi"
            build_json = load_build_json(base_dir)
            gh_repo = build_json.get("toolchain", {}).get("git", {}).get("repo", "")
            plan.extend(
                toolchain_download_plan(
                    base_dir=base_dir,
                    toolchain_root=toolchain_root,
                    toolchain_ver=toolchain_ver,
                    download_name=download_name,
                    gh_repo=gh_repo,
                )
            )

        if target.is_linux:
            build_dir = base_dir / "multi_build" / target.preset
            sysroot_cap = toolchain_sysroot

            def setup_symlinks(bd=build_dir, sr=sysroot_cap):
                lib_dir = bd / "lib"
                lib_dir.mkdir(parents=True, exist_ok=True)
                for src in [
                    sr / "lib/libstdc++.so.6",
                    sr / "lib/libssp.so.0",
                    sr / "usr/lib/libbacktrace.so.0",
                ]:
                    dst = lib_dir / src.name
                    if dst.is_symlink() or dst.exists():
                        dst.unlink()
                    dst.symlink_to(src)

            plan.add(PythonStep(
                name="setup-stdlib-symlinks",
                fn=setup_symlinks,
                description=(
                    f"Symlink libstdc++/libssp/libbacktrace from sysroot into "
                    f"{build_dir}/lib/"
                ),
            ))

    plan.extend(configure_and_build_plan(target, base_dir, env))
    return plan


# ---------------------------------------------------------------------------
# Plan: Emscripten / WebAssembly
# ---------------------------------------------------------------------------

def emscripten_plan(
    target: TargetSpec,
    host: HostInfo,
    base_dir: Path,
) -> BuildPlan:
    plan = BuildPlan(
        f"emscripten — {target.platform}:{target.architecture}:{target.sysroot}"
    )
    plan.extend(host_tools_plan(host, base_dir))

    default_root = base_dir / "multi_build/compilers/emsdk"
    toolchain_root = Path(os.environ.get("TOOLCHAIN_ROOT", str(default_root)))

    build_json = load_build_json(base_dir)
    emsdk_version = (
        build_json.get("toolchain", {}).get("emsdk", {}).get("version", "latest")
    )

    patched_emscripten_cmake = (
        base_dir / "toolchain/cmake/Platform/Emscripten.cmake"
    )
    emsdk_cmake_dst = (
        toolchain_root
        / "upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
    )

    def clone_and_install_emsdk(root=toolchain_root, ver=emsdk_version):
        subprocess.run(
            [
                "git", "clone",
                "https://github.com/emscripten-core/emsdk.git",
                str(root),
            ],
            check=True,
        )
        subprocess.run([str(root / "emsdk"), "install", ver], check=True)
        subprocess.run([str(root / "emsdk"), "activate", ver], check=True)
        print("::info::Transplanting patched Emscripten.cmake into EMSDK")
        shutil.copy2(patched_emscripten_cmake, emsdk_cmake_dst)

    plan.add(PythonStep(
        name="emsdk-setup",
        fn=clone_and_install_emsdk,
        description=f"Clone and install emsdk {emsdk_version} into {toolchain_root}",
        skip_if=lambda: toolchain_root.is_dir(),
        skip_reason=f"{toolchain_root} already exists, using compiler on disk",
    ))

    plan.add(SourceEnvStep(
        name="emsdk-env",
        script=toolchain_root / "emsdk_env.sh",
        description="Source emsdk_env.sh to configure the Emscripten environment",
    ))

    env = _base_env(target, host, base_dir)
    env.update({
        "TOOLCHAIN_PREFIX": target.architecture,
        "TOOLCHAIN_ROOT": str(toolchain_root),
        "EMSCRIPTEN": str(toolchain_root),
        "EMSDK": str(toolchain_root),
    })

    plan.extend(configure_and_build_plan(target, base_dir, env))
    return plan


# ---------------------------------------------------------------------------
# Plan: Xcode (macOS / iOS)
# ---------------------------------------------------------------------------

def xcode_plan(
    target: TargetSpec,
    host: HostInfo,
    base_dir: Path,
) -> BuildPlan:
    plan = BuildPlan(
        f"xcode — {target.platform}:{target.architecture}:{target.sysroot}"
    )
    plan.extend(host_tools_plan(host, base_dir))

    env = _base_env(target, host, base_dir)
    plan.extend(configure_and_build_plan(target, base_dir, env))
    return plan


# ---------------------------------------------------------------------------
# Plan: Android
# ---------------------------------------------------------------------------

def android_plan(
    target: TargetSpec,
    host: HostInfo,
    base_dir: Path,
) -> BuildPlan:
    plan = BuildPlan(
        f"android — {target.platform}:{target.architecture}:{target.sysroot}"
    )
    plan.extend(host_tools_plan(host, base_dir))

    default_sdk = base_dir / "multi_build/compilers/android/latest"
    android_sdk = Path(os.environ.get("ANDROID_SDK", str(default_sdk)))
    ndk_version = get_preset_value(base_dir, target.preset, "NDK_VERSION")

    sdktools_url = (
        "https://dl.google.com/android/repository"
        "/commandlinetools-linux-10406996_latest.zip"
    )
    ptools_url = (
        "https://dl.google.com/android/repository/platform-tools-latest-linux.zip"
    )

    def install_android_sdk(sdk=android_sdk, ndk_ver=ndk_version):
        parent = sdk.parent
        parent.mkdir(parents=True, exist_ok=True)

        for url, name in (
            (sdktools_url, "android-sdk-tools.zip"),
            (ptools_url, "android-platform-tools.zip"),
        ):
            subprocess.run(["wget", url, "-O", name], cwd=str(parent), check=True)

        for archive in ("android-sdk-tools.zip", "android-platform-tools.zip"):
            subprocess.run(["unzip", archive], cwd=str(parent), check=True)

        (parent / "latest/cmdline-tools").mkdir(parents=True, exist_ok=True)
        (parent / "cmdline-tools").rename(parent / "latest/cmdline-tools/latest")
        (parent / "platform-tools").rename(parent / "latest/platform-tools")

        sdkmanager = sdk / "cmdline-tools/latest/bin/sdkmanager"
        ndk_versions = [ndk_ver] if _is_ci() else ["25.2.9519653", "29.0.14206865"]
        packages = (
            ["build-tools;35.0.0"]
            + [f"ndk;{v}" for v in ndk_versions]
            + ["platforms;android-19", "platforms;android-33", "platforms;android-35"]
        )
        subprocess.run(
            [str(sdkmanager), "--install"] + packages,
            input="\n".join(["y"] * 14) + "\n",
            text=True,
            cwd=str(sdk),
            check=True,
        )

    plan.add(PythonStep(
        name="android-sdk-setup",
        fn=install_android_sdk,
        description=f"Download and install Android SDK + NDK {ndk_version}",
        skip_if=lambda: android_sdk.is_dir(),
        skip_reason=f"{android_sdk} already present",
    ))

    env = _base_env(target, host, base_dir)
    env.update({
        "ANDROID_SDK": str(android_sdk),
        "TOOLCHAIN_PREFIX": target.architecture,
        "TOOLCHAIN_ROOT": str(android_sdk / "ndk" / ndk_version),
    })

    plan.extend(configure_and_build_plan(target, base_dir, env))
    return plan


# ---------------------------------------------------------------------------
# Plan: MinGW / Windows cross-compilation
# ---------------------------------------------------------------------------

def mingw_plan(
    target: TargetSpec,
    host: HostInfo,
    base_dir: Path,
) -> BuildPlan:
    plan = BuildPlan(
        f"mingw — {target.platform}:{target.architecture}:{target.sysroot}"
    )
    plan.extend(host_tools_plan(host, base_dir))

    toolchain_ver = get_preset_value(base_dir, target.preset, "TOOLCHAIN_VERSION")
    if not toolchain_ver:
        sys.exit("No TOOLCHAIN_VERSION found in the preset for MinGW build.")

    default_root = (
        base_dir / "multi_build/compilers/mingw" / target.architecture / toolchain_ver
    )
    toolchain_root = Path(os.environ.get("TOOLCHAIN_ROOT", str(default_root)))

    if target.is_downloadable:
        download_name = f"windows-{target.architecture}_posix"
        build_json = load_build_json(base_dir)
        gh_repo = build_json.get("toolchain", {}).get("git", {}).get("repo", "")
        plan.extend(
            toolchain_download_plan(
                base_dir=base_dir,
                toolchain_root=toolchain_root,
                toolchain_ver=toolchain_ver,
                download_name=download_name,
                gh_repo=gh_repo,
            )
        )

    build_dir = base_dir / "multi_build" / target.preset
    plan.add(PythonStep(
        name="mkdir-build-dir",
        fn=lambda: build_dir.mkdir(parents=True, exist_ok=True),
        description=f"Create {build_dir}",
    ))

    env = _base_env(target, host, base_dir)
    env.update({
        "TOOLCHAIN_PREFIX": target.architecture,
        "TOOLCHAIN_ROOT": str(toolchain_root),
    })

    plan.extend(configure_and_build_plan(target, base_dir, env))
    return plan


# ---------------------------------------------------------------------------
# Build dispatcher
# ---------------------------------------------------------------------------

_NATIVE_PLATFORMS = frozenset(
    {"desktop", "beaglebone", "generic", "native", "raspberry", "console"}
)


def build_plan_for(target: TargetSpec, host: HostInfo, base_dir: Path) -> BuildPlan:
    """Select and construct the correct BuildPlan for the given target."""
    if "mingw32" in target.architecture:
        return mingw_plan(target, host, base_dir)
    if target.platform in _NATIVE_PLATFORMS:
        if host.os == "darwin" and target.platform == "desktop":
            return xcode_plan(target, host, base_dir)
        return native_plan(target, host, base_dir)
    if target.platform == "web":
        return emscripten_plan(target, host, base_dir)
    if target.platform in ("ios", "xcode"):
        return xcode_plan(target, host, base_dir)
    if target.platform == "android":
        return android_plan(target, host, base_dir)
    if target.platform == "windows":
        return mingw_plan(target, host, base_dir)
    sys.exit(f"::error::Unknown platform: {target.platform}")


# ---------------------------------------------------------------------------
# Subcommand helpers
# ---------------------------------------------------------------------------

def _source_dirs(base_dir: Path) -> list[Path]:
    return [base_dir / d for d in ("examples", "src", "tools") if (base_dir / d).exists()]


def _all_source_files(base_dir: Path) -> list[Path]:
    files: list[Path] = []
    for d in _source_dirs(base_dir):
        files.extend(d.rglob("*.cpp"))
        files.extend(d.rglob("*.h"))
    return files


def cmd_print_env(preset: str, base_dir: Path) -> None:
    """Print the environment variables for a cmake preset."""
    result = subprocess.run(
        ["cmake", "-N", "--log-level", "DEBUG", "--preset", preset],
        capture_output=True,
        text=True,
        cwd=str(base_dir),
    )
    in_env = False
    for line in result.stdout.splitlines():
        if "Preset environment variables" in line:
            in_env = True
            continue
        if in_env:
            stripped = line.strip()
            if not stripped:
                break
            print(stripped)
    print(f"NINJA={require_tool('ninja')}")
    print(f"VCPKG_ROOT={resolve_vcpkg_root()}")
    if re.match(r"^web-wasm", preset):
        emsdk = base_dir / "compilers/emsdk"
        print(f"EMSCRIPTEN={emsdk}")
        print(f"EMSDK={emsdk}")
        print(f"PATH={emsdk}/upstream/bin:{os.environ.get('PATH', '')}")


def _list_presets(base_dir: Path) -> None:
    result = subprocess.run(
        ["cmake", "-S", str(base_dir), "--list-presets"],
        capture_output=True,
        text=True,
    )
    # Convert cmake preset names  desktop-x86_64-buildroot-linux-gnu-multi
    # back to the platform:arch:sysroot form used by this tool.
    # The pattern captures  ([platform])-([everything-in-the-middle])-([sysroot])
    # where sysroot is the last lowercase-alphanumeric word.
    pat = re.compile(r'^\s+"([a-z][a-z0-9]*)-(.+)-([a-z0-9]+)"(\s+-.*)?$')
    rows: list[tuple[str, str]] = []
    for line in result.stdout.splitlines():
        m = pat.match(line)
        if m:
            platform_, arch, sysroot, desc = m.groups()
            name = f"{platform_}:{arch}:{sysroot}"
            rows.append((name, (desc or "").strip("- ").strip()))
    col = max((len(name) for name, _ in rows), default=0)
    for name, desc in rows:
        print(f"    {name:<{col}}  {desc}")


# ---------------------------------------------------------------------------
# Main entry point
# ---------------------------------------------------------------------------

def main() -> None:
    import argparse

    base_dir = Path(__file__).parent.parent.resolve()
    host = HostInfo.detect()

    # HOST_TOOLCHAIN_TRIPLET must be present in the environment before any
    # cmake preset query runs, because platform preset files include a
    # sub-file whose name is derived from this variable.
    os.environ.setdefault("HOST_TOOLCHAIN_TRIPLET", host.triplet)

    parser = argparse.ArgumentParser(
        prog="cb",
        description="coffeecutie build tool",
        formatter_class=argparse.RawTextHelpFormatter,
        add_help=True,
    )
    # Pre-scan for --dry-run / --list-steps so the flag is accepted anywhere
    # on the command line (before or after the subcommand name).
    _dry_run_flags = {"--dry-run", "--list-steps"}
    dry_run: bool = bool(_dry_run_flags & set(sys.argv[1:]))
    sys.argv = [a for a in sys.argv if a not in _dry_run_flags]

    sub = parser.add_subparsers(dest="command", metavar="<command>")

    # build
    p = sub.add_parser("build", help="Build a target  platform:arch:sysroot[:mode]")
    p.add_argument("target", help="e.g. desktop:x86_64-buildroot-linux-gnu:multi")

    # web-build
    p = sub.add_parser("web-build", help="Build a WebAssembly target")
    p.add_argument("target")

    # xcode-build / ios-build
    for name in ("xcode-build", "ios-build"):
        p = sub.add_parser(name, help="Build an Xcode (macOS/iOS) target")
        p.add_argument("target")

    # android-build
    p = sub.add_parser("android-build", help="Build an Android target")
    p.add_argument("target")

    # windows-build
    p = sub.add_parser("windows-build", help="Build a Windows (MinGW) target")
    p.add_argument("target")

    # console-build
    p = sub.add_parser("console-build", help="Build a console target")
    p.add_argument("target")

    # host-build
    sub.add_parser("host-build", help="Build the host tools only")

    # print-env
    p = sub.add_parser("print-env", help="Print environment variables for a preset")
    p.add_argument("preset")

    # env
    p = sub.add_parser("env", help="Spawn a shell inside a preset environment")
    p.add_argument("target")

    # format
    sub.add_parser("format", help="Format all C++ source files with clang-format")

    # lint-cmake
    sub.add_parser("lint-cmake", help="Lint CMake files with cmake-format")

    # get-notes
    sub.add_parser(
        "get-notes",
        help="Print TODO/NOTE/WARNING/BUG/FIXME comments from source",
    )

    # type-guards
    sub.add_parser(
        "type-guards",
        help="Find uses of deprecated [u]int[8|16|32|64] types",
    )

    # query-source
    p = sub.add_parser("query-source", help="Search source files with git grep")
    p.add_argument("pattern")

    args = parser.parse_args()
    cmd = args.command

    # --- build commands --------------------------------------------------

    if cmd in (
        "build",
        "web-build",
        "xcode-build",
        "ios-build",
        "android-build",
        "windows-build",
        "console-build",
    ):
        target = TargetSpec.parse(args.target)

        # Subcommand overrides the platform field so that e.g.
        # `cb web-build wasm32:emscripten:...` still routes correctly.
        _platform_override = {
            "web-build": "web",
            "xcode-build": "xcode",
            "ios-build": "ios",
            "android-build": "android",
            "windows-build": "windows",
            "console-build": "console",
        }
        if cmd in _platform_override:
            target.platform = _platform_override[cmd]

        # Core tools required for every build
        check_programs("cmake", "ninja", "vcpkg")

        # Platform-specific tools
        if target.platform == "web":
            # git: emsdk clone; node/nodejs supplied by emsdk itself
            check_programs("git")
        elif target.platform == "android":
            # wget + unzip: SDK download; java: sdkmanager requires a JRE
            check_programs("wget", "unzip", "java")
        elif target.platform in ("xcode", "ios"):
            # xcodebuild: cmake toolchain issues SEND_ERROR if missing
            check_programs("xcodebuild")
        elif target.is_linux:
            # Only require packaging tools when the preset actually enables them
            if get_preset_value(base_dir, target.preset, "GENERATE_APPIMAGE") == "ON":
                check_programs("mksquashfs")
            if get_preset_value(base_dir, target.preset, "GENERATE_FLATPAK") == "ON":
                check_programs("flatpak")
        if target.is_downloadable or "mingw32" in target.architecture:
            # gh: toolchain download via GitHub Releases
            check_programs("gh")

        plan = build_plan_for(target, host, base_dir)
        plan.execute(dry_run=dry_run)

    elif cmd == "host-build":
        check_programs("cmake", "ninja", "vcpkg")
        plan = host_tools_plan(host, base_dir)
        plan.execute(dry_run=dry_run)

    # --- env / introspection commands ------------------------------------

    elif cmd == "print-env":
        check_programs("cmake")
        preset = args.preset.replace(":", "-")
        cmd_print_env(preset, base_dir)

    elif cmd == "env":
        check_programs("cmake")
        preset = args.target.replace(":", "-")
        build_dir = base_dir / "multi_build" / preset
        build_dir.mkdir(parents=True, exist_ok=True)
        print(f"Entering env for preset {preset}")
        cmd_print_env(preset, base_dir)
        os.chdir(str(build_dir))
        shell = os.environ.get("SHELL", "bash")
        os.execlp(shell, shell)

    # --- code tool commands ----------------------------------------------

    elif cmd == "format":
        check_programs("wget")
        if not dry_run:
            download_host_tools_plan(host, base_dir).execute()
        clang_format = os.environ.get(
            "CLANG_FORMAT",
            str(base_dir / "multi_build/compilers/bin/clang-format"),
        )
        if dry_run:
            print(f"Would run: {clang_format} -i -style=file <each source file>")
        else:
            for f in _all_source_files(base_dir):
                subprocess.run([clang_format, "-i", "-style=file", str(f)], check=True)

    elif cmd == "lint-cmake":
        check_programs("cmake-format")
        lint_dirs = [
            str(base_dir / d)
            for d in ("examples", "src", "tests", "toolchain", "tools")
        ]
        if dry_run:
            print(f"Would run: cmake-format -i -c .cmake-format {' '.join(lint_dirs)}")
        else:
            print("::group::Linting CMake files")
            subprocess.run(
                ["cmake-format", "-i", "-c", str(base_dir / ".cmake-format")]
                + lint_dirs,
                check=True,
            )
            print("::endgroup::")

    elif cmd == "get-notes":
        check_programs("git")
        subprocess.run(
            [
                "git", "grep", "-E",
                r"(//|/\*)\s?(TODO|NOTE|WARNING|BUG|FIXME)",
            ],
            cwd=str(base_dir),
        )

    elif cmd == "type-guards":
        check_programs("git")
        for f in _all_source_files(base_dir):
            subprocess.run(
                ["git", "grep", "-E", r"(u|)int(8|16|32|64)", str(f), "/dev/null"],
                cwd=str(base_dir),
            )

    elif cmd == "query-source":
        check_programs("git")
        subprocess.run(
            ["git", "grep", "-E", args.pattern],
            cwd=str(base_dir),
        )

    # --- default: help + preset list -------------------------------------

    else:
        print(
            "usage: cb [--dry-run] <command> [args]\n"
            "\nBuild commands:\n"
            "    build               — Build a target: platform:arch:sysroot[:mode]\n"
            "    web-build           — Build a WebAssembly target (Emscripten)\n"
            "    xcode-build         — Build a macOS/iOS target (Xcode)\n"
            "    ios-build           — Alias for xcode-build\n"
            "    android-build       — Build an Android target\n"
            "    windows-build       — Build a Windows target (MinGW)\n"
            "    console-build       — Build a console/embedded target\n"
            "    host-build          — Build host tools only\n"
            "\nAvailable targets:"
        )
        _list_presets(base_dir)
        print(
            "\nUtility commands:\n"
            "    format              — Format C++ source files with clang-format\n"
            "    lint-cmake          — Lint CMake files with cmake-format\n"
            "    get-notes           — Print TODO/NOTE/WARNING/BUG/FIXME from source\n"
            "    type-guards         — Find deprecated [u]int[8|16|32|64] uses\n"
            "    query-source        — Search source files (git grep)\n"
            "    print-env           — Print resolved environment for a preset\n"
            "    env                 — Spawn a shell inside a preset environment\n"
            "\nFlags:\n"
            "    --dry-run, --list-steps\n"
            "                        — Print all steps with commands and env vars\n"
            "                          without executing anything\n"
        )


if __name__ == "__main__":
    main()
