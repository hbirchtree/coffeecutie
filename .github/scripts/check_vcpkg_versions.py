#!/usr/bin/env python3
"""
Compares pinned vcpkg port versions (as resolved by vcpkg.json +
vcpkg-configuration.json) against the latest version available on each
registry's default branch, and reports how many commits behind each
port is.

Reads GITHUB_TOKEN from the environment (optional, but strongly
recommended to avoid GitHub API rate limits).
"""
from __future__ import annotations

import json
import os
import sys
import time
import urllib.error
import urllib.request
from dataclasses import dataclass, field
from pathlib import Path
from urllib.parse import urlparse

REPO_ROOT = Path(__file__).resolve().parents[2]
VCPKG_JSON = REPO_ROOT / "vcpkg.json"
VCPKG_CONFIG_JSON = REPO_ROOT / "vcpkg-configuration.json"

GITHUB_TOKEN = os.environ.get("GITHUB_TOKEN", "")
MAX_COMMIT_PAGES = 10  # 100 commits/page -> looks back up to 1000 commits per port
API_ROOT = "https://api.github.com"
RAW_ROOT = "https://raw.githubusercontent.com"


def http_get(url: str, headers: dict | None = None, retries: int = 3) -> tuple[int, bytes]:
    req_headers = {"User-Agent": "vcpkg-version-check"}
    if headers:
        req_headers.update(headers)
    req = urllib.request.Request(url, headers=req_headers)
    for attempt in range(retries):
        try:
            with urllib.request.urlopen(req, timeout=30) as resp:
                return resp.status, resp.read()
        except urllib.error.HTTPError as e:
            if e.code == 404:
                return 404, b""
            if e.code in (403, 429) and attempt < retries - 1:
                # Likely rate-limited; back off and retry.
                time.sleep(2 * (attempt + 1))
                continue
            return e.code, e.read() if e.fp else b""
        except urllib.error.URLError:
            if attempt < retries - 1:
                time.sleep(2 * (attempt + 1))
                continue
            raise
    return 0, b""


def gh_api(path: str) -> dict | list | None:
    headers = {"Accept": "application/vnd.github+json"}
    if GITHUB_TOKEN:
        headers["Authorization"] = f"Bearer {GITHUB_TOKEN}"
    status, body = http_get(f"{API_ROOT}{path}", headers=headers)
    if status != 200:
        return None
    return json.loads(body)


def raw_file(owner: str, repo: str, ref: str, path: str) -> str | None:
    status, body = http_get(f"{RAW_ROOT}/{owner}/{repo}/{ref}/{path}")
    if status != 200:
        return None
    return body.decode("utf-8", errors="replace")


def owner_repo_from_url(repo_url: str) -> tuple[str, str]:
    parsed = urlparse(repo_url)
    parts = [p for p in parsed.path.split("/") if p]
    owner, repo = parts[0], parts[1]
    if repo.endswith(".git"):
        repo = repo[: -len(".git")]
    return owner, repo


@dataclass
class Registry:
    owner: str
    repo: str
    baseline: str
    packages: list[str]
    default_branch: str | None = None
    baseline_date: str | None = None

    @property
    def label(self) -> str:
        return f"{self.owner}/{self.repo}"

    def matches(self, name: str) -> bool:
        return "*" in self.packages or name in self.packages


@dataclass
class PortVersion:
    version: str | None
    port_version: int = 0

    def __str__(self) -> str:
        if self.version is None:
            return "?"
        if self.port_version:
            return f"{self.version}#{self.port_version}"
        return self.version


def parse_port_manifest(text: str) -> PortVersion:
    data = json.loads(text)
    version = (
        data.get("version")
        or data.get("version-semver")
        or data.get("version-date")
        or data.get("version-string")
    )
    return PortVersion(version=version, port_version=int(data.get("port-version", 0)))


def parse_port_control(text: str) -> PortVersion:
    version = None
    port_version = 0
    for line in text.splitlines():
        if line.lower().startswith("version:"):
            version = line.split(":", 1)[1].strip()
        elif line.lower().startswith("port-version:"):
            try:
                port_version = int(line.split(":", 1)[1].strip())
            except ValueError:
                pass
    return PortVersion(version=version, port_version=port_version)


def fetch_port_version(reg: Registry, name: str, ref: str) -> PortVersion | None:
    manifest = raw_file(reg.owner, reg.repo, ref, f"ports/{name}/vcpkg.json")
    if manifest is not None:
        try:
            return parse_port_manifest(manifest)
        except json.JSONDecodeError:
            pass
    control = raw_file(reg.owner, reg.repo, ref, f"ports/{name}/CONTROL")
    if control is not None:
        return parse_port_control(control)
    return None


