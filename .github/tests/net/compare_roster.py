#!/usr/bin/env python3
"""Diff PLAYERDUMP snapshots from a client/server BlamGraphics log pair.

Each snapshot is one line matching:
    PLAYERDUMP tag=<tag> [<json roster array>]
emitted by the "dump_players" dummy-plug custom event (resource_creation.cpp).
Tags are paired across the two logs by their numeric suffix, e.g.
server_t1 <-> client_t1, so the two dumps being compared were taken at the
same wall-clock offset in each process's dummy_plug config.

A roster entry:
    {"player_idx", "seat_idx", "name", "remote", "loading_progress",
     "connected", "position"}

Checks performed, independent of timing (a bug regardless of when seen):
  - duplicate player_idx within a single roster (classic symptom of the
    PlayerSync ordering issue: a local player's own roster entry arrives
    before PlayerJoinConfirm remaps its player_idx, so the "is_self" check
    misses and a ghost remote copy of yourself gets created)

Checks performed only on the LAST paired snapshot (earlier ones may still
be mid-handshake):
  - player_idx present on one side but not the other
  - name mismatch for the same player_idx
  - connected=false for a player_idx that exists on both sides
"""
import json
import re
import sys
from collections import defaultdict

DUMP_RE = re.compile(r"PLAYERDUMP tag=(\S+)\s+(\[.*\])\s*$")


def load_dumps(path):
    """tag -> roster (list of dict)"""
    out = {}
    with open(path, "r", errors="replace") as f:
        for line in f:
            m = DUMP_RE.search(line)
            if not m:
                continue
            tag, blob = m.group(1), m.group(2)
            try:
                out[tag] = json.loads(blob)
            except json.JSONDecodeError as e:
                print(f"WARN: {path}: bad JSON for tag {tag}: {e}", file=sys.stderr)
    return out


def duplicates(roster):
    seen = defaultdict(int)
    for e in roster:
        seen[e["player_idx"]] += 1
    return {idx: n for idx, n in seen.items() if n > 1}


def by_idx(roster):
    return {e["player_idx"]: e for e in roster}


def pair_tags(server_tags, client_tags):
    """('server_t1','client_t1') pairs ordered by trailing snapshot number."""
    def suffix(tag):
        m = re.search(r"(\d+)$", tag)
        return int(m.group(1)) if m else -1

    server_by_n = {suffix(t): t for t in server_tags}
    client_by_n = {suffix(t): t for t in client_tags}
    common = sorted(set(server_by_n) & set(client_by_n))
    return [(server_by_n[n], client_by_n[n]) for n in common]


def main():
    if len(sys.argv) != 3:
        print(f"usage: {sys.argv[0]} <server.log> <client.log>", file=sys.stderr)
        return 2

    server_dumps = load_dumps(sys.argv[1])
    client_dumps = load_dumps(sys.argv[2])

    if not server_dumps:
        print(f"FAIL: no PLAYERDUMP lines found in {sys.argv[1]}")
        return 1
    if not client_dumps:
        print(f"FAIL: no PLAYERDUMP lines found in {sys.argv[2]}")
        return 1

    problems = 0

    # Duplicate-player_idx check: every snapshot, both sides, always a bug.
    for label, dumps in (("server", server_dumps), ("client", client_dumps)):
        for tag, roster in dumps.items():
            dupes = duplicates(roster)
            if dupes:
                problems += 1
                print(f"FAIL: {label}/{tag}: duplicate player_idx in roster: {dupes}")
                for idx in dupes:
                    entries = [e for e in roster if e["player_idx"] == idx]
                    for e in entries:
                        print(f"    {e}")

    pairs = pair_tags(server_dumps.keys(), client_dumps.keys())
    if not pairs:
        print("FAIL: no matching (server_tN, client_tN) snapshot pairs found")
        return 1

    print(f"Snapshot pairs compared: {[p for p in pairs]}")
    last_server_tag, last_client_tag = pairs[-1]
    s_roster = server_dumps[last_server_tag]
    c_roster = client_dumps[last_client_tag]
    s_by = by_idx(s_roster)
    c_by = by_idx(c_roster)

    print(f"\nFinal snapshot: server={last_server_tag} ({len(s_roster)} entries), "
          f"client={last_client_tag} ({len(c_roster)} entries)")

    # Slots reserved for local splitscreen (seat_idx 0-3, see
    # map_loading.cpp) exist independently and identically on both
    # processes whether or not networking is involved — an idle seat's
    # player_idx is never sent over PlayerSync, so the same small integer
    # showing up "locally" on both sides is expected, not a replication
    # bug. Only compare identities that at least one side actually learned
    # about *through the network* — i.e. was marked remote=true by
    # whichever side received it via PlayerSync.
    networked = {e["player_idx"] for e in s_roster if e["remote"]} | \
                {e["player_idx"] for e in c_roster if e["remote"]}
    if not networked:
        problems += 1
        print("FAIL: no player_idx was marked remote=true on either side — "
              "replication doesn't appear to have happened at all")

    s_net = {i: e for i, e in s_by.items() if i in networked}
    c_net = {i: e for i, e in c_by.items() if i in networked}

    only_server = set(s_net) - set(c_net)
    only_client = set(c_net) - set(s_net)
    if only_server:
        problems += 1
        print(f"FAIL: player_idx on server but missing from client: {sorted(only_server)}")
        for idx in sorted(only_server):
            print(f"    server: {s_net[idx]}")
    if only_client:
        problems += 1
        print(f"FAIL: player_idx on client but missing from server: {sorted(only_client)}")
        for idx in sorted(only_client):
            print(f"    client: {c_net[idx]}")

    for idx in sorted(set(s_net) & set(c_net)):
        s_e, c_e = s_by[idx], c_by[idx]
        if s_e["name"] != c_e["name"]:
            problems += 1
            print(f"FAIL: player_idx={idx} name mismatch: "
                  f"server={s_e['name']!r} client={c_e['name']!r}")
        # A player_idx that's remote from one side is local from the other
        # (each process sees the peer as "remote", itself as local) — one
        # of the two entries should be remote=false; if both are, or
        # neither is, replication has gone wrong.
        if s_e["remote"] == c_e["remote"]:
            problems += 1
            print(f"FAIL: player_idx={idx} remote flag not opposite: "
                  f"server.remote={s_e['remote']} client.remote={c_e['remote']}")
        # The entry each side marks "remote" (i.e. is the peer, not itself)
        # should show connected=true by the final snapshot.
        for label, e in (("server", s_e), ("client", c_e)):
            if e["remote"] and not e["connected"]:
                problems += 1
                print(f"FAIL: player_idx={idx} on {label}: marked remote "
                      f"but connected=false")

    if problems == 0:
        print("\nPASS: rosters agree, no duplicates found")
        return 0
    else:
        print(f"\nFAIL: {problems} problem(s) found")
        return 1


if __name__ == "__main__":
    sys.exit(main())
