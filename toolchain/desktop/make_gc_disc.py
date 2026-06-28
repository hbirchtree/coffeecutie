#!/usr/bin/env python3
"""Assemble a Dolphin DirectoryBlob GameCube "disc" from a DOL + asset folders.

Dolphin boots a folder shaped like a real disc when pointed at <root>/sys/main.dol
(via Main.Core.DefaultISO). The app reads files/ at runtime via the DVD/FST. We
boot the DOL *directly* (executable boot) and only use this disc for its files,
so the apploader never runs -- an empty apploader.img is left for Dolphin to
auto-substitute with a safe stub. Dolphin patches the disc header with the real
FST offset/size, so boot.bin only needs the magic word + a game id.

Layout produced:
  <out>/sys/boot.bin        disc header (game id + magic 0xC2339F3D)
  <out>/sys/bi2.bin         boot info 2 (zeros)
  <out>/sys/apploader.img   empty -> Dolphin substitutes a non-running stub
  <out>/sys/main.dol        copy of the DOL (only needed for a valid blob)
  <out>/files/...           flattened contents of every --files dir
"""

import argparse
import os
import shutil
import struct
import sys

GC_MAGIC = 0xC2339F3D  # disc header offset 0x1C


def make_boot_bin(game_id: str, game_name: str) -> bytes:
    hdr = bytearray(0x440)
    gid = game_id.encode("ascii")[:6].ljust(6, b"\0")
    hdr[0x00:0x06] = gid
    struct.pack_into(">I", hdr, 0x1C, GC_MAGIC)
    name = game_name.encode("ascii")[:0x3F]
    hdr[0x20:0x20 + len(name)] = name
    return bytes(hdr)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--dol", required=True, help="main.dol to place on the disc")
    ap.add_argument("--files", nargs="*", default=[],
                    help="folders whose contents are copied (flat) into files/")
    ap.add_argument("--out", required=True, help="output disc root directory")
    ap.add_argument("--game-id", default="COFE01")
    ap.add_argument("--game-name", default="coffeecutie")
    args = ap.parse_args()

    sys_dir = os.path.join(args.out, "sys")
    files_dir = os.path.join(args.out, "files")
    os.makedirs(sys_dir, exist_ok=True)
    # Rebuild files/ each time so removed assets don't linger on the disc.
    if os.path.isdir(files_dir):
        shutil.rmtree(files_dir)
    os.makedirs(files_dir)

    with open(os.path.join(sys_dir, "boot.bin"), "wb") as f:
        f.write(make_boot_bin(args.game_id, args.game_name))
    with open(os.path.join(sys_dir, "bi2.bin"), "wb") as f:
        f.write(b"\0" * 0x2000)
    open(os.path.join(sys_dir, "apploader.img"), "wb").close()
    shutil.copyfile(args.dol, os.path.join(sys_dir, "main.dol"))

    count = 0
    for src in args.files:
        if not os.path.isdir(src):
            print(f"warning: --files dir not found: {src}", file=sys.stderr)
            continue
        for entry in sorted(os.listdir(src)):
            sp = os.path.join(src, entry)
            if os.path.isfile(sp):
                shutil.copyfile(sp, os.path.join(files_dir, entry))
                count += 1

    print(f"disc: {args.out} ({count} file(s) in files/)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
