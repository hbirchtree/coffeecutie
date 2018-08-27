#!/usr/bin/env python3
from __future__ import print_function

USE_TEXT_PARSING = False

from sys import stderr, version_info, path
from os import getcwd
try:
    from yaml import load, dump
except ImportError:
    USE_TEXT_PARSING = True

from os.path import dirname, realpath, isfile
from argparse import ArgumentParser

if version_info[0] == 3:
    from python.common import build_yml_filespec
else:
    path.append("%s/python/" % (realpath(dirname(__file__))))
    from python.common import build_yml_filespec

try:
    from yaml import CLoader as Loader, CDumper as Dumper
except ImportError:
    try:
        from yaml import Loader, Dumper
    except ImportError:
        pass

if __name__ == '__main__':
    args = ArgumentParser('BuildInfo')

    args.add_argument('--source-dir', dest='src_dir', type=str,
                      help='source directory containing build.yml',
                      default=dirname(dirname(realpath(__file__))))
    args.add_argument('keys', nargs='*',
                      help='keys to index build info with')

    args = args.parse_args()

    def read_yaml_file(file):
        with open(file, mode='r') as f:
            if USE_TEXT_PARSING:
                return f.read()
            else:
                return load(f.read(), Loader=Loader)

    def save_yaml_file(file, src):
        with open(file, mode='w') as f:
            f.write(dump(src, default_flow_style=False))

    this_dir = args.src_dir

    build_config = '%s/%s' % (this_dir, build_yml_filespec)

    if isfile(build_config):
        config = read_yaml_file(build_config)
    else:
        raise RuntimeError('Failed to locate build configuration file, exiting')

    values = config

    if USE_TEXT_PARSING:
        prim_key = args.keys.pop(0)
        if prim_key == 'dependencies':
            sec_key = args.keys.pop(0)
            idx = config.find(sec_key)
            if idx < 0:
                exit(0)
            idx = config.find(':', idx) + 1
            end = config.find('\n', idx)
            print(config[idx:end].strip())
        elif prim_key == 'script_location':
            idx = config.find(prim_key)
            if idx < 0:
                exit(0)
            idx = config.find(':', idx) + 1
            end = config.find('\n', idx)
            print(config[idx:end].strip())
        elif prim_key == 'makefile_location':
            idx = config.find(prim_key)
            if idx < 0:
                exit(0)
            idx = config.find(':', idx) + 1
            end = config.find('\n', idx)
            print(config[idx:end].strip())
        exit(0)

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
    except IndexError:
        pass
    except KeyError:
        pass

    exit(0)
