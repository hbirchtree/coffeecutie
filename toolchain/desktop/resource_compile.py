#!/usr/bin/env python3

import json
import subprocess
from argparse import ArgumentParser
from glob import glob
from os import makedirs
from os.path import dirname, getmtime, exists
from shutil import which
from hashlib import sha256


PROGRAMS = {}


def needs_update(output: str, dependencies: list):
    if not exists(output):
        return True
    out_ts = getmtime(output)
    dep_ts = [ getmtime(dep) for dep in dependencies ]
    return sum([ 1 if dep > out_ts else 0 for dep in dep_ts ]) > 0


def shader_dependencies(shader_file: str, cache_directory: str):
    glslang = PROGRAMS['glslang'] if 'glslang' in PROGRAMS else 'glslangValidator'
    path_hash = sha256(shader_file.encode()).hexdigest()
    dep_file = f'{cache_directory}/{path_hash}.deps'

    def get_deps():
        with open(dep_file) as f:
            deps = f.read().split(': ')[-1].replace('\n', '').split(' ')
            return deps
        return []

    if exists(dep_file):
        return get_deps()
    subprocess.call([
        glslang,
        '--depfile', dep_file,
        '-G100',
        shader_file])

    return get_deps()


def run(program, *args):
    program = PROGRAMS[program]
    process_args = [program, *args]
    print(' '.join(process_args))
    subprocess.call(process_args)


def compile_shaders(
        values: dict,
        cache_directory: str,
        root_directory: str,
        out_directory: str,
        target: str,
        build_mode: str,
        extra_dependencies: list):
    files = values['files']
    variants = values['variants']
    assemblies = values['assemblies']
    opt_level = '--O0'
    if 'matrix' in values and target in values['matrix']:
        allowed_variants = [ x for x in values['matrix'][target] ]
        variants = [ x
            for x in values['variants'] if len([ v for v in allowed_variants if x.startswith(v) ]) > 0
        ]
        if 'spv' not in allowed_variants:
            assemblies = []
    if 'optimization_level' in values:
        level = values['optimization_level']
        if level == 'fast':
            opt_level = '--Ofast'
        if level == 'size':
            opt_level = '--Osize'
    for variant in variants:
        profile, version = variant.split(':')
        for file in files:
            stem_name, extension = file.split('.')
            in_file = f'{root_directory}/{file}'
            out_file = f'{out_directory}/{stem_name}.{profile}{version}.{extension}'
            file_dependencies = shader_dependencies(in_file, cache_directory)
            if not needs_update(out_file, [in_file] + extra_dependencies + file_dependencies):
                continue
            print(f' * Emitting {file} as {profile} {version}')
            run(
                'ShaderCooker',
                '-f',
                in_file,
                opt_level,
                '-p', profile,
                '-V', version,
                '-s', extension,
                '-o', out_file
            )
    for assembly in assemblies:
        out_file = f'{out_directory}/{assembly}.spv'
        shaders = assemblies[assembly]
        file_args = []
        in_files = [] + extra_dependencies
        for file in shaders:
            _, extension = file.split('.')
            in_file = f'{root_directory}/{file}'
            file_args.extend(['-s', f'{extension}', in_file])
            in_files.append(in_file)
            in_files = in_files + shader_dependencies(in_file, cache_directory)
        if not needs_update(out_file, in_files):
            continue
        print(f' * Emitting shader assembly {assembly}.spv <- {shaders}')
        extra_args = []
        if values['strip_assemblies']:
            extra_args.append('--strip-debug')
        run(
            'ShaderCooker',
            '--force',
            '-M',
            '-B',
            opt_level,
            *extra_args,
            *file_args,
            '-o', out_file
        )


def encode_textures(
        values: dict,
        cache_directory: str,
        root_directory: str,
        out_directory: str,
        target: str,
        build_mode: str,
        extra_dependencies: list):
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

    def _predict_names(basename):
        return glob(f'{out_directory}/{basename}.*')

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
        outputs = _predict_names(basename)
        res = [needs_update(x, [rendered_file]) for x in outputs]
        for out_of_date in res:
            if out_of_date:
                break
        else:
            return
        compress_mode = 'fast' if 'Deb' in build_mode else 'release'
        run(
            'TextureCompressor',
            *[ f'--codec={codec}:{fmt}' for codec, fmt in codecs ],
            *[ f'--resolution={res}' for res in resolutions ],
            f'--mode={compress_mode}',
            rendered_file,
            f'--output={out_directory}/'
            )

    for file in values['files']:
        resolutions = _generate_mipmaps(file['mipmap_range'])
        codecs = []
        for v in variants:
            selected = [ x for x in file['formats'] if x in variants[v] ]
            for fmt in selected:
                codecs.append((v, fmt))
        _process_file(**file, codecs=codecs, resolutions=resolutions)


def process_resources(definition: dict, extra_dependencies: list, **kwargs):
    for key in definition:
        if key == 'shaders':
            compile_shaders(definition[key],
                            extra_dependencies=extra_dependencies + [PROGRAMS['ShaderCooker']],
                            **kwargs)
        if key == 'textures':
            encode_textures(definition[key],
                            extra_dependencies=extra_dependencies + [PROGRAMS['TextureCompressor']],
                            **kwargs)


if __name__ == '__main__':
    parser = ArgumentParser(__file__)
    parser.add_argument('-p', '--path', dest='paths', action='append')
    parser.add_argument('-o', '--output', dest='output')
    parser.add_argument('--cache', dest='cache_dir', required=True)
    parser.add_argument('-P', '--program', dest='programs', action='append')
    parser.add_argument('-t', '--target', dest='target', required=True)
    parser.add_argument('-b', '--build-mode', dest='build_mode', required=True)
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
            # Add files and tools to dependencies
            process_resources(
                definitions,
                cache_directory=args.cache_dir,
                root_directory=root_dir,
                out_directory=args.output,
                target=args.target,
                build_mode=args.build_mode,
                extra_dependencies=[resource_def])
