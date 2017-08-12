#!/usr/bin/env python3

from yaml import load, dump
try:
    from yaml import CLoader as Loader, CDumper as Dumper
except ImportError:
    from yaml import Loader, Dumper

from os.path import isfile, dirname, realpath, curdir

from argparse import ArgumentParser

from collections import namedtuple

from common import *

_yaml_template = 'cmake/Templates/coffee-build.yml'


DATAFORMAT_YAML = 0
DATAFORMAT_TEXT = 1


_TARGET_NAMES = {
    'linux': {
        'ubuntu': ['amd64', 'i686'],
        'fedora': ['amd64'],
        'emscripten': ['wasm', 'asmjs'],
        'steam': ['amd64'],
        'android': ['armv7a', 'armv8a', 'armv7a.kitkat'],
        'maemo': ['armel', 'i386'],
        'raspberry': ['armhf']
    },
    'osx': [
        'osx',
        'ios',
        'ios.x86_64'],
    'windows': {
        'win32': ['amd64'],
        'uwp': ['amd64']
    }
}

appveyor_targets = ['windows']
travis_targets = ['osx', 'ubuntu', 'fedora',
                  'emscripten', 'steam',
                  'android', 'maemo',
                  'raspberry']
jenkins_targets = list(_TARGET_NAMES.keys()) + list(_TARGET_NAMES['linux'].keys())

DeployInfo = namedtuple('DeployInfo', 'deploy_branches, build_branches')


def assertm(condition, message='BAD ASSERT'):
    if not condition:
        raise AssertionError(message)


def parse_yaml(source_file):
    with open(source_file, mode='r') as f:
        data = load(f.read(), Loader=Loader)
        return data


def render_yaml(source):
    yaml_struct = dump(source, default_flow_style=False)
    assert ( type(yaml_struct) == str)
    return yaml_struct


def git_get_origin(srcDir):
    import re
    with open('%s/.git/config' % srcDir, mode='r') as f:
        data = f.read()

        remote_patt = re.compile('\[remote .*')
        prop_patt = re.compile('^\s+([a-z]+)\s*=\s*([\W\w]+)$')

        block_started = True
        for line in data.split('\n'):
            if len(line) < 1:
                continue
            if remote_patt.match(line):
                block_started = True
                continue
            if block_started:
                match = prop_patt.findall(line)
                if not match:
                    block_started = False
                    continue
                if match[0][0] == 'url':
                    return match[0][1]
                print(match)


        raise RuntimeError('Failed to parse')


def try_get_key(d, k, v): # d=dictionary, k=key, v=default val
    try:
        return d[k]
    except KeyError:
        return v


def create_env_matrix(current, build_info):
    def get_targets(d1, d2, out, prefix):
        def zip_lists(v1, v2, out, prefix):
            for e1 in v1:
                assertm (type(e1) == str, '%s != %s' % (type(d1), str))
                for e2 in v2:
                    if e1 == e2:
                        assertm (type(e1) == type(e2), '%s != %s' % (type(e1), type(e2)))
                        out.add(prefix + e1)

        # Special cases:
        present_types = [type(d1), type(d2)]

        if list in present_types and dict in present_types:
            dtype = None
            ltype = None
            if type(d1) == dict:
                dtype = d1
                ltype = d2
            else:
                dtype = d2
                ltype = d1
            for vnt in dtype:
                if vnt in ltype:
                    for arch in dtype[vnt]:
                        out.add(prefix + vnt + '.' + arch)
            return
        elif dict in present_types and str in present_types:
            dtype = None
            stype = None
            if type(d1) == dict:
                dtype = d1
                stype = d2
            else:
                dtype = d2
                stype = d1
            assertm (stype == 'all', 'Invalid string in dictionary')
            get_targets(dtype, dtype, out, prefix)

        assertm (type(d1) == type(d2), '%s != %s' % (type(d1), type(d2)))
        if type(d1) == dict:
            for k in d1:
                if k in d2:
                    v1 = d1[k]
                    v2 = d2[k]
                    sub_prefix = None
                    if len(prefix) > 0:
                        sub_prefix = prefix + '.' + k
                    else:
                        sub_prefix = k + '.'
                    assertm (type(v1) == type(v2), '%s != %s' % (type(v1), type(v2)))
                    if type(v1) == dict:
                        get_targets(v1, v2, out, sub_prefix)
                    elif type(v1) == list:
                        zip_lists(v1, v2, out, sub_prefix)
                    else:
                        assertm (False, 'Bad type in array') # WRONG TYPE IN LIST
        elif type(d1) == list:
            zip_lists(d1, d2, out, prefix)


    plat_data = try_get_key(build_info, 'platforms', [])
    resident_data = _TARGET_NAMES[current]
    prefix = ''

    if current != 'linux':
        plat_data = try_get_key(plat_data, current, [])
        #prefix = current + '.'

    out = set()

    get_targets(resident_data, plat_data, out, prefix)

    out = list(out)
    out.sort()
    return out


