#!/usr/bin/env python3
"""Wrap the linked gc_apploader ELF's raw code into the apploader.img format Dolphin's
BS2 HLE reads directly off disc (CBoot::RunApploader): a 0x20-byte header giving the
entry point / body size / trailer size, followed by the body itself.

  0x00..0x10  unused by RunApploader (real IPL uses it for a build date) -> zeroed
  0x10        entry point (u32 BE) -- absolute address of apploader_entry
  0x14        body size (u32 BE)   -- length of --raw
  0x18        trailer size (u32 BE) -- always 0 here
  0x1C        pad (u32 BE) -- zeroed
  0x20..      body bytes (--raw, verbatim)
"""

import argparse
import re
import struct
import subprocess
import sys


def find_entry(nm: str, elf: str, symbol: str) -> int:
    out = subprocess.run([nm, elf], capture_output=True, check=True, text=True).stdout
    for line in out.splitlines():
        m = re.match(r"^([0-9a-fA-F]+)\s+\S+\s+" + re.escape(symbol) + r"$", line)
        if m:
            return int(m.group(1), 16)
    print(f"error: symbol '{symbol}' not found in {elf}", file=sys.stderr)
    sys.exit(1)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--elf", required=True, help="linked apploader ELF (for symbol lookup)")
    ap.add_argument("--raw", required=True, help="objcopy -O binary output of --elf")
    ap.add_argument("--nm", required=True, help="path to the target's nm")
    ap.add_argument("--entry-symbol", default="apploader_entry")
    ap.add_argument("--out", required=True)
    args = ap.parse_args()

    entry = find_entry(args.nm, args.elf, args.entry_symbol)
    with open(args.raw, "rb") as f:
        body = f.read()

    header = bytearray(0x20)
    struct.pack_into(">I", header, 0x10, entry)
    struct.pack_into(">I", header, 0x14, len(body))
    struct.pack_into(">I", header, 0x18, 0)

    with open(args.out, "wb") as f:
        f.write(header)
        f.write(body)

    print(f"apploader: {args.out} (entry=0x{entry:08x}, body={len(body)}B)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
