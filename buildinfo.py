#!/usr/bin/env python
from __future__ import print_function

from sys import stderr, version_info, path
from os import getcwd
from yaml import load, dump

if version_info[0] == 3:
    from tools.python.common import build_yml_filespec
else:
    path.append("%s/tools/python/" % (getcwd()))
    from common import build_yml_filespec

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
        eprint('Failed to locate build configuration file, exiting', file=stderr)
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

        if type(values) == dict:
            for k in values.keys():
                print(k)
        elif type(values) == list:
            for v in values:
                print(v)
        else:
            print(values)
    except [IndexError, KeyError]:
        pass

    exit(0)
