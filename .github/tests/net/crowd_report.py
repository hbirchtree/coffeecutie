#!/usr/bin/env python3
"""Traffic and remote-player smoothness from a crowd_plugs.py run, read from the
journals the host and its spawned clients wrote: per-connection traffic, how
often remote players freeze or jump between dumps, and how far each viewer
draws them from where their owner had them (after the best latency shift)."""
import json, math, sys, glob, statistics

root = sys.argv[1]


def records(path):
    for line in open(path):
        try:
            yield json.loads(line)
        except Exception:
            pass


def stats(path, label):
    rows = [r for r in records(path) if r.get("type") == "net_stats" or r.get("event") == "net_stats"
            or (isinstance(r.get("data"), dict) and "out_bytes_per_sec" in r["data"])]
    rows = [r.get("data", r) for r in rows]
    if not rows:
        return
    out = [r["out_bytes_per_sec"] for r in rows]
    inn = [r["in_bytes_per_sec"] for r in rows]
    pend = [r["pending_reliable"] + r.get("sent_unacked_reliable", 0) for r in rows]
    q = [r["queue_time_us"] / 1000 for r in rows]
    print(f"  {label}: out {statistics.median(out)/1024:.1f} KB/s (max {max(out)/1024:.1f}), "
          f"in {statistics.median(inn)/1024:.1f} KB/s (max {max(inn)/1024:.1f}), "
          f"reliable backlog max {max(pend)/1024:.1f} KB, queue max {max(q):.0f} ms")


def smoothness(path, label, remote=True):
    dumps = [r for r in records(path) if r.get("type") == "state_dump"]
    series = {}
    for d in dumps:
        for p in d["data"].get("players", []):
            if bool(p.get("remote")) == remote and p.get("position"):
                series.setdefault(p["player_idx"], []).append((d["t_ms"], p["position"]))
    frozen = jumps = steps = 0
    worst = 0.
    for idx, s in series.items():
        speeds = []
        for (t0, a), (t1, b) in zip(s, s[1:]):
            dt = (t1 - t0) / 1000
            if dt <= 0:
                continue
            speeds.append(math.dist(a[:2], b[:2]) / dt)
        moving = [v for v in speeds if v > 0.3]
        if len(moving) < 5:
            continue
        typical = statistics.median(moving)
        for v in speeds:
            steps += 1
            frozen += v < 0.1 * typical
            jumps += v > 2 * typical
            worst = max(worst, v / typical)
    if steps:
        print(f"  {label}: {len(series)} remote players, {steps} steps: frozen {100*frozen/steps:.1f}%, "
              f"jumps>2x {100*jumps/steps:.1f}%, worst {worst:.1f}x walking speed")


print("host")
stats(f"{root}/journal.jsonl", "per client connection")
smoothness(f"{root}/journal.jsonl", "own seats (ground truth)", False)
smoothness(f"{root}/journal.jsonl", "client players")
for c in sorted(glob.glob(f"{root}/client*/journal.jsonl")):
    name = c.split("/")[-2]
    print(name)
    stats(c, "own connection")
    smoothness(c, "remote players")


def tracks(path):
    """Per player index: (remote?, [positions by dump index])"""
    out = {}
    dumps = [r for r in records(path) if r.get("type") == "state_dump"]
    for k, d in enumerate(dumps):
        for p in d["data"].get("players", []):
            if p.get("position"):
                out.setdefault(p["player_idx"], (bool(p.get("remote")), {}))[1][k] = p["position"]
    return out


def residual(root):
    procs = [f"{root}/journal.jsonl"] + sorted(glob.glob(f"{root}/client*/journal.jsonl"))
    all_tracks = {pth: tracks(pth) for pth in procs}
    owner = {}
    for pth, tr in all_tracks.items():
        for idx, (remote, pos) in tr.items():
            if not remote:
                owner[idx] = pos
    print("residual after best latency shift (wu), per viewer:")
    for pth, tr in all_tracks.items():
        errs, shifts = [], []
        for idx, (remote, seen) in tr.items():
            if not remote or idx not in owner:
                continue
            truth = owner[idx]
            best = None
            for L in range(0, 9):
                d = [math.dist(seen[k][:2], truth[k - L][:2]) for k in seen if k - L in truth]
                if len(d) > 20:
                    # drop respawns/teleports
                    d = sorted(d)[: int(len(d) * .98)]
                    m = statistics.mean(d)
                    if best is None or m < best[0]:
                        best = (m, L)
            if best:
                errs.append(best[0]); shifts.append(best[1] * 250)
        if errs:
            name = pth.split("/")[-2] if "client" in pth else "host"
            print(f"  {name}: mean {statistics.mean(errs):.3f} wu, worst player {max(errs):.3f} wu, "
                  f"latency ~{statistics.median(shifts)} ms")


residual(root)
