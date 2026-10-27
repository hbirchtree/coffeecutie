#!/usr/bin/env python3
"""Turn a dummy plug's controller events into a Dolphin input movie (.dtm),
so BlamGX can be driven like BlamGraphics is: dolphin-emu -m out.dtm

Times become SI polls at --polls-per-second (libogc's default sampling).
Buttons map by name (RB is Z), LEFT/RIGHT sticks to the main/C-stick and the
triggers to L/R, clicking past 0.9."""

import argparse
import json
import struct

BUTTONS = {
    "START": 0, "A": 1, "B": 2, "X": 3, "Y": 4, "RB": 5,
    "POV_UP": 6, "POV_DOWN": 7, "POV_LEFT": 8, "POV_RIGHT": 9,
}
L_BIT, R_BIT, CONNECTED_BIT = 10, 11, 14


def stick(v):
    return max(0, min(255, round(128 + v * 127)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("plug")
    ap.add_argument("out")
    ap.add_argument("--polls-per-second", type=float, default=120.0)
    ap.add_argument("--game-id", default="")
    ap.add_argument("--index", type=int, default=0, help="plug controller index")
    args = ap.parse_args()

    plug = json.load(open(args.plug))
    events = [e for e in plug.get("events", [])
              if e.get("type") in ("controller_button", "controller_axis")
              and e.get("index", 0) == args.index]
    end = max([plug.get("end_time", 0)] +
              [e["time"] + e.get("duration", 0) for e in events]) / 1000.0
    rate = args.polls_per_second
    polls = int(end * rate) + 1

    buttons = [0] * polls
    axes = {k: [0.0] * polls for k in
            ("LEFT_X", "LEFT_Y", "RIGHT_X", "RIGHT_Y", "TRIGGER_LEFT", "TRIGGER_RIGHT")}
    for e in events:
        first = int(e["time"] / 1000.0 * rate)
        last = min(polls, max(first + 1, int((e["time"] + e.get("duration", 1)) / 1000.0 * rate)))
        if e["type"] == "controller_button":
            bit = BUTTONS.get(e.get("button", ""))
            if bit is None:
                continue
            for i in range(first, last):
                buttons[i] |= 1 << bit
        else:
            name = e.get("axis", "")
            if name not in axes:
                continue
            for i in range(first, last):
                axes[name][i] = float(e.get("value", 0.0))

    body = bytearray()
    for i in range(polls):
        b = buttons[i] | (1 << CONNECTED_BIT)
        tl, tr = axes["TRIGGER_LEFT"][i], axes["TRIGGER_RIGHT"][i]
        if tl > 0.9:
            b |= 1 << L_BIT
        if tr > 0.9:
            b |= 1 << R_BIT
        body += struct.pack(
            "<H6B", b,
            round(max(0.0, min(1.0, tl)) * 255), round(max(0.0, min(1.0, tr)) * 255),
            stick(axes["LEFT_X"][i]), stick(-axes["LEFT_Y"][i]),  # GC sticks are up-positive
            stick(axes["RIGHT_X"][i]), stick(-axes["RIGHT_Y"][i]))

    header = bytearray(256)
    header[0:4] = b"DTM\x1a"
    gid = args.game_id.encode()[:6]
    header[4:4 + len(gid)] = gid
    header[0xB] = 1  # GC controller 1
    struct.pack_into("<QQ", header, 0xD, int(end * 60), polls)
    # Playback stops past this many CPU ticks (Gekko: 486 MHz)
    struct.pack_into("<Q", header, 0xED, int((end + 60) * 486_000_000))
    open(args.out, "wb").write(bytes(header) + bytes(body))


if __name__ == "__main__":
    main()
