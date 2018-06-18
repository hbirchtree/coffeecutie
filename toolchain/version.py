#!/usr/bin/env python

from argparse import ArgumentParser
from os.path import dirname
from os.path import realpath
from os.path import isfile
from buildinfo import build_yml_filespec
from buildinfo import *

# common is always in path because buildinfo fixes the PYTHONPATH
from python.common import try_get_key

version_vars = ['major', 'minor', 'release', 'patch', 'hotfix']

def fallback_yml_read(data):
    output =  {}

    try:
        version_idx = data.find('version:')
        version_section = data[version_idx:]
        output['version'] = {}

        for k in version_vars:
            start = version_section.find(k)
            key_set = version_section[start:version_section.find('\n', start)]
            output['version'].update({k: int(key_set[key_set.find(':')+1:].strip())})

        version_prefix_section = data[data.find('versionprefix:'):]
        output['versionprefix'] = version_prefix_section[version_prefix_section.find(':')+1:].strip()
    except KeyError:
        pass
    except ValueError:
        pass

    return output

if __name__ == '__main__':
    args = ArgumentParser('VersionUtil')
    
    args.add_argument('--source-dir', dest='src_dir', type=str,
                      help='source directory containing build.yml',
                      default=dirname(dirname(realpath(__file__))))

    args.add_argument('--version', action='store_true')

    args.add_argument('increment_type', type=str,
                      choices=['none'] + version_vars,
                      help='which part of version to increment',
                      default='release')

    args = args.parse_args()

    def read_yaml_file(file):
        with open(file, mode='r') as f:
            yml_data = f.read()
            try:
                return load(yml_data, Loader=Loader)
            except NameError:
                return fallback_yml_read(yml_data)
    def save_yaml_file(file, src):
        data_dump = dump(src, default_flow_style=False)
        with open(file, mode='w') as f:
            f.write(data_dump)
    
    this_dir = args.src_dir

    build_config = '%s/%s' % (this_dir, build_yml_filespec)

    if not isfile(build_config):
        raise RuntimeError('Failed to locate build configuration file, exiting')

    def print_version(data):
        ver_str = ""
        i = 0
        for k in version_vars:
            ver_str += str(try_get_key(data, k, '0'))
            if i != 4:
                ver_str += '.'
            i += 1
        return ver_str

    config = read_yaml_file(build_config)

    version_cfg = try_get_key(config, 'version', {})

    version_prefix = try_get_key(config, 'versionprefix', 'automated-release-')

    if len(version_cfg.keys()) == 0:
        for k in version_vars:
            version_cfg[k] = 0
        version_cfg['major'] = 1

    if args.increment_type != 'none':
        version_cfg[args.increment_type] = 1 +  try_get_key(version_cfg,
                                                            args.increment_type,
                                                            0)

        found_type = False
        for i in range(len(version_vars)):
            if found_type:
                version_cfg[version_vars[i]] = 0
                continue
            if version_vars[i] == args.increment_type and args.increment_type:
                found_type = True
                continue

        config['version'] = version_cfg
        config['versionprefix'] = version_prefix

        save_yaml_file(build_config, config)

    if not args.version:
        print(version_prefix + print_version(version_cfg))
    else:
        print(print_version(version_cfg))

