#!/usr/bin/env python3

from yaml import load, dump
try:
    from yaml import CLoader as Loader, CDumper as Dumper
except ImportError:
    from yaml import Loader, Dumper

from os.path import isfile, dirname, realpath

from argparse import ArgumentParser

from collections import namedtuple

_yaml_template = 'cmake/Templates/coffee-build.yml'


_TARGET_NAMES = {
    'linux': {
        'ubuntu': ['amd64', 'i686'],
        'fedora': ['amd64'],
        'emscripten': ['wasm', 'asmjs'],
        'steam': ['amd64'],
        'android': ['armv7a', 'armv8a', 'armv7a.kitkat'],
        'maemo': ['armv7a'],
        'raspberry': ['armhf']
    },
    'osx': ['osx', 'ios'],
    'windows': ['win32', 'uwp']
}

appveyor_targets = ['windows']
travis_targets = ['osx', 'ubuntu', 'fedora',
                  'emscripten', 'steam',
                  'android', 'maemo',
                  'raspberry']

DeployInfo = namedtuple('DeployInfo', 'deploy_branches, build_branches')


def parse_yaml(source_file):
    with open(source_file, mode='r') as f:
        data = load(f.read(), Loader=Loader)
        return data


def render_yaml(source):
    if len(source) < 1:
        return ''
    return dump(source, default_flow_style=False)


def parse_buildinfo(file):
    return parse_yaml(file)


def create_deploy_info(build_info):
    deploy_data = DeployInfo([], [])

    if 'branches' in build_info:
        for e in build_info['branches']:
            name = 'master'
            if 'name' in e:
                name = e['name']
            if 'build' in e:
                if e['build']:
                    deploy_data.build_branches.append(name)
            else:
                deploy_data.build_branches.append(name)
            if 'deploy' in e:
                if e['deploy']:
                    deploy_data.deploy_branches.append(name)
            else:
                pass

    if len(deploy_data.deploy_branches) == 0:
        deploy_data.deploy_branches.append('master')

    return (deploy_data.build_branches,
            deploy_data.deploy_branches)


def appveyor_gen_config(build_info):
    deploy_info = create_deploy_info(build_info)

    dependencies_list = ""

    for e in build_info['dependencies']: dependencies_list += e.split(":")[1] + ";"

    return {
        'version': '{build}',
        'skip_tags': True,
        'branches': {
            'only': deploy_info[0]
        },
        'configuration': ['Debug'],
        'platform': 'x64',
        'clone_depth': 1,
        'clone_script': [
            {
                'cmd': 'git clone -q --recursive --branch=%APPVEYOR_REPO_BRANCH% https://github.com/%APPVEYOR_REPO_NAME%.git %APPVEYOR_BUILD_FOLDER%',
            },
            {
                'cmd': 'git checkout -qf %APPVEYOR_REPO_COMMIT%'
            }
        ],
        'environment': {
            'BUILD_DIR': 'C:\\project\\%APPVEYOR_PROJECT_SLUG%',
            'CMAKE_BIN': 'C:\\Program Files\\CMake\\bin\\cmake.exe',
            'MAKEFILE_DIR': 'tools\\makers',
            'BUILDVARIANT': 'win32.amd64',
            'DEPENDENCIES': dependencies_list
        },
        'install': [
            {'ps': 'tools\\ci\\appveyor-deps.ps1'}
        ],
        'build_script': [
            {'ps': 'tools\\ci\\appveyor-build.ps1'}
        ],
        'artifacts': [
            {'path': '*.zip', 'name': 'Libraries'}
        ],
        'deploy': [
            {
                'provider': 'releases',
                'release': 'Appveyor Build $(APPVEYOR_BUILD_NUMBER)',
                'description': 'Automatic build by Appveyor',
                'artifact': 'Libraries',
                'prelease': True,
                'on': {
                    'appveyor_repo_tag': True
                },
            }
        ]
    }


