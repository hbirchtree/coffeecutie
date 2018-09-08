#!/usr/bin/env python3

from os.path import dirname, realpath, isfile

from argparse import ArgumentParser

from python.ci.common import parse_yaml, render_yaml, assertm, load_targets, flatten_map, MAKEFILE_DIR
from python.ci.appveyor import appveyor_gen_config
from python.ci.jenkins import jenkins_gen_config
from python.ci.travis import travis_gen_config

from python.common import try_get_key, build_yml_filespec

_yaml_template = 'cmake/Templates/coffee-build.yml'


DATAFORMAT_YAML = 0
DATAFORMAT_TEXT = 1


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
        for plat in flatten_map(try_get_key(build_info, 'platforms', [])):
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
    for config in sorted(list(configs.keys())):
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


appveyor_targets = []
jenkins_targets = []
travis_targets = []


CI_SERVICES = [ConfigCreator(travis_gen_config, 'Travis CI', travis_targets, '.travis.yml'),
               ConfigCreator(appveyor_gen_config, 'Appveyor CI', appveyor_targets, 'appveyor.yml'),
               ConfigCreator(jenkins_gen_config, 'Jenkins CI', jenkins_targets, '.jenkins.groovy',
                             data_format=DATAFORMAT_TEXT)]


def main():
    args = ArgumentParser(description='Generate Continuous Integration configuration files from build.yml')

    cur_dir = dirname(realpath(dirname(__file__)))

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

    # Parse targets for support config
    global appveyor_targets
    global jenkins_targets
    global travis_targets

    makefile_dir = '%s/toolchain/makers' % cur_dir

    travis_targets += load_targets(makefile_dir, 'linux').supports
    travis_targets += load_targets(makefile_dir, 'osx').supports

    jenkins_targets += load_targets(makefile_dir, 'linux').supports
    jenkins_targets += load_targets(makefile_dir, 'osx').supports
    jenkins_targets += load_targets(makefile_dir, 'windows').supports

    appveyor_targets += load_targets(makefile_dir, 'windows').supports

    if args.list_plats:
        print(render_yaml({
            'linux': load_targets(makefile_dir, 'linux').real_targets,
            'osx': load_targets(makefile_dir, 'windows').real_targets,
            'windows': load_targets(makefile_dir, 'windows').real_targets
        }))
        exit(0)

    global MAKEFILE_DIR
    MAKEFILE_DIR = makefile_dir

    build_info = parse_yaml(args.input_file)

    configs = generate_config_files(CI_SERVICES, build_info, cur_dir)

    process_configs(configs,
                    print_config=args.print_config,
                    overwrite=args.overwrite,
                    cur_dir=cur_dir)


if __name__ == '__main__':
    main()
