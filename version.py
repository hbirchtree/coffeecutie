#!/usr/bin/env python

from buildinfo import *
# common is always in path because buildinfo fixes the PYTHONPATH
from common import try_get_key

version_vars = ['major', 'minor', 'release', 'patch', 'hotfix']

if __name__ == '__main__':
    args = ArgumentParser('VersionUtil')
    
    args.add_argument('--source-dir', dest='src_dir', type=str,
                      help='source directory containing build.yml',
                      default=dirname(realpath(__file__)))

    args.add_argument('--version', action='store_true')

    args.add_argument('increment_type', type=str,
                      choices=['none'] + version_vars,
                      help='which part of version to increment',
                      default='release')

    args = args.parse_args()

    def read_yaml_file(file):
        with open(file, mode='r') as f:
            return load(f.read(), Loader=Loader);
    def save_yaml_file(file, src):
        with open(file, mode='w') as f:
            f.write(dump(src, default_flow_style=False))
    
    this_dir = args.src_dir

    build_config = '%s/%s' % (this_dir, build_yml_filespec)

    if not isfile(build_config):
        eprint('Failed to locate build configuration file, exiting', file=stderr)
        exit(1)

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
        version_cfg['release'] = 1

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

