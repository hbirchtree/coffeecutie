#!/usr/bin/env python3

from yaml import load, dump
try:
    from yaml import CLoader as Loader, CDumper as Dumper
except ImportError:
    from yaml import Loader, Dumper

import toolchain.python.common as common

from os.path import dirname, realpath, isfile, isdir
from subprocess import call
from sys import executable

if __name__ == '__main__':
    def read_yaml_file(file):
        with open(file, mode='r') as f:
            return load(f.read(), Loader=Loader)

    def save_yaml_file(file, src):
        with open(file, mode='w') as f:
            f.write(dump(src, default_flow_style=False))

    this_dir = dirname(realpath(__file__))

    local_config = '%s/%s' % (this_dir, common.local_yml_filespec)
    build_config = '%s/%s' % (this_dir, common.build_yml_filespec)

    repo_dir = None
    project_name = None
    display_name = None

    if isfile(build_config):
        #print('Found build configuration')
        config = read_yaml_file(build_config)

        if 'name' in config and 'display_name' in config:
            project_name = config['name']
            display_name = config['display_name']
    else:
        print('Failed to locate build configuration file, exiting')
        exit(1)

    if isfile(local_config):
        #print('Found local configuration')
        config = read_yaml_file(local_config)
        if 'source_dir' in config:
            repo_dir = config['source_dir']
            if not isdir(repo_dir):
                repo_dir = None
    else:
        print('Failed to locate local configuration, please specify new path:')
        repo_dir = input()

    if repo_dir is None or not isdir(repo_dir):
        print('Repo directory not found, exiting')
        exit(1)

    generate_file = '%s/toolchain/generate_project.py' % repo_dir

    with open(generate_file):
        pass

    args = [executable, generate_file,
            project_name, display_name,
            '-d', this_dir,
            '--execute']

    #print('Executing %s %s' % (executable, args))

    STDOUT = open('/dev/stdout', mode='w')

    exit(call(args=args, stdout=STDOUT, stderr=STDOUT))