def default_branch(reg: Registry) -> str:
    if reg.default_branch:
        return reg.default_branch
    info = gh_api(f"/repos/{reg.owner}/{reg.repo}")
    reg.default_branch = (info or {}).get("default_branch", "master")
    return reg.default_branch


def baseline_date(reg: Registry) -> str | None:
    if reg.baseline_date:
        return reg.baseline_date
    commit = gh_api(f"/repos/{reg.owner}/{reg.repo}/commits/{reg.baseline}")
    if not commit:
        return None
    reg.baseline_date = commit["commit"]["committer"]["date"]
    return reg.baseline_date


def commits_behind(reg: Registry, name: str) -> str:
    since = baseline_date(reg)
    if since is None:
        return "?"
    branch = default_branch(reg)
    path = f"ports/{name}"
    count = 0
    page = 1
    while page <= MAX_COMMIT_PAGES:
        commits = gh_api(
            f"/repos/{reg.owner}/{reg.repo}/commits"
            f"?path={path}&sha={branch}&since={since}&per_page=100&page={page}"
        )
        if commits is None:
            return "?"
        count += len(commits)
        if len(commits) < 100:
            return str(count)
        page += 1
    return f"{count}+"


def collect_dependency_names(vcpkg_json: dict) -> set[str]:
    names: set[str] = set()

    def add_from_list(deps: list) -> None:
        for dep in deps:
            if isinstance(dep, str):
                names.add(dep)
            elif isinstance(dep, dict) and "name" in dep:
                names.add(dep["name"])

    add_from_list(vcpkg_json.get("dependencies", []))
    for feature in vcpkg_json.get("features", {}).values():
        add_from_list(feature.get("dependencies", []))
    return names


def collect_overrides(vcpkg_json: dict) -> dict[str, PortVersion]:
    overrides = {}
    for entry in vcpkg_json.get("overrides", []):
        overrides[entry["name"]] = PortVersion(
            version=entry.get("version"), port_version=int(entry.get("port-version", 0))
        )
    return overrides


def is_port_dir(p: Path) -> bool:
    return (p / "vcpkg.json").is_file() or (p / "CONTROL").is_file()


def discover_local_overlay_ports(vcpkg_config: dict) -> dict[str, Path]:
    """
    Best-effort discovery of locally patched/overlay ports. These are wired
    in several ways across this repo: vcpkg-configuration.json's
    "overlay-ports" (which may point at a directory-of-ports OR directly at
    a single port directory), CMake triplets setting VCPKG_OVERLAY_PORTS to
    a whole ports directory, and CMakePresets.json pinning individual ports.
    Rather than parse every one of those mechanisms, scan the filesystem:
    any immediate child of a toolchain/vcpkg/ports* directory that itself
    contains a vcpkg.json or CONTROL is a local port definition.
    """
    local_ports: dict[str, Path] = {}

    candidate_dirs = set()
    for overlay_path in vcpkg_config.get("overlay-ports", []):
        candidate_dirs.add(REPO_ROOT / overlay_path)
    candidate_dirs.update(REPO_ROOT.glob("toolchain/vcpkg/ports*"))

    for d in candidate_dirs:
        if not d.is_dir():
            continue
        if is_port_dir(d):
            local_ports[d.name] = d
        else:
            for child in d.iterdir():
                if child.is_dir() and is_port_dir(child):
                    local_ports[child.name] = child
    return local_ports


def read_local_port_version(path: Path) -> PortVersion | None:
    manifest = path / "vcpkg.json"
    if manifest.is_file():
        try:
            return parse_port_manifest(manifest.read_text())
        except json.JSONDecodeError:
            pass
    control = path / "CONTROL"
    if control.is_file():
        return parse_port_control(control.read_text())
    return None


def build_registries(vcpkg_config: dict) -> list[Registry]:
    registries = []
    for entry in vcpkg_config.get("registries", []):
        if entry.get("kind") != "git":
            continue
        owner, repo = owner_repo_from_url(entry["repository"])
        registries.append(
            Registry(
                owner=owner,
                repo=repo,
                baseline=entry["baseline"],
                packages=entry.get("packages", []),
            )
        )
    return registries


def resolve_registry(name: str, registries: list[Registry]) -> Registry | None:
    # Explicit package lists take priority over "*" wildcard entries.
    for reg in registries:
        if "*" not in reg.packages and name in reg.packages:
            return reg
    for reg in registries:
        if "*" in reg.packages:
            return reg
    return None