def travis_gen_config(build_info):
    def create_env_matrix(current, build_info):
        out = []
        for dist in _TARGET_NAMES[current]:
            if dist not in build_info['platforms']:
                continue
            sub = _TARGET_NAMES[current]
            if type(sub) == dict:
                # These have sub-architectures, eg. amd64
                for arch in sub[dist]:
                    out.append('BUILDVARIANT=%s.%s' % (dist, arch))
            elif type(sub) == list:
                # These do not have architectures
                out.append('BUILDVARIANT=%s' % (dist,))
        out.sort()
        return out

    def create_build_matrix(targets, build_info):
        excludes = []
        matrices = []
        for target in targets:
            matrix = create_env_matrix(target, build_info)
            matrices += matrix
            for target2 in targets:
                if target2 != target:
                    for dist in matrix:
                        excludes.append({'os': target2, 'env': dist})
        return matrices, excludes

    targets = ['linux', 'osx']

    build_matrix = create_build_matrix(targets, build_info)
    deploy_data = create_deploy_info(build_info)

    script_loc = build_info['script_location']
    make_loc = build_info['makefile_location']
    dependencies = ''

    for dep in build_info['dependencies']:
        dependencies = '%s;%s' % (dep, dependencies)

    return {
        'language': 'cpp',
        'dist': 'trusty',
        'sudo': 'required',
        'services': ['docker'],
        'before_script': ['%s/install-dependencies.sh' % script_loc],
        'script': ['%s/build.sh' % script_loc],
        'os': targets,
        'compiles': ['clang'],
        'env':
            {
                'global': ['MAKEFILE_DIR=%s' % make_loc,
                           'DEPENDENCIES=%s' % dependencies],
                'matrix': build_matrix[0]
            },
        'matrix':
            {
                'exclude': build_matrix[1]
            },
        'branches': {
            'only': deploy_data[0]
        },
        'deploy': {
            'provider': 'releases',
            'api_key': '${GITHUB_TOKEN}',
            'file': 'libraries_$BUILDVARIANT.tar.gz',
            'skip_cleanup': True,
            'on': {
                'branch': deploy_data[1].pop()
            }
        }
    }


class ConfigCreator(object):
    def __init__(self, func, name, targets, filename):
        self.func = func
        self.name = name
        self.targets = targets
        self.filename = filename

    def compatible_with(self, target):
        return target in self.targets

    def get_filename(self):
        return self.filename

    def __str__(self):
        return self.name

    def __call__(self, *args):
        return self.func(*args)


def generate_config_files(services, build_info):
    service_configs = {}

    for service in services:
        for plat in build_info['platforms']:
            if service.compatible_with(plat):
                service_configs[service.get_filename()] = service(build_info)
                break

    return service_configs


def main():
    args = ArgumentParser(description='Generate Continuous Integration configuration files from build.yml')

    cur_dir = realpath(dirname(__file__))

    default_file = '%s/%s' % (cur_dir, 'build.yml')

    args.add_argument('input_file', metavar='input_file', type=str,
                      help='file used for configuring CI systems, defaults to %s' % default_file,
                      default=default_file)
    args.add_argument('--overwrite-ci-files', dest='overwrite', action='store_true',
                      help='overwrite existing CI config files',
                      default=False)

    args = args.parse_args()

    build_info = parse_yaml(args.input_file)

    ci_services = [ConfigCreator(travis_gen_config, 'Travis CI', travis_targets, '.travis.yml'),
                   ConfigCreator(appveyor_gen_config, 'Appveyor CI', appveyor_targets, 'appveyor.yml')]

    configs = generate_config_files(ci_services, build_info)

    for config in configs:
        trg_file = '%s/%s' % (cur_dir, config)
        if not args.overwrite and isfile(trg_file):
            print('Skipping %s, it exists' % trg_file)
            continue
        with open(trg_file, mode='w') as f:
            f.write(render_yaml(configs[config]))


if __name__ == '__main__':
    main()
