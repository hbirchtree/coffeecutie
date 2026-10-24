#!/usr/bin/env python3
"""Rewrite <stamp> only when the files in <dir> changed (name, size, mtime).

The GameCube disc is packed after the ELF links; linking against this stamp
makes files dropped into the assets folder (maps, configs) repack the disc.
"""

from os import makedirs, scandir
from os.path import dirname, exists
from sys import argv

directory, stamp = argv[1], argv[2]

listing = ''
if exists(directory):
    for entry in sorted(scandir(directory), key=lambda e: e.name):
        if entry.is_file():
            st = entry.stat()
            listing += f'{entry.name} {st.st_size} {st.st_mtime_ns}\n'

previous = open(stamp).read() if exists(stamp) else None
if listing != previous:
    makedirs(dirname(stamp), exist_ok=True)
    with open(stamp, 'w') as f:
        f.write(listing)
