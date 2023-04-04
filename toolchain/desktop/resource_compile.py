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
#    print(' '.join(process_args))
    subprocess.call(process_args)


def compile_shaders(
        values: dict,
        cache_directory: str,
        root_directory: str,
        out_directory: str,
        target: str):
    files = values['files']
    variants = values['variants']
    assemblies = values['assemblies']
    if 'matrix' in values and target in values['matrix']:
        allowed_variants = [ x for x in values['matrix'][target] ]
        variants = [ x
            for x in values['variants'] if len([ v for v in allowed_variants if x.startswith(v) ]) > 0
        ]
        if 'spv' not in allowed_variants:
            assemblies = []
    for variant in variants:
        profile, version = variant.split(':')
        for file in files:
            stem_name, extension = file.split('.')
            in_file = f'{root_directory}/{file}'
            out_file = f'{out_directory}/{stem_name}.{profile}{version}.{extension}'
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
    for assembly in assemblies:
        out_file = f'{out_directory}/{assembly}.spv'
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


def encode_textures(
        values: dict,
        cache_directory: str,
        root_directory: str,
        out_directory: str,
        target: str):
    variants = values['variants']
    if 'matrix' in values and target in values['matrix']:
        to_remove = []
        for v in variants:
            if v not in values['matrix'][target]:
                to_remove.append(v)
        for v in to_remove:
            del variants[v]

    def _generate_mipmaps(mipmap_range: list):
        maximum, minimum = mipmap_range
        current = maximum
        out = []
        while current >= minimum:
            out.append(int(current))
            current = current / 2
        return out

    def _process_file(
            source: str,
            punchthrough_color: str,
            codecs: list,
            resolutions: list,
            **kwargs):
        descriptor = {
            'file': source,
            'codecs': [ f'{codec}:{fmt}' for codec, fmt in codecs],
            'punchthrough_color': punchthrough_color,
            'resolutions': resolutions
        }
        extension = source.split('.')[-1]
        basename = '.'.join(source.split('.')[:-1])
        print(f'{basename}[{extension}] -> {descriptor}')
        rendered_file = f'{cache_directory}/{basename}.png'
        if extension == 'svg':
            if needs_update(rendered_file, [f'{root_directory}/{source}']):
                run(
                    'Inkscape',
                    f'{root_directory}/{source}',
                    f'--export-filename={rendered_file}',
                    f'--export-width={resolutions[0]}'
                )
        else:
            rendered_file = f'{root_directory}/{source}'
        run(
            'TextureCompressor',
            *[ f'-c {codec}:{fmt}' for codec, fmt in codecs ],
            *[ f'-r {res}' for res in resolutions ],
            rendered_file,
            f'-o {out_directory}/{basename}' + '.{resolution}.{codec}'
            )

    for file in values['files']:
        resolutions = _generate_mipmaps(file['mipmap_range'])
        codecs = []
        for v in variants:
            selected = [ x for x in file['formats'] if x in variants[v] ]
            for fmt in selected:
                codecs.append((v, fmt))
        _process_file(**file, codecs=codecs, resolutions=resolutions)


def process_resources(definition: dict, **kwargs):
    for key in definition:
        if key == 'shaders':
            compile_shaders(definition[key], **kwargs)
        if key == 'textures':
            encode_textures(definition[key], **kwargs)


if __name__ == '__main__':
    parser = ArgumentParser(__file__)
    parser.add_argument('-p', '--path', dest='paths', action='append')
    parser.add_argument('-o', '--output', dest='output')
    parser.add_argument('--cache', dest='cache_dir', required=True)
    parser.add_argument('-P', '--program', dest='programs', action='append')
    parser.add_argument('-t', '--target', dest='target', required=True)
    args = parser.parse_args()
    for program_pair in args.programs:
        program, path = program_pair.split('=')
        PROGRAMS[program] = path
    target = args.target
    makedirs(args.output, exist_ok=True)
    makedirs(args.cache_dir, exist_ok=True)
    for path in args.paths:
        resource_defs = glob(f'{path}/**/resources.json', recursive=True)
        for resource_def in resource_defs:
            root_dir = dirname(resource_def)
            root_dir = '.' if root_dir == '' else root_dir
            definitions = json.load(open(resource_def))
            process_resources(
                definitions,
                cache_directory=args.cache_dir,
                root_directory=root_dir,
                out_directory=args.output,
                target=args.target)
