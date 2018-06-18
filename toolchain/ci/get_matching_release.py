#!/usr/bin/env python

from argparse import ArgumentParser

args = ArgumentParser('TravisHelper')

args.add_argument('command', type=str)

args.add_argument('items', nargs='*')

args = args.parse_args()

if args.command == 'get-closest-release':
    target = args.items.pop(0)

    assert(len(args.items) > 0)

    # First, check for exact match
    for e in args.items:
        if target == e:
            print(e)
            exit(0)

    # Second, check for approximate match
    for e in args.items:
        if target.startswith(e):
            print(e)
            exit(0)

    # In the worst case, return the latest release
    print(args.items.pop(0))
    exit(0)
