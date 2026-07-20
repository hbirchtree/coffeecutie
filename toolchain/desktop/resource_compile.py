#!/usr/bin/env python3

import json
import subprocess
from argparse import ArgumentParser
from concurrent.futures import ThreadPoolExecutor, as_completed
from glob import glob
from os import makedirs, cpu_count, environ
from os.path import dirname, getmtime, exists
from shutil import copyfile, which
from hashlib import sha256


PROGRAMS = {}

# Per-file progress output is noise in normal builds; opt in with VERBOSE=1.
VERBOSE = environ.get('VERBOSE', '') not in ('', '0')


def log(*args, **kwargs):
    if VERBOSE:
        print(*args, **kwargs)


# Thread pool for dispatching external compiler processes concurrently.
# Set in __main__ once job count is known.
EXECUTOR = None
FUTURES = []


class RunError(Exception):
    def __init__(self, cmd, stdout, stderr, returncode=None):
        super().__init__(' '.join(cmd))
        self.cmd = cmd
        self.stdout = stdout
        self.stderr = stderr
        self.returncode = returncode


def submit(fn, *args, **kwargs):
    """Schedule a work unit on the pool, tracking its future."""
    FUTURES.append(EXECUTOR.submit(fn, *args, **kwargs))

DEFAULT_TEX_VARIANTS = {
    'astc': ['rgba'],
    'bc1': ['rgb'],
    'bc2': ['rgba'],
    'bc3': ['rgba'],
    'bc4': ['r'],
    'bc5': ['rg', 'ra'],
    'bc7': ['rgba'],
    'etc1': ['rgb'],
    'etc2': ['rgba', 'rgb', 'rg', 'r', 'ra'],
    'png': ['rgba', 'rgb', 'rg', 'r', 'ra'],
    'pvrtc1': ['rgba'],
    'raw': ['rgba', 'rgb', 'rg', 'r', 'ra'],
}

DEFAULT_TEX_MATRIX = {
    'Android': [
        'astc', # Mobile only
        'etc1', # For old PowerVR/Mali chips
        'etc2',
        'pvrtc1', # PowerVR only
        'raw',
    ],
    'Android:armv7-a': [
        'etc1', # For old PowerVR/Mali chips
        'etc2',
        'pvrtc1', # PowerVR only
    ],
    'Android:arm64': [
        'astc', # Mobile only
        'bc1', # Only Tegra seems to support BCn
        'bc2', # ...
        'bc3', # ...
        'bc4', # ...
        'bc5', # ...
        'bc6', # ...
        'bc7', # ... but it supports it GOOD!
        'etc2',
        'pvrtc1', # PowerVR only
    ],
    'Darwin': ['bc1', 'bc2', 'bc3', 'raw'],
    'Emscripten': [
        'astc',
        'bc1',
        'bc2',
        'bc3',
        'etc1',
        'etc2',
    ],
    'iOS': [
        'pvrtc1',
        'raw',
    ],
    'Linux:armv7l': [
        'astc', # For Mali
        'etc1', # Last resort
        'etc2', # Baseline OpenGL ES 3.0
        'pvrtc1', # For PowerVR
    ],
    'Linux:armv8': [
        'astc', # For Mali
        'etc2', # Baseline OpenGL ES 3.0
        'pvrtc1', # For PowerVR
        'raw',
    ],
    'Linux:x86_64': [
        'bc1',
        'bc2',
        'bc3',
        'bc4',
        'bc5',
        'bc6',
        'bc7',
        'etc2', # ETC2 is supported, but not well
        'raw',
    ],
    'Linux': [
        'bc1',
        'bc2',
        'bc3',
        'bc4',
        'bc5',
        'bc6',
        'bc7',
        'etc2',
        'raw',
    ],
    'Windows': [
        'bc1',
        'bc2',
        'bc3',
        'bc4',
        'bc5',
        'bc6',
        'bc7',
        'etc2',
        'raw',
    ]
}

