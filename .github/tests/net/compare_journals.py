#!/usr/bin/env python3
"""Diff and inspect the journal.jsonl files written by a set of BlamGraphics
processes (see journal.h — one file per process, in that process's TMPDIR).

Each journal line is {"t_ms": ..., "type": ..., "data": {...}}. Entry types
of interest here:
  - state_dump:  full snapshot (same shape as state.json's
                 {"players": [...], "objects": [...]}), written on every
                 dump_state event
  - net_*:       connection lifecycle, joins, received rosters
  - game_event:  GameEventBus event names (payload-less trace)
  - dummy_event: what the test script injected, for timeline correlation

Usage:
  compare_journals.py <journal.jsonl> <journal.jsonl> [more...]
      Compares the LAST state_dump of every journal pairwise against the
      first journal (treated as the server/authority). With EXPECT_OBJECTS=N
      in the environment, the authority must also have spawned at least N
      replicated objects.

      Bipeds are checked too: within each dump, every player in play wears a
      model and has a collision body where its camera is (and nobody else
      does), and a networked player in play on both sides is at the same
      place, within POSITION_TOLERANCE (default 1.0, world units).
      MIN_IN_PLAY=N requires every dump to show at least N players in play,
      e.g. both sides' split screen seats.

  compare_journals.py --timeline <journal.jsonl> [more...]
      Prints all journals merged into one timeline (prefixed by journal
      name) instead of comparing. For eyeballing event ordering across
      processes; timestamps are per-process (relative to that journal's
      open), so ordering across files is approximate.
"""
import json
import os
import sys
from collections import defaultdict


def load_journal(path):
    entries = []
    with open(path, "r") as f:
        for line_no, line in enumerate(f, 1):
            line = line.strip()
            if not line:
                continue
            try:
                entries.append(json.loads(line))
            except json.JSONDecodeError as e:
                print(f"WARN: {path}:{line_no}: bad JSON line: {e}",
                      file=sys.stderr)
    return entries


def last_state_dump(entries):
    for entry in reversed(entries):
        if entry.get("type") == "state_dump":
            return entry.get("data", {})
    return None


def duplicates(roster):
    seen = defaultdict(int)
    for e in roster:
        seen[e["player_idx"]] += 1
    return {idx: n for idx, n in seen.items() if n > 1}


def compare_rosters(a_label, a_roster, b_label, b_roster):
    problems = 0

    for label, roster in ((a_label, a_roster), (b_label, b_roster)):
        dupes = duplicates(roster)
        if dupes:
            problems += 1
            print(f"FAIL: {label}: duplicate player_idx in roster: {dupes}")

    print(f"{a_label} players: {len(a_roster)}  "
          f"{b_label} players: {len(b_roster)}")

    networked = {e["player_idx"] for e in a_roster if e["remote"]} | \
                {e["player_idx"] for e in b_roster if e["remote"]}
    if not networked:
        problems += 1
        print(f"FAIL: {a_label} vs {b_label}: no player_idx marked "
              f"remote=true on either side — no replication happened")

    a_by = {e["player_idx"]: e for e in a_roster if e["player_idx"] in networked}
    b_by = {e["player_idx"]: e for e in b_roster if e["player_idx"] in networked}

    for only, present, missing in ((set(a_by) - set(b_by), a_label, b_label),
                                   (set(b_by) - set(a_by), b_label, a_label)):
        if only:
            problems += 1
            print(f"FAIL: player_idx on {present} but missing from "
                  f"{missing}: {sorted(only)}")

    for idx in sorted(set(a_by) & set(b_by)):
        a_e, b_e = a_by[idx], b_by[idx]
        if a_e["name"] != b_e["name"]:
            problems += 1
            print(f"FAIL: player_idx={idx} name mismatch: "
                  f"{a_label}={a_e['name']!r} {b_label}={b_e['name']!r}")
        if a_e["remote"] == b_e["remote"]:
            problems += 1
            print(f"FAIL: player_idx={idx} remote flag not opposite: "
                  f"{a_label}.remote={a_e['remote']} "
                  f"{b_label}.remote={b_e['remote']}")
        for label, e in ((a_label, a_e), (b_label, b_e)):
            if e["remote"] and not e["connected"]:
                problems += 1
                print(f"FAIL: player_idx={idx} on {label}: marked remote "
                      f"but connected=false")
    return problems


