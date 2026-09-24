#!/usr/bin/env python3
"""Run include-what-you-use over a compile_commands.json and rank the results.

IWYU is clang-based; the desktop presets build with g++. This strips the
GCC-only flags that clang would reject, so a g++ database can be reused
without configuring a second build tree.

  ./toolchain/iwyu/run_iwyu.py -p multi_build/desktop-x64-linux-multi \
      --filter src/coffee/graphics

  ./toolchain/iwyu/run_iwyu.py -p <builddir> --filter <path> --fix
"""
import argparse
import concurrent.futures
import json
import os
import re
import shlex
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))

# Flags g++ accepts and clang does not. Extend as the toolchain moves.
GCC_ONLY = re.compile(
    r"^-(fno-lifetime-dse|fno-semantic-interposition|fconcepts.*|"
    r"fabi-version=.*|flto-partition=.*|fdevirtualize.*|fipa-.*|"
    r"mno-fma4|Wno-(maybe-uninitialized|psabi|class-memaccess|"
    r"stringop-overflow|dangling-reference|free-nonheap-object))$"
)
DROP_WITH_ARG = {"-o", "-c", "-include-pch", "-Xassembler"}


def iwyu_command(entry, mapping, extra):
    args = shlex.split(entry.get("command") or " ".join(entry["arguments"]))[1:]
    kept, skip = [], False
    for a in args:
        if skip:
            skip = False
            continue
        if a in DROP_WITH_ARG:
            skip = True
            continue
        if a.endswith((".cpp", ".cc", ".cxx", ".c", ".o")):
            continue
        if GCC_ONLY.match(a):
            continue
        kept.append(a)
    cmd = [IWYU]
    for m in mapping:
        cmd += ["-Xiwyu", "--mapping_file=" + m]
    cmd += ["-Xiwyu", "--no_fwd_decls", "-Xiwyu", "--max_line_length=100"]
    cmd += extra + kept + ["-c", entry["file"]]
    return cmd


REMOVE_RE = re.compile(r"^- #include\s+([<\"].*?[>\"])", re.M)


def analyse(entry, mapping, extra):
    cmd = iwyu_command(entry, mapping, extra)
    try:
        r = subprocess.run(
            cmd, capture_output=True, text=True, cwd=entry["directory"], timeout=900
        )
    except subprocess.TimeoutExpired:
        return entry["file"], None, "timeout", ""
    out = r.stderr
    if "fatal error:" in out or "error: " in out and "should remove" not in out:
        first = next(
            (l for l in out.splitlines() if "error:" in l), "unknown parse failure"
        )
        return entry["file"], None, first.strip()[:160], out
    removals = REMOVE_RE.findall(out)
    return entry["file"], removals, None, out


def main():
    p = argparse.ArgumentParser()
    p.add_argument("-p", "--build-dir", required=True,
                   help="directory holding compile_commands.json")
    p.add_argument("--filter", default="",
                   help="only analyse files whose path contains this")
    p.add_argument("-j", "--jobs", type=int, default=os.cpu_count())
    p.add_argument("--iwyu", default=shutil.which("include-what-you-use"))
    p.add_argument("--fix", action="store_true",
                   help="apply changes with fix_includes.py")
    p.add_argument("--headers", action="store_true",
                   help="with --fix, allow REMOVING includes from headers "
                        "(fix_includes defaults to --safe_headers, which only "
                        "adds to them). This is where the transitive graph "
                        "actually shrinks, and where a bad call hurts most")
    p.add_argument("--report", default="iwyu-report.txt")
    args = p.parse_args()

    global IWYU
    IWYU = args.iwyu
    if not IWYU:
        sys.exit("include-what-you-use not found; pass --iwyu")

    db_path = os.path.join(args.build_dir, "compile_commands.json")
    if not os.path.exists(db_path):
        sys.exit("no compile_commands.json in " + args.build_dir)
    db = [e for e in json.load(open(db_path)) if args.filter in e["file"]]
    if not db:
        sys.exit("no entries matched --filter " + args.filter)

    mapping = [os.path.join(HERE, "coffeecutie.imp")]
    extra = []
    print("%d translation units, %d jobs" % (len(db), args.jobs))

    results, failures, raw = [], [], []
    with concurrent.futures.ThreadPoolExecutor(args.jobs) as ex:
        futs = {ex.submit(analyse, e, mapping, extra): e for e in db}
        for n, f in enumerate(concurrent.futures.as_completed(futs), 1):
            path, removals, err, out = f.result()
            rel = os.path.relpath(path, ROOT)
            if err:
                failures.append((rel, err))
            else:
                results.append((len(removals), rel, removals))
                raw.append(out)
            print("  [%d/%d] %s" % (n, len(db), rel), file=sys.stderr)

    results.sort(reverse=True)
    with open(args.report, "w") as fh:
        fh.write("# IWYU: removable includes, most first\n\n")
        for count, rel, removals in results:
            if not count:
                continue
            fh.write("%-70s %d\n" % (rel, count))
            for inc in removals:
                fh.write("    - %s\n" % inc)
            fh.write("\n")
        if failures:
            fh.write("\n# did not parse (%d)\n\n" % len(failures))
            for rel, err in failures:
                fh.write("%-70s %s\n" % (rel, err))

    total = sum(c for c, _, _ in results)
    print("\n%d removable includes across %d files (%d failed to parse)"
          % (total, sum(1 for c, _, _ in results if c), len(failures)))
    print("report: " + args.report)

    if args.fix:
        fixer = shutil.which("fix_includes.py") or shutil.which("fix_include")
        if not fixer:
            sys.exit("fix_includes.py not found")
        # --noreorder: .clang-format owns include order, so leave it alone.
        fix_cmd = [fixer, "--noreorder"]
        if args.headers:
            fix_cmd.append("--nosafe_headers")
        proc = subprocess.run(fix_cmd, input="\n".join(raw), text=True)
        print("\nReview the edits, then run ./cb format.")
        sys.exit(proc.returncode)


if __name__ == "__main__":
    main()