DEFAULT_SHADER_MATRIX = {
    'Android:8': ['es:200'], # Android 2.2
    'Android:18': ['es:200', 'es:300'], # Android 4.3
    'Android:21': ['es:200', 'es:300', 'es:310'], # Android 5.0
    'Android:24': ['es:200', 'es:300', 'es:310', 'es:320'], # Android 7.0
    'Android:30': ['es:300', 'es:310', 'es:320', 'core:460'], # NVIDIA Shield runs Android 11
    'Android:31': ['es:300', 'es:310', 'es:320'], # Android 12+ doesn't need to generate GL 4.6 shaders
    'Darwin': ['core:410'],
    'Emscripten': ['es:300'],
    'iOS': ['es:300'],
    'Linux:armv7l': ['es:200', 'es:300'],
    'Linux:armv8': ['es:'],
    # Linux, omitted because it supports all
    'Windows': ['spv', 'core:'],
}

running_processes = []


def needs_update(output: str, dependencies: list):
    if not exists(output):
        return True
    out_ts = getmtime(output)
    return sum([ 1 if (exists(dep) and getmtime(dep) > out_ts) else 0
                 for dep in dependencies ]) > 0


def shader_dependencies(shader_file: str, cache_directory: str):
    glslang = PROGRAMS['glslang'] if 'glslang' in PROGRAMS else 'glslangValidator'
    path_hash = sha256(shader_file.encode()).hexdigest()
    dep_file = f'{cache_directory}/{path_hash}.deps'

    def get_deps():
        with open(dep_file) as f:
            deps = f.read().split(': ')[-1].replace('\n', '').split(' ')
            return deps
        return []

    dep_run = subprocess.run(
        [glslang, '--depfile', dep_file, '-G100', shader_file],
        capture_output=True,
        text=True)
    if dep_run.returncode != 0:
        print(dep_run.stdout, dep_run.stderr)

    return get_deps()


def run(program, *args):
    program = PROGRAMS[program]
    process_args = [program, *args]
    ret = subprocess.run(process_args, capture_output=True)
    if ret.returncode != 0:
        # Raise instead of exit() so the failure propagates out of the
        # worker thread and is reported when futures are drained.
        raise RunError(
            process_args, ret.stdout.decode(), ret.stderr.decode(),
            ret.returncode)


def _get_best_match(matrix, target, arch, api):
    # 1. Try target:arch:api (with API floor)
    if arch != 'none' and api != 0:
        best_api = -1
        best_val = None
        for key, val in matrix.items():
            parts = key.split(':')
            if len(parts) == 3 and parts[0] == target and parts[1] == arch and parts[2].isdigit():
                k_api = int(parts[2])
                if api >= k_api and k_api > best_api:
                    best_api = k_api
                    best_val = val
        if best_val is not None:
            return best_val

    # 2. Try target:api (with API floor) - Higher priority than pure arch
    if api != 0:
        best_api = -1
        best_val = None
        for key, val in matrix.items():
            parts = key.split(':')
            if len(parts) == 2 and parts[0] == target and parts[1].isdigit():
                k_api = int(parts[1])
                if api >= k_api and k_api > best_api:
                    best_api = k_api
                    best_val = val
        if best_val is not None:
            return best_val

    # 3. Try target:arch (exact)
    if arch != 'none':
        key = f'{target}:{arch}'
        if key in matrix:
            return matrix[key]

    # 4. Try target (exact)
    if target in matrix:
        return matrix[target]

    return None


