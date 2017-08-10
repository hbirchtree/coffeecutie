#!/usr/bin/env python3

from sys import stderr
from common import build_yml_filespec

from yaml import load, dump
try:
    from yaml import CLoader as Loader, CDumper as Dumper
except ImportError:
    from yaml import Loader, Dumper

from os.path import dirname, realpath, isfile

from argparse import ArgumentParser

if __name__ == '__main__':
    args = ArgumentParser('BuildInfo')

    args.add_argument('--source-dir', dest='src_dir', type=str,
                      help='source directory containing build.yml',
                      default=dirname(realpath(__file__)))
    args.add_argument('keys', nargs='*',
                      help='keys to index build info with')

    args = args.parse_args()

    def read_yaml_file(file):
        with open(file, mode='r') as f:
            return load(f.read(), Loader=Loader)

    def save_yaml_file(file, src):
        with open(file, mode='w') as f:
            f.write(dump(src, default_flow_style=False))

    this_dir = args.src_dir

    build_config = '%s/%s' % (this_dir, build_yml_filespec)

    if isfile(build_config):
        config = read_yaml_file(build_config)
    else:
        print('Failed to locate build configuration file, exiting', file=stderr)
        exit(1)

    values = config

    try:
        while args.keys:
            if type(values) == list:
                values = values[int(args.keys.pop(0))]
            elif type(values) == dict:
                values = values[args.keys.pop(0)]
            else:
                break

        print(values)
    except [IndexError, KeyError]:
        pass

    exit(0)