def get_dep_list(build_info):
    dependencies = ''
    deps = try_get_key(build_info, 'dependencies', [])
    for dep in deps:
        dependencies = '%s;%s' % (dep, dependencies)
    return dependencies


def parse_buildinfo(file):
    return parse_yaml(file)


def create_deploy_info(build_info):
    deploy_data = DeployInfo([], [])

    for e in try_get_key(build_info, 'branches', []):
        name = 'master'
        if 'name' in e:
            name = e['name']
        if 'build' in e:
            if e['build']:
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


def appveyor_gen_config(build_info, srcDir):
    deploy_info = create_deploy_info(build_info)

    dependencies_list = get_dep_list(build_info)

    script_loc = build_info['script_location'].replace('/', '\\')
    make_loc = build_info['makefile_location'].replace('/', '\\')

    deploy_patterns = ''

    return {
        'version': '{build}',
        'skip_tags': True,
        'branches': {
            'only': deploy_info[0]
        },
        'configuration': ['Debug'],
        'platform': 'x64',
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
            'MAKEFILE_DIR': make_loc,
            'BUILDVARIANT': 'win32.amd64',
            'DEPENDENCIES': dependencies_list,
            'DEPLOY_PATTERNS': deploy_patterns,
            'GITHUB_TOKEN': {
                'secure': ''
            }
        },
        'install': [
            {'ps': '%s\\appveyor-deps.ps1' % script_loc}
        ],
        'build_script': [
            {'ps': '%s\\appveyor-build.ps1' % script_loc}
        ],
        'artifacts': [
            {'path': '*.zip', 'name': 'Libraries'}
        ],
        'deploy_script': [
            {'ps': '%s\\appveyor-deploy.ps1' % script_loc}
        ]
    }


def travis_gen_config(build_info, srcDir):
    def create_travis_matrix(current, build_info):
        env = create_env_matrix(current, build_info)

        for i, var in enumerate(env):
            env[i] = 'BUILDVARIANT=%s' % var

        return env

    def create_build_matrix(targets, build_info):
        excludes = []
        matrices = []
        for target in targets:
            matrix = create_env_matrix(target, build_info)
            for i, e in enumerate(matrix):
                matrix[i] = "BUILDVARIANT=%s" % e
            matrices += matrix
            for target2 in targets:
                if target2 != target:
                    for dist in matrix:
                        excludes.append({'os': target2, 'env': dist})
        return matrices, excludes

    targets = ['linux', 'osx']

    build_matrix = create_build_matrix(targets, build_info)

    deploy_data = create_deploy_info(build_info)

    script_loc = try_get_key(build_info, 'script_location', 'ci')
    make_loc = try_get_key(build_info, 'makefile_location', 'ci')

    dependencies = get_dep_list(build_info).replace(";", "%")

    return {
        'language': 'cpp',
        'dist': 'trusty',
        'group': 'deprecated-2017Q2',
        'sudo': 'required',
        'services': ['docker'],
        'notifications': {
            'email': True
        },
        'before_script': ['%s/travis-deps.sh' % script_loc],
        'script': ['%s/travis-build.sh' % script_loc],
        'os': targets,
        'compiles': ['clang'],
        'env':
            {
                'global': ['MAKEFILE_DIR=%s' % make_loc,
                           'DEPENDENCIES=%s' % dependencies,
                           'PYTHONPATH=/usr/lib/python3/dist-packages'],
                'matrix': build_matrix[0]
            },
        'matrix':
            {
                'exclude': build_matrix[1]
            },
        'branches': {
            'only': deploy_data[0]
        },
        'after_success': ['%s/travis-deploy.sh' % script_loc]
    }