def compile_shaders(
        values: dict,
        cache_directory: str,
        root_directory: str,
        out_directory: str,
        target: str,
        arch: str,
        api: int,
        build_mode: str,
        extra_dependencies: list):
    files = values['files']
    variants = values['variants']
    assemblies = values['assemblies']
    opt_level = '--O0'

    allowed_variants = _get_best_match(values.get('matrix', {}), target, arch, api)
    if allowed_variants is None:
        allowed_variants = _get_best_match(DEFAULT_SHADER_MATRIX, target, arch, api)

    if allowed_variants is not None:
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
            log(f' * Emitting {file} as {profile} {version}')
            submit(
                run,
                'ShaderCooker',
                '--force',
                '--compact',
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
        log(f' * Emitting shader assembly {assembly}.spv <- {shaders}')
        extra_args = []
        if values['strip_assemblies']:
            extra_args.append('--strip-debug')
        submit(
            run,
            'ShaderCooker',
            '--force',
            '--compact',
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
        arch: str,
        api: int,
        build_mode: str,
        extra_dependencies: list):
    if 'gxtexconv' in PROGRAMS:
        for entry in values['files']:
            _process_gx_texture(
                entry, cache_directory, root_directory, out_directory,
                extra_dependencies + [PROGRAMS['gxtexconv']])
        return

    variants = dict(DEFAULT_TEX_VARIANTS) # values['variants']

    allowed_variants = _get_best_match(values.get('matrix', {}), target, arch, api)
    if allowed_variants is None:
        allowed_variants = _get_best_match(DEFAULT_TEX_MATRIX, target, arch, api)

    if allowed_variants is not None:
        to_remove = []
        for v in variants:
            if v not in allowed_variants:
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
        file_dir = dirname(source)
        # print(f'{basename}[{extension}] -> {descriptor}')
        rendered_file = f'{cache_directory}/{basename}.png'
        if extension != 'svg':
            rendered_file = f'{root_directory}/{source}'
        def _predict_names(basename):
            return [f'{out_directory}/{basename}.0.{codec}' for codec, _ in codecs]
        outputs = _predict_names(basename)
        res = [needs_update(x, [rendered_file]) for x in outputs]
        for out_of_date in res:
            if out_of_date:
                break
        else:
            if len(res) > 0:
                return
        log(f'* Processing file {source} -> {codecs}')
        compress_mode = 'fast' if 'Deb' in build_mode else 'release'

        # SVG render must precede compression, so keep both in one task.
        def _task():
            if extension == 'svg' and needs_update(
                    rendered_file, [f'{root_directory}/{source}']):
                run(
                    'Inkscape',
                    f'{root_directory}/{source}',
                    f'--export-filename={rendered_file}',
                    f'--export-width={resolutions[0]}'
                )
            run(
                'TextureCompressor',
                *[ f'--codec={codec}:{fmt}' for codec, fmt in codecs ],
                *[ f'--resolution={res}' for res in resolutions ],
                f'--mode={compress_mode}',
                rendered_file,
                f'--output={out_directory}/{file_dir}'
                )
        submit(_task)

    for file in values['files']:
        resolutions = _generate_mipmaps(file['mipmap_range'])
        codecs = []
        for v in variants:
            if v not in DEFAULT_TEX_MATRIX[target]:
                continue
            selected = [ x for x in file['formats'] if x in variants[v] ]
            for fmt in selected:
                codecs.append((v, fmt))
        _process_file(**file, codecs=codecs, resolutions=resolutions)


def copy_files(
        values: dict,
        cache_directory: str,
        root_directory: str,
        out_directory: str,
        target: str,
        arch: str,
        api: int,
        build_mode: str,
        extra_dependencies: list):
    for file in values:
        submit(copyfile, f'{root_directory}/{file}', f'{out_directory}/{file}')




def _infer_gx_format(formats: list):
    """Pick a GX texture format from a regular texture entry's channel list."""
    chars = set(''.join(formats))
    has_alpha = 'a' in chars
    color = chars - {'a'}
    if color == {'r'}:
        return 'IA8' if has_alpha else 'I8'
    # rgb / rg colour: RGB5A3 keeps alpha; otherwise compress with CMPR.
    return 'RGB5A3' if has_alpha else 'CMPR'


def _process_gx_texture(
        entry: dict,
        cache_directory: str,
        root_directory: str,
        out_directory: str,
        extra_dependencies: list):
    # devkitPro gxtexconv texture format names -> colfmt index (see `gxtexconv`
    # usage). CMPR is the GX block-compressed (S3TC/DXT1-like) 4bpp format.
    GX_TEX_FORMATS = {
        'I4':     0,
        'I8':     1,
        'IA4':    2,
        'IA8':    3,
        'RGB565': 4,
        'RGB5A3': 5,
        'RGBA8':  6,
        'CI4':    8,
        'CI8':    9,
        'CMPR':  14,
    }
    GX_TEX_LIMITS = {
        'I4':     1024,
        'I8':     1024,
        'IA4':    1024,
        'IA8':     512,
        'RGB565':  512,
        'RGB5A3':  512,
        'CI4':    1024,
        'CI8':     512,
        'CMPR':   1024,
    }
    """Convert one regular texture entry to a GameCube/Wii .tpl via gxtexconv.
    SVG sources are rasterized with Inkscape first (as on the desktop path)."""
    source = entry['source']
    fmt_name = (entry.get('gx_format') or _infer_gx_format(
        entry.get('formats', ['rgba']))).upper()
    fmt = GX_TEX_FORMATS.get(fmt_name)
    if fmt is None:
        raise RunError(
            ['gxtexconv'], '',
            f'unknown GX texture format "{fmt_name}" for {source}')

    extension = source.split('.')[-1]
    basename = '.'.join(source.split('.')[:-1])
    src_file = f'{root_directory}/{source}'
    out_file = f'{out_directory}/{basename}.tpl'
    makedirs(dirname(out_file), exist_ok=True)

    mipmap_range = entry.get('mipmap_range', [0, 0])
    max_res = mipmap_range[0] if mipmap_range else 0
    fmt_limit = GX_TEX_LIMITS.get(fmt_name) or 0
    if max_res > fmt_limit:
        max_res = fmt_limit
    rendered = src_file
    if extension == 'svg':
        rendered = f'{cache_directory}/{basename}.png'
        makedirs(dirname(rendered), exist_ok=True)

    if not needs_update(out_file, [src_file] + extra_dependencies):
        return

    log(f'* GX texture {source} -> {basename}.tpl ({fmt_name})')

    def _task():
        if extension == 'svg':
            run('Inkscape', src_file,
                f'--export-filename={rendered}',
                f'--export-width={max_res}')
        args = ['-i', rendered, '-o', out_file, f'colfmt={fmt}']
        if mipmap_range and mipmap_range[0] != mipmap_range[1]:
            args.append('mipmap=yes')
        run('gxtexconv', *args)

    submit(_task)


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
        if key == 'copy':
            copy_files(definition[key],
                       extra_dependencies=extra_dependencies,
                       **kwargs)


if __name__ == '__main__':
    parser = ArgumentParser(__file__)
    parser.add_argument('-p', '--path', dest='paths', action='append')
    parser.add_argument('-o', '--output', dest='output')
    parser.add_argument('--cache', dest='cache_dir', required=True)
    parser.add_argument('-P', '--program', dest='programs', action='append')
    parser.add_argument('-t', '--target', dest='target', required=True)
    parser.add_argument('-a', '--arch', dest='arch', default='none')
    parser.add_argument('-A', '--api', dest='api', type=int, default=0)
    parser.add_argument('-b', '--build-mode', dest='build_mode', required=True)
    parser.add_argument('-j', '--jobs', dest='jobs', type=int,
                        default=cpu_count() or 1,
                        help='Max concurrent compiler processes')
    args = parser.parse_args()
    EXECUTOR = ThreadPoolExecutor(max_workers=max(1, args.jobs))
    for program_pair in (args.programs or []):
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
            # Create pre-requisite directories if needed
            if 'out_dirs' in definitions:
                for directory in definitions['out_dirs']:
                    makedirs(f'{args.output}/{directory}', exist_ok=True)
                    makedirs(f'{args.cache_dir}/{directory}', exist_ok=True)
            # Add files and tools to dependencies
            process_resources(
                definitions,
                cache_directory=args.cache_dir,
                root_directory=root_dir,
                out_directory=args.output,
                target=args.target,
                arch=args.arch,
                api=args.api,
                build_mode=args.build_mode,
                extra_dependencies=[resource_def])

    # Wait for all dispatched compiler processes and report any failures.
    failed = False
    for future in as_completed(FUTURES):
        try:
            future.result()
        except RunError as e:
            print(' '.join(e.cmd))
            # A negative returncode means the process was killed by a
            # signal (Python/subprocess convention: -N for signal N), e.g.
            # -4 (SIGILL) from a CPU-target mismatch -- stdout/stderr are
            # typically empty in that case since the process never got to
            # write anything, which otherwise looks just like a silent,
            # unexplained failure. Surface it explicitly instead of
            # printing an empty "ERROR:\n\n\n".
            if e.returncode is not None and e.returncode < 0:
                import signal
                try:
                    sig_name = signal.Signals(-e.returncode).name
                except ValueError:
                    sig_name = f'signal {-e.returncode}'
                print(f'ERROR: killed by {sig_name} (returncode={e.returncode})')
            print(f'ERROR:\n{e.stdout}\n{e.stderr}')
            failed = True
    EXECUTOR.shutdown()
    if failed:
        exit(1)