def main() -> int:
    vcpkg_json = json.loads(VCPKG_JSON.read_text())
    vcpkg_config = json.loads(VCPKG_CONFIG_JSON.read_text())

    registries = build_registries(vcpkg_config)
    local_overlays = discover_local_overlay_ports(vcpkg_config)
    overrides = collect_overrides(vcpkg_json)
    declared_names = collect_dependency_names(vcpkg_json)
    # Include overlay-only ports (e.g. transitive deps like status-code/bullet2
    # that aren't listed directly in vcpkg.json) on a best-effort basis.
    dep_names = declared_names | set(overrides.keys()) | set(local_overlays.keys())

    rows = []
    skipped = []

    for name in sorted(dep_names):
        is_local = name in local_overlays
        reg = resolve_registry(name, registries)

        if is_local:
            pinned = read_local_port_version(local_overlays[name])
            if pinned is None:
                skipped.append((name, "local overlay port: could not parse version"))
                continue
            if reg is None:
                skipped.append((name, "local overlay port, no matching upstream registry"))
                continue
            latest = fetch_port_version(reg, name, default_branch(reg))
            behind = commits_behind(reg, name)
            source = "local overlay (patched)"
        else:
            if reg is None:
                skipped.append((name, "no matching registry"))
                continue
            pinned = overrides.get(name) or fetch_port_version(reg, name, reg.baseline)
            latest = fetch_port_version(reg, name, default_branch(reg))
            behind = commits_behind(reg, name)
            source = "override" if name in overrides else "registry"

        if pinned is None or latest is None:
            skipped.append((name, f"could not read port from {reg.label}"))
            continue

        status = "up to date" if str(pinned) == str(latest) else "OUTDATED"
        note = "" if name in declared_names or name in overrides else "not a direct dependency"
        rows.append(
            {
                "name": name,
                "registry": reg.label,
                "pinned": str(pinned),
                "latest": str(latest),
                "behind": behind,
                "status": status,
                "source": source,
                "note": note,
            }
        )

    rows.sort(key=lambda r: (r["status"] != "OUTDATED", r["name"]))
    main_rows = [r for r in rows if r["source"] != "local overlay (patched)"]
    overlay_rows = [r for r in rows if r["source"] == "local overlay (patched)"]

    lines = []
    lines.append(f"# vcpkg dependency freshness report")
    lines.append("")
    lines.append(
        f"Checked {len(rows)} package(s), skipped {len(skipped)}. "
        f"Generated against `vcpkg-configuration.json` baselines."
    )
    lines.append("")
    lines.append("## Registry dependencies")
    lines.append("")
    lines.append("| Package | Source | Registry | Pinned | Latest | Commits behind | Status |")
    lines.append("|---|---|---|---|---|---|---|")
    for r in main_rows:
        marker = "🔴" if r["status"] == "OUTDATED" else "🟢"
        lines.append(
            f"| {r['name']} | {r['source']} | {r['registry']} | `{r['pinned']}` | `{r['latest']}` | "
            f"{r['behind']} | {marker} {r['status']} |"
        )

    if overlay_rows:
        lines.append("")
        lines.append("## Local overlay ports")
        lines.append("")
        lines.append(
            "These ports are patched forks maintained in `toolchain/vcpkg/ports*` and are "
            "**not necessarily used by every build target** (wired in per-triplet/per-preset "
            "via `VCPKG_OVERLAY_PORTS`, or globally via `vcpkg-configuration.json`). Comparison "
            "against upstream is best-effort — it does not account for local patches, only the "
            "base version being forked from."
        )
        lines.append("")
        lines.append("| Package | Upstream registry | Pinned (local) | Latest (upstream) | Commits behind | Status | Note |")
        lines.append("|---|---|---|---|---|---|---|")
        for r in overlay_rows:
            marker = "🔴" if r["status"] == "OUTDATED" else "🟢"
            lines.append(
                f"| {r['name']} | {r['registry']} | `{r['pinned']}` | `{r['latest']}` | "
                f"{r['behind']} | {marker} {r['status']} | {r['note']} |"
            )

    if skipped:
        lines.append("")
        lines.append("<details><summary>Skipped packages</summary>")
        lines.append("")
        lines.append("| Package | Reason |")
        lines.append("|---|---|")
        for name, reason in skipped:
            lines.append(f"| {name} | {reason} |")
        lines.append("")
        lines.append("</details>")

    report = "\n".join(lines)
    print(report)

    summary_path = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary_path:
        with open(summary_path, "a", encoding="utf-8") as f:
            f.write(report + "\n")

    outdated_count = sum(1 for r in rows if r["status"] == "OUTDATED")
    print(f"\n{outdated_count} package(s) outdated.", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
