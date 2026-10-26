#!/usr/bin/env python3
"""A crowd for network load tests: a host with 4 split-screen seats that spawns
N clients of 4 seats each, at a simulated lag; every seat runs, fires and turns
now and then. Everyone dumps state every 250 ms for crowd_report.py.

    crowd_plugs.py <dir> <clients> <lag ms>
    DUMMY_PLUG_CONFIG=<dir>/crowd_host.json ./cb run ...:rel/BlamGraphics -- <assets> <map>
    crowd_report.py <the host's TMPDIR>
"""
import json, random, sys

out_dir, clients, lag = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
SEATS, PORT = 4, 27107
MOVE_AT, MOVE_FOR, END = 45000, 60000, 110000
g = {"window": {"width": 160, "height": 120, "depth": 24}, "software_renderer": "llvmpipe",
     "gl": [{"major": 4, "minor": 6}], "gles": [], "screenshot_quality": 40}


def seats(seed):
    ev = []
    for s in range(SEATS):
        rng = random.Random(seed * 100 + s)
        ev.append({"time": 1200 + s * 50, "type": "controller_connect", "index": s})
        ev.append({"time": MOVE_AT - 3000, "type": "camera", "seat": s, "physics": True})
        t0 = MOVE_AT + s * 300
        ev.append({"time": t0, "type": "controller_axis", "axis": "LEFT_Y", "value": -1.0,
                   "duration": MOVE_FOR, "index": s})
        ev.append({"time": t0, "type": "controller_axis", "axis": "TRIGGER_RIGHT", "value": 1.0,
                   "duration": MOVE_FOR, "index": s})
        t = t0 + rng.randint(3000, 7000)
        while t < t0 + MOVE_FOR:
            ev.append({"time": t, "type": "controller_axis", "axis": "RIGHT_X",
                       "value": rng.choice([-1, 1]) * rng.uniform(.5, 1.), "duration": rng.randint(200, 600),
                       "index": s})
            t += rng.randint(3000, 7000)
    ev += [{"time": t, "type": "dump_state"} for t in range(MOVE_AT, MOVE_AT + MOVE_FOR, 250)]
    return ev


host = {"graphics": g, "time_base": "session", "spawn_grace_ms": 1800000,
        "spawn": [{"config": f"crowd_client{i}.json", "id": f"client{i}", "args": ["--net-lag", str(lag)]}
                  for i in range(clients)],
        "events": [{"time": 500, "type": "net_listen", "clock": "local", "address": f"127.0.0.1:{PORT}"}] + seats(0),
        "end_time": END}
json.dump(host, open(f"{out_dir}/crowd_host.json", "w"), indent=1)
for i in range(clients):
    c = {"graphics": g, "time_base": "session",
         "events": [{"time": 500, "type": "net_connect", "clock": "local", "address": f"127.0.0.1:{PORT}"}] + seats(i + 1),
         "end_time": END + 2000}
    json.dump(c, open(f"{out_dir}/crowd_client{i}.json", "w"), indent=1)
