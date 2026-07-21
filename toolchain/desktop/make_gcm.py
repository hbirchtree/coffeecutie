#!/usr/bin/env python3
"""Assemble a real, bootable GameCube disc image (.gcm) from a DOL, a flat set of asset
files and a built gc_apploader image (see gc_apploader/).

Unlike make_gc_disc.py's DirectoryBlob-shaped output (which only Dolphin's directory
loader understands, and which skips the apploader entirely by booting the DOL as an
"executable"), this writes an actual disc: a real boot.bin with dol/FST offsets filled
in, the real apploader at the fixed 0x2440 disc offset, the DOL, and a real FST -- the
same shape Dolphin (or any other GC tool) expects when opening a plain ISO/GCM. Trimmed,
not padded to the retail 1.4 GiB disc size; both Dolphin and real-hardware loaders (e.g.
Swiss) accept trimmed images fine.

Layout:
  0x00000000  boot.bin        (0x440,  disc header: game id, magic, dol/fst offsets)
  0x00000440  bi2.bin         (0x2000, zeroed -- unused by BS2 HLE)
  0x00002440  apploader.img   (from gc_apploader)
  <32-aligned> main.dol
  <32-aligned> FST (root dir entry + one entry per file, flat, + string table)
  <32-aligned> file data, one per input file, each 32-byte aligned
"""

import argparse
import os
import struct
import sys

GC_MAGIC = 0xC2339F3D  # disc header offset 0x1C
APPLOADER_OFFSET = 0x2440


def align_up(value: int, align: int) -> int:
    return (value + align - 1) & ~(align - 1)


def make_boot_bin(game_id: str, game_name: str, dol_offset: int, fst_offset: int,
                   fst_size: int) -> bytes:
    hdr = bytearray(0x440)
    gid = game_id.encode("ascii")[:6].ljust(6, b"\0")
    hdr[0x00:0x06] = gid
    struct.pack_into(">I", hdr, 0x1C, GC_MAGIC)
    name = game_name.encode("ascii")[:0x3F]
    hdr[0x20:0x20 + len(name)] = name
    struct.pack_into(">I", hdr, 0x420, dol_offset)
    struct.pack_into(">I", hdr, 0x424, fst_offset)
    struct.pack_into(">I", hdr, 0x428, fst_size)
    struct.pack_into(">I", hdr, 0x42C, fst_size)  # fst_max_size: no secondary partitions
    return bytes(hdr)


def build_fst(names: list, data_offsets: list, sizes: list) -> bytes:
    """Flat FST: one root dir entry followed by one file entry per input, matching the
    layout platforms/gekko/dvd.h's parse_fst() (and any standard GC FST reader) expects."""
    count = 1 + len(names)
    entries = bytearray(count * 12)

    # Root dir entry: type=1, name_offset=0, parent=0, num_entries=count.
    entries[0] = 1
    struct.pack_into(">I", entries, 4, 0)
    struct.pack_into(">I", entries, 8, count)

    strings = bytearray(b"\0")  # offset 0 reserved (root has no name)
    for i, (name, off, size) in enumerate(zip(names, data_offsets, sizes)):
        name_off = len(strings)
        strings += name.encode("ascii") + b"\0"

        base = (i + 1) * 12
        entries[base] = 0  # file
        entries[base + 1] = (name_off >> 16) & 0xFF
        entries[base + 2] = (name_off >> 8) & 0xFF
        entries[base + 3] = name_off & 0xFF
        struct.pack_into(">I", entries, base + 4, off)
        struct.pack_into(">I", entries, base + 8, size)

    return bytes(entries) + bytes(strings)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--dol", required=True, help="main.dol to place on the disc")
    ap.add_argument("--files", nargs="*", default=[],
                     help="folders whose contents are copied (flat) onto the disc")
    ap.add_argument("--apploader", required=True, help="apploader.img (see gc_apploader/)")
    ap.add_argument("--out", required=True, help="output .gcm path")
    ap.add_argument("--game-id", default="COFE01")
    ap.add_argument("--game-name", default="coffeecutie")
    args = ap.parse_args()

    with open(args.apploader, "rb") as f:
        apploader = f.read()
    with open(args.dol, "rb") as f:
        dol = f.read()

    file_paths = []
    for src in args.files:
        if not os.path.isdir(src):
            print(f"warning: --files dir not found: {src}", file=sys.stderr)
            continue
        for entry in sorted(os.listdir(src)):
            sp = os.path.join(src, entry)
            if os.path.isfile(sp):
                file_paths.append((entry, sp))

    dol_offset = align_up(APPLOADER_OFFSET + len(apploader), 32)
    fst_offset = align_up(dol_offset + len(dol), 32)

    # First pass with placeholder file offsets to learn the FST's own size, since file
    # data placement depends on where the FST ends.
    placeholder = build_fst([n for n, _ in file_paths], [0] * len(file_paths),
                             [os.path.getsize(p) for _, p in file_paths])
    data_start = align_up(fst_offset + len(placeholder), 32)

    data_offsets = []
    cursor = data_start
    for _, sp in file_paths:
        data_offsets.append(cursor)
        cursor = align_up(cursor + os.path.getsize(sp), 32)

    fst = build_fst([n for n, _ in file_paths], data_offsets,
                     [os.path.getsize(p) for _, p in file_paths])
    assert len(fst) == len(placeholder), "FST size changed between passes"

    boot_bin = make_boot_bin(args.game_id, args.game_name, dol_offset, fst_offset, len(fst))

    os.makedirs(os.path.dirname(os.path.abspath(args.out)) or ".", exist_ok=True)
    with open(args.out, "wb") as f:
        f.write(boot_bin)
        f.write(b"\0" * (APPLOADER_OFFSET - f.tell()))  # bi2.bin region, zeroed
        f.write(apploader)
        f.write(b"\0" * (dol_offset - f.tell()))
        f.write(dol)
        f.write(b"\0" * (fst_offset - f.tell()))
        f.write(fst)
        for (name, sp), off in zip(file_paths, data_offsets):
            f.write(b"\0" * (off - f.tell()))
            with open(sp, "rb") as sf:
                f.write(sf.read())
        # platforms/gekko/dvd.h always bounce-reads in 32-byte-aligned spans (see
        # read_at()), so the image must extend to the next 32-byte boundary past the
        # last real byte or that last aligned read runs past EOF and fails.
        f.write(b"\0" * (align_up(f.tell(), 32) - f.tell()))

    total = os.path.getsize(args.out)
    print(f"gcm: {args.out} ({total} bytes, {len(file_paths)} file(s), "
          f"dol@0x{dol_offset:x} fst@0x{fst_offset:x})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
