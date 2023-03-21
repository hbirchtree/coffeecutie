#!/usr/bin/env python3

import json
import subprocess
from argparse import ArgumentParser
from glob import glob
from os import makedirs
from os.path import dirname, getmtime, exists
from shutil import which


PROGRAMS = {}


def needs_update(output: str, dependencies: list):
    if not exists(output):
        return True
    out_ts = getmtime(output)
    dep_ts = [ getmtime(dep) for dep in dependencies ]
    return sum([ 1 if dep > out_ts else 0 for dep in dep_ts ]) > 0


def run(program, *args):
    program = PROGRAMS[program]
    process_args = [program, *args]
    subprocess.call(process_args)


def compile_shaders(values: dict, root_directory: str, out_dir: str):
    files = values['files']
    for variant in values['variants']:
        profile, version = variant.split(':')
        for file in files:
            stem_name, extension = file.split('.')
            in_file = f'{root_directory}/{file}'
            out_file = f'{out_dir}/{stem_name}.{profile}{version}.{extension}'
            if not needs_update(out_file, [in_file]):
                continue
            print(f' * Emitting {file} as {profile} {version}')
            run(
                'ShaderCooker',
                '-f',
                in_file,
                '-p', f'{profile}',
                '-V', f'{version}',
                '-s', f'{extension}',
                '-o', out_file
            )
    files = []
    assemblies = values['assemblies']
    for assembly in assemblies:
        out_file = f'{out_dir}/{assembly}.spv'
        shaders = assemblies[assembly]
        file_args = []
        in_files = []
        for file in shaders:
            _, extension = file.split('.')
            in_file = f'{root_directory}/{file}'
            file_args.extend(['-s', f'{extension}', in_file])
            in_files.append(in_file)
        if not needs_update(out_file, in_files):
            continue
        print(f' * Emitting shader assembly {assembly}.spv <- {shaders}')
        extra_args = []
        if values['strip_assemblies']:
            extra_args.append('--strip-debug')
        run(
            'ShaderCooker',
            '-f',
            '-M',
            '-B',
            *extra_args,
            *file_args,
            '-o', out_file
        )


def process_resources(definition: dict, root_directory: str, out_dir: str):
    for key in definition:
        if key == 'shaders':
            compile_shaders(definition[key], root_directory, out_dir)


if __name__ == '__main__':
    parser = ArgumentParser(__file__)
    parser.add_argument('-p', '--path', dest='paths', action='append')
    parser.add_argument('-o', '--output', dest='output')
    parser.add_argument('-P', '--program', dest='programs', action='append')
    args = parser.parse_args()
    for program_pair in args.programs:
        program, path = program_pair.split('=')
        PROGRAMS[program] = path
    out_dir = args.output
    makedirs(out_dir, exist_ok=True)
    for path in args.paths:
        resource_defs = glob(f'{path}/**/resources.json', recursive=True)
        for resource_def in resource_defs:
            root_dir = dirname(resource_def)
            root_dir = '.' if root_dir == '' else root_dir
            definitions = json.load(open(resource_def))
            process_resources(definitions, root_dir, out_dir)
