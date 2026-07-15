#!/usr/bin/env python3
"""Diff the two state.json files written by a client/server BlamGraphics
pair (see the "dump_state" custom dummy-plug event in resource_creation.cpp).

Each process writes its own state.json into its own (harness-isolated)
TMPDIR, overwritten every time dump_state fires — so by the time both
processes have exited, each file holds that process's end-of-test view of
the world. This script loads both and diffs the "players" section.

state.json shape: {"players": [ {player_idx, seat_idx, name, remote,
loading_progress, connected, position}, ... ]}

Checks performed:
  - duplicate player_idx within a single side's roster (classic symptom of
    a local player's own roster entry arriving before its network identity
    is resolved, so an "is this me" check misses and a ghost copy gets
    created)
  - for player_idx values either side marked remote=true (i.e. actually
    went through PlayerSync, as opposed to a same-numbered but never-
    networked local splitscreen placeholder — see seat_idx 0-3 reservation
    in map_loading.cpp): missing on one side, name mismatch, remote flags
    not opposite on the two sides, or remote=true without connected=true
"""
import json
import sys
from collections import defaultdict


def load_state(path):
    with open(path, "r") as f:
        state = json.load(f)
    return state.get("players", [])


def duplicates(roster):
    seen = defaultdict(int)
    for e in roster:
        seen[e["player_idx"]] += 1
    return {idx: n for idx, n in seen.items() if n > 1}


def by_idx(roster):
    return {e["player_idx"]: e for e in roster}


def main():
    if len(sys.argv) != 3:
        print(f"usage: {sys.argv[0]} <server_state.json> <client_state.json>",
              file=sys.stderr)
        return 2

    try:
        s_roster = load_state(sys.argv[1])
    except (OSError, json.JSONDecodeError) as e:
        print(f"FAIL: could not read {sys.argv[1]}: {e}")
        return 1
    try:
        c_roster = load_state(sys.argv[2])
    except (OSError, json.JSONDecodeError) as e:
        print(f"FAIL: could not read {sys.argv[2]}: {e}")
        return 1

    problems = 0

    for label, roster in (("server", s_roster), ("client", c_roster)):
        dupes = duplicates(roster)
        if dupes:
            problems += 1
            print(f"FAIL: {label}: duplicate player_idx in roster: {dupes}")
            for idx in dupes:
                for e in roster:
                    if e["player_idx"] == idx:
                        print(f"    {e}")

    s_by, c_by = by_idx(s_roster), by_idx(c_roster)
    print(f"server players: {len(s_roster)}  client players: {len(c_roster)}")

    # Slots reserved for local splitscreen (seat_idx 0-3) exist
    # independently and identically on both processes whether or not
    # networking is involved — an idle seat's player_idx is never sent
    # over PlayerSync, so the same small integer showing up "locally" on
    # both sides is expected, not a replication bug. Only compare
    # identities that at least one side actually learned about *through
    # the network* — i.e. was marked remote=true by whichever side
    # received it via PlayerSync.
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
        print("\nPASS: state agrees, no duplicates found")
        return 0
    else:
        print(f"\nFAIL: {problems} problem(s) found")
        return 1


if __name__ == "__main__":
    sys.exit(main())