def compare_objects(a_label, a_objects, b_label, b_objects):
    problems = 0
    a_by = {o["net_id"]: o for o in a_objects}
    b_by = {o["net_id"]: o for o in b_objects}
    print(f"{a_label} objects: {len(a_by)}  {b_label} objects: {len(b_by)}")

    for only, present, missing in ((set(a_by) - set(b_by), a_label, b_label),
                                   (set(b_by) - set(a_by), b_label, a_label)):
        if only:
            problems += 1
            print(f"FAIL: net_id on {present} but missing from {missing}: "
                  f"{sorted(only)}")

    for net_id in sorted(set(a_by) & set(b_by)):
        a_o, b_o = a_by[net_id], b_by[net_id]
        if (a_o["tag_id"], a_o["tag_class"]) != (b_o["tag_id"], b_o["tag_class"]):
            problems += 1
            print(f"FAIL: net_id={net_id} tag mismatch: "
                  f"{a_label}={a_o['tag_class']}:{a_o['tag_id']} "
                  f"{b_label}={b_o['tag_class']}:{b_o['tag_id']}")
        if any(abs(x - y) > 1e-3
               for x, y in zip(a_o["position"], b_o["position"])):
            problems += 1
            print(f"FAIL: net_id={net_id} position mismatch: "
                  f"{a_label}={a_o['position']} {b_label}={b_o['position']}")
    return problems


LOCAL_ONLY_IDX_BASE = 0x10000  # PlayerInfo::local_only_idx_base


def close(a, b, tolerance):
    return all(abs(x - y) <= tolerance for x, y in zip(a, b))


def check_bipeds(label, players):
    """A player's biped (model + collision body) exists exactly while it is
    in play, and sits where its camera is: the model's feet eye_height under
    the camera, and the body under it, whether the body drives the camera
    (local physics mode) or follows it (everyone else)."""
    problems = 0
    in_play = 0
    for p in players:
        biped = p.get("biped")
        if biped is None:  # journal from before bipeds were dumped
            continue
        idx = p["player_idx"]
        model, body = biped["model"], biped["body"]
        if not biped["in_play"]:
            for what, value in (("model", model), ("body", body)):
                if value is not None:
                    problems += 1
                    print(f"FAIL: {label}: player_idx={idx} is not in play "
                          f"but still has a {what}")
            continue
        in_play += 1
        if model is None:
            problems += 1
            print(f"FAIL: {label}: player_idx={idx} is in play without a "
                  f"biped model")
        elif not close(model["position"],
                       [p["position"][0], p["position"][1],
                        p["position"][2] - biped.get("eye_height", 0.0)],
                       0.05):
            problems += 1
            print(f"FAIL: {label}: player_idx={idx} model at "
                  f"{model['position']}, camera at {p['position']}")
        if body is None:
            problems += 1
            print(f"FAIL: {label}: player_idx={idx} is in play without a "
                  f"collision body")
            continue
        kinematic = p["remote"] or not p.get("physics", False)
        if body["kinematic"] != kinematic:
            problems += 1
            print(f"FAIL: {label}: player_idx={idx} body kinematic="
                  f"{body['kinematic']}, expected {kinematic}")
        eye = [body["position"][0], body["position"][1],
               body["position"][2] + biped["eye_offset"]]
        if not close(eye, p["position"], 0.1):
            problems += 1
            print(f"FAIL: {label}: player_idx={idx} body at "
                  f"{body['position']} is not under camera {p['position']}")
    return problems, in_play


def check_local_seats(label, players):
    """Once a client has joined, its split screen seats leave the server's
    index space, so the server's players can never land on them."""
    if not any(p["remote"] for p in players):
        return 0
    problems = 0
    for p in players:
        if p["remote"] or p["seat_idx"] == 0:
            continue
        if p["player_idx"] < LOCAL_ONLY_IDX_BASE:
            problems += 1
            print(f"FAIL: {label}: local seat {p['seat_idx']} has "
                  f"player_idx={p['player_idx']}, inside the server's "
                  f"index space")
    return problems


def compare_bipeds(a_label, a_roster, b_label, b_roster):
    """A networked player in play on both sides is at the same place on
    both. Each process dumps at its own scripted time, so positions are
    compared with a tolerance, and a player caught spawning between the two
    dumps (in play on one side only) is only noted; MIN_IN_PLAY is what
    catches bipeds that never show up."""
    tolerance = float(os.environ.get("POSITION_TOLERANCE", "1.0"))
    problems = 0
    a_by = {e["player_idx"]: e for e in a_roster}
    b_by = {e["player_idx"]: e for e in b_roster}
    for idx in sorted(set(a_by) & set(b_by)):
        a_e, b_e = a_by[idx], b_by[idx]
        if a_e["remote"] == b_e["remote"]:
            continue  # not the same player, compare_rosters reports it
        if "biped" not in a_e or "biped" not in b_e:
            continue
        a_play, b_play = a_e["biped"]["in_play"], b_e["biped"]["in_play"]
        if a_play != b_play:
            print(f"NOTE: player_idx={idx} in play on "
                  f"{a_label if a_play else b_label} only")
            continue
        if not a_play:
            continue
        delta = max(abs(x - y) for x, y in
                    zip(a_e["position"], b_e["position"]))
        print(f"player_idx={idx}: {a_label} sees {a_e['position']}, "
              f"{b_label} sees {b_e['position']} (off by {delta:.3f})")
        if delta > tolerance:
            problems += 1
            print(f"FAIL: player_idx={idx} is {delta:.3f} apart between "
                  f"{a_label} and {b_label} (tolerance {tolerance})")
    return problems