def jenkins_gen_config(build_info, src_dir):
    def mk_groovy_list(l):
        glist = ''
        for e in l:
            glist = "'%s', %s" % (e, glist)
        return glist

    def mk_shell_list(l):
        glist = ''
        for e in l:
            glist = "%s;%s" % (e, glist)
        return glist

    def sshgit_to_https(url):
        import re
        patt = re.compile('git@(.+):(.+)')

        match = patt.findall(url)

        if match:
            return 'https://%s/%s' % (match[0][0], match[0][1])
        else:
            return url

    template_dir = '%s/cmake/Templates' % src_dir

    deps = get_dep_list(build_info)

    linux_targets = create_env_matrix('linux', build_info)
    osx_targets = create_env_matrix('osx', build_info)
    windows_targets = create_env_matrix('windows', build_info)

    repo_url = git_get_origin(src_dir)

    repo_url = sshgit_to_https(repo_url)

    variables = {
        'PROJECT_NAME': try_get_key(build_info, 'name', 'coffee'),

        'LINUX_TARGETS': mk_groovy_list(linux_targets),
        'OSX_TARGETS': mk_groovy_list(osx_targets),
        'WINDOWS_TARGETS': mk_groovy_list(windows_targets),

        'DEPENDENCIES': deps,
        'REPO_URL': repo_url,

        'SCRIPT_DIR': try_get_key(build_info, 'script_location', 'ci'),
        'MAKEFILE_DIR': try_get_key(build_info, 'makefile_location', 'ci')
    }

    data = ''
    with open('%s/JenkinsTemplate.groovy' % template_dir, mode='r') as f:
        data = f.read()

    return configure_string(data, variables)


class ConfigCreator(object):
    def __init__(self, func, name,
                 targets, filename,
                 data_format=DATAFORMAT_YAML):
        self.func = func
        self.name = name
        self.targets = targets
        self.filename = filename
        self.data_format = data_format

    def compatible_with(self, target):
        return target in self.targets

    def get_filename(self):
        return self.filename

    def get_data_format(self):
        return self.data_format

    def __str__(self):
        return self.name

    def __call__(self, *args):
        return self.func(*args)


def generate_config_files(services, build_info, source_dir):
    service_configs = {}

    for service in services:
        for plat in try_get_key(build_info, 'platforms', []):
            if service.compatible_with(plat):
                try:
                    service_configs[service.get_filename()] = (service(build_info, source_dir), service)
                except FileNotFoundError as e:
                    print('%s: %s: %s: %s' % (service, e.__class__.__name__, e.filename, e.strerror))
                except RuntimeError as e:
                    print('%s: %s: %s' % (service, e.__class__.__name__, e.args))
                break

    return service_configs


def process_configs(configs, print_config=False, overwrite=False, cur_dir='.'):
    for config in configs:
        trg_file = '%s/%s' % (cur_dir, config)

        data, srv = configs[config]

        if srv.data_format == DATAFORMAT_TEXT:
            pass
        elif srv.data_format == DATAFORMAT_YAML:
            if isfile(trg_file) and not overwrite:
                data2 = parse_yaml(trg_file)
                #data = {**data, **data2} # If there is only Python 3.5+
                data.update(data2)

            data = render_yaml(data)
        else:
            print('Unknown output data format: %s' % srv.data_format)
            continue

        assert ( type(data) == str )

        if print_config:
            print('Configuration for %s:' % config)
            print('-' * 80)
            print(data)
            print('-' * 80)
        else:
            with open(trg_file, mode='w') as f:
                f.write(data)


CI_SERVICES = [ConfigCreator(travis_gen_config, 'Travis CI', travis_targets, '.travis.yml'),
               ConfigCreator(appveyor_gen_config, 'Appveyor CI', appveyor_targets, 'appveyor.yml'),
               ConfigCreator(jenkins_gen_config, 'Jenkins CI', jenkins_targets, '.jenkins.groovy',
                             data_format=DATAFORMAT_TEXT)]


def main():
    args = ArgumentParser(description='Generate Continuous Integration configuration files from build.yml')

    cur_dir = realpath(dirname(__file__))

    default_file = '%s/%s' % (cur_dir, build_yml_filespec)

    args.add_argument('input_file', metavar='input_file', type=str, nargs='*',
                      help='file used for configuring CI systems, defaults to %s' % default_file,
                      default=default_file,)
    args.add_argument('--overwrite-ci-files', dest='overwrite', action='store_true',
                      help='overwrite existing CI config files',
                      default=False)

    args.add_argument('--list-platforms', dest='list_plats', action='store_true',
                      help='list valid platforms and architectures',
                      default=False)

    args.add_argument('--print-configuration', dest='print_config', action='store_true',
                      help='print generated configuration to stdout',
                      default=False)

    args = args.parse_args()

    if args.list_plats:
        print(render_yaml(_TARGET_NAMES))
        exit(0)

    build_info = parse_yaml(args.input_file)

    configs = generate_config_files(CI_SERVICES, build_info, realpath(dirname(__file__)))

    process_configs(configs,
                    print_config=args.print_config,
                    overwrite=args.overwrite,
                    cur_dir=realpath(dirname(__file__)))


if __name__ == '__main__':
    main()