def unanswered_requests(label, entries):
    requested = {e["data"]["request_id"] for e in entries
                 if e.get("type") == "net_spawn_request"}
    answered = {e["data"]["request_id"] for e in entries
                if e.get("type") in ("net_spawn_accepted",
                                     "net_spawn_rejected")}
    if requested:
        print(f"{label} spawn requests: {len(requested)}, "
              f"answered: {len(requested & answered)}")
    missing = requested - answered
    if missing:
        print(f"FAIL: {label}: spawn requests never answered: "
              f"{sorted(missing)}")
        return 1
    return 0


def clock_error(a_label, a_entries, b_label, b_entries):
    """Same-machine only: both processes share compo::clock, so the true GNS
    offset is the difference of their journalled GNS-minus-local bases."""
    def base(entries):
        for e in entries:
            if e.get("type") == "net_clock_base":
                return e["data"]["gns_minus_local"]
        return None
    samples = [e["data"] for e in b_entries if e.get("type") == "net_clock"]
    a_base, b_base = base(a_entries), base(b_entries)
    if not samples or a_base is None or b_base is None:
        return 0
    truth = a_base - b_base
    error = samples[-1]["applied"] - truth
    best_rtt = min(s["rtt"] for s in samples)
    tolerance = int(os.environ.get("CLOCK_TOLERANCE_US", "2000"))
    print(f"{b_label} clock: {len(samples)} samples, best rtt {best_rtt}us, "
          f"error vs {a_label} {error}us (tolerance {tolerance}us), "
          f"steady offset {samples[-1]['steady_offset']}us")
    if abs(error) > tolerance:
        print(f"FAIL: {b_label} clock offset off by {error}us")
        return 1
    return 0


def label_for(path):
    # <out>/client0/journal.jsonl -> client0; <out>/journal.jsonl -> <out>
    return os.path.basename(os.path.dirname(os.path.abspath(path))) or path


def timeline(paths):
    merged = []
    for path in paths:
        label = label_for(path)
        for entry in load_journal(path):
            merged.append((entry.get("t_ms", 0), label, entry))
    merged.sort(key=lambda item: item[0])
    for t_ms, label, entry in merged:
        data = entry.get("data")
        detail = f" {json.dumps(data)}" if data is not None else ""
        print(f"{t_ms:>8}ms {label:>12} {entry.get('type', '?')}{detail}")
    return 0


def main():
    args = sys.argv[1:]
    if args and args[0] == "--timeline":
        paths = args[1:]
        if not paths:
            print(f"usage: {sys.argv[0]} --timeline <journal.jsonl>...",
                  file=sys.stderr)
            return 2
        return timeline(paths)

    if len(args) < 2:
        print(f"usage: {sys.argv[0]} <authority.jsonl> <peer.jsonl>... | "
              f"--timeline <journal.jsonl>...", file=sys.stderr)
        return 2

    dumps = []
    for path in args:
        entries = load_journal(path)
        dump = last_state_dump(entries)
        if dump is None:
            print(f"FAIL: {path}: no state_dump entry in journal "
                  f"(dump_state never fired?)")
            return 1
        dumps.append((label_for(path), dump))

    problems = 0
    authority_label, authority = dumps[0]
    expect_objects = int(os.environ.get("EXPECT_OBJECTS", "0"))
    if len(authority.get("objects", [])) < expect_objects:
        problems += 1
        print(f"FAIL: {authority_label} has "
              f"{len(authority.get('objects', []))} replicated objects, "
              f"expected at least {expect_objects}")
    min_in_play = int(os.environ.get("MIN_IN_PLAY", "0"))
    for i, (label, dump) in enumerate(dumps):
        players = dump.get("players", [])
        biped_problems, in_play = check_bipeds(label, players)
        problems += biped_problems
        if i > 0:  # the authority's seats are the server's index space
            problems += check_local_seats(label, players)
        print(f"{label}: {in_play} player(s) in play")
        if in_play < min_in_play:
            problems += 1
            print(f"FAIL: {label} has {in_play} player(s) in play, expected "
                  f"at least {min_in_play}")
    authority_entries = load_journal(args[0])
    for path in args[1:]:
        entries = load_journal(path)
        problems += unanswered_requests(label_for(path), entries)
        problems += clock_error(authority_label, authority_entries,
                                label_for(path), entries)
    for peer_label, peer in dumps[1:]:
        problems += compare_rosters(authority_label,
                                    authority.get("players", []),
                                    peer_label, peer.get("players", []))
        problems += compare_bipeds(authority_label,
                                   authority.get("players", []),
                                   peer_label, peer.get("players", []))
        problems += compare_objects(authority_label,
                                    authority.get("objects", []),
                                    peer_label, peer.get("objects", []))

    if problems == 0:
        print("\nPASS: journals agree, no duplicates found")
        return 0
    print(f"\nFAIL: {problems} problem(s) found")
    return 1


if __name__ == "__main__":
    sys.exit(main())
