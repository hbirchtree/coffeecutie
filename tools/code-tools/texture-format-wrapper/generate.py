#!/usr/bin/env python3

import json
import re
from os.path import dirname


etc2_128_filter = re.compile(r'_(RGBA8|SRGB8_ALPHA8|RG11)_')

rgba_filter = re.compile(r'(_RGBA(|8)_|_RGBA[0-9]{1,2}(_SNORM|F|I|UI)?$|_RGB[0-9]{1,2}_A[0-9])')
rgb_filter = re.compile(r'(_(|S)RGB(|8)_|_(|S)RGB[0-9]{1,3}(_SNORM|F|I|UI)?$)')
rg_filter = re.compile(r'(_(RED_GREEN|RG11)_|_RG([0-9]{1,2})?(|_SNORM|F|I|UI)$)')
red_filter = re.compile(r'(_(RED|R11)_|_R[0-9]{1,2}(_SNORM|F|I|UI)?$)')

depth_stencil_filter = re.compile('_DEPTH(24|32F)_STENCIL(8)$')
depth_filter = re.compile('_DEPTH_COMPONENT(16|32F)$')
stencil_filter = re.compile('_STENCIL_INDEX(8)$')

rgbx_formats = [
    'RGB9_E5',
]


def parse_types():
    types = []
    root_dir = dirname(__file__)
    with open(f'{root_dir}/formats.json') as atlas:
        types = json.load(atlas)
    with open(f'{root_dir}/addendum.json') as atlas:
        types = types + json.load(atlas)
    return types


def compressed_block_size(format: str):
    if 'DXT1' in format or 'RGTC1' in format:
        return 64
    if 'DXT3' in format or 'DXT5' in format or 'RGTC2' in format:
        return 128
    if 'BPTC' in format:
        return 128
    if 'ETC1' in format:
        return 64
    if 'ETC2' in format or 'EAC' in format:
        if etc2_128_filter.findall(format):
            return 128
        return 64
    if 'PVRTC2' in format:
        return 32 if '2BPP' in format else 128
    if 'PVRTC' in format:
        return 64 if '2BPP' in format else 128
    if 'ASTC' in format:
        return 128
    raise ValueError(f'failed to compute block size for {format}')


def is_srgb(format: str):
    return '_SRGB' in format


def is_floating_point(value_type: str):
    return 'FLOAT' in value_type


def extract_compressed_components(format: str):
    if format[-4:] in ['_EXT']:
        return 4, 'rgba'

    if rgba_filter.findall(format):
        return 4, 'rgba'
    if format[3:] in rgbx_formats:
        return 4, 'rgbx'
    elif rgb_filter.findall(format) or 'R11F_G11F_B10F' in format:
        return 3, 'rgb'
    elif rg_filter.findall(format):
        return 2, 'rg'
    elif red_filter.findall(format):
        return 1, 'red'
    elif depth_stencil_filter.findall(format):
        return 2, 'depth_stencil'
    elif stencil_filter.findall(format):
        return 1, 'stencil'
    elif depth_filter.findall(format):
        return 1, 'depth'
    else:
        raise ValueError(f'Component count not deduced from {format}')


def extract_component_bit_layout(format):
    if format[-4:] in ['_EXT']:
        return '{}'

    # GL_RGBxxx_Ax
    res = re.compile(r'_RGB([0-9])([0-9])([0-9])_A([0-9])$').findall(format)
    if res:
        return '{' + \
               f'.r = {res[0][0]}, ' + \
               f'.g = {res[0][1]}, ' + \
               f'.b = {res[0][2]}, ' + \
               f'.a = {res[0][3]}' + \
               '}'
    # GL_RGBx_Ax
    res = re.compile(r'_RGB([0-9])_A([0-9])$').findall(format)
    if res:
        return '{' + \
               f'.r = {res[0][0]}, ' + \
               f'.g = {res[0][0]}, ' + \
               f'.b = {res[0][0]}, ' + \
               f'.a = {res[0][1]}' + \
               '}'
    # GL_RGBxxx_Ex
    res = re.compile(r'_RGB([0-9])([0-9])([0-9])_E([0-9])$').findall(format)
    if res:
        return '{' + \
               f'.r = {res[0][0]}, ' + \
               f'.g = {res[0][1]}, ' + \
               f'.b = {res[0][2]}, ' + \
               f'.extra = {res[0][3]}' + \
               '}'
    # GL_RGBxxx
    res = re.compile(r'_RGB([0-9])([0-9])([0-9])$').findall(format)
    if res:
        return '{' + \
               f'.r = {res[0][0]}, ' + \
               f'.g = {res[0][1]}, ' + \
               f'.b = {res[0][2]}, ' + \
               '}'
    # GL_RGBA32UI
    # GL_RGB32UI
    # GL_RG8I
    # GL_R32F
    res = re.compile(r'_RGBA([0-9]{1,2})(|I|UI|F|_SNORM)').findall(format)
    if res:
        return '{' + \
               f'.r = {res[0][0]}, ' + \
               f'.g = {res[0][0]}, ' + \
               f'.b = {res[0][0]}, ' + \
               f'.a = {res[0][0]}, ' + \
               '}'
    res = re.compile(r'_(S)?RGB([0-9]{1,2})(|I|UI|F|_SNORM)').findall(format)
    if res:
        return '{' + \
               f'.r = {res[0][1]}, ' + \
               f'.g = {res[0][1]}, ' + \
               f'.b = {res[0][1]}, ' + \
               '}'
    res = re.compile(r'_RG([0-9]{1,2})(|I|UI|F|_SNORM)').findall(format)
    if res:
        return '{' + \
               f'.r = {res[0][0]}, ' + \
               f'.g = {res[0][0]}, ' + \
               '}'
    res = re.compile(r'_R([0-9]{1,2})(|I|UI|F|_SNORM)').findall(format)
    if res:
        return '{' + \
               f'.r = {res[0][0]}, ' + \
               '}'
    # GL_DEPTH_COMPONENTxx
    res = re.compile(r'_DEPTH_COMPONENT([0-9]{1,2})(|F)').findall(format)
    if res:
        return '{' + \
               f'.depth = {res[0][0]}, ' + \
               '}'
    # GL_STENCIL_INDEXx
    res = re.compile(r'_STENCIL_INDEX([0-9])').findall(format)
    if res:
        return '{' + \
               f'.stencil = {res[0][0]}, ' + \
               '}'
    # GL_DEPTHxx_STENCILx
    res = re.compile(r'_DEPTH([0-9]{1,2})(|F)_STENCIL8').findall(format)
    if res:
        return '{' + \
               f'.depth = {res[0][0]}, ' + \
               '}'

    raise ValueError(f'Bit layout not deduced from {format}')


def format_as_enum(format: str):
    if format.startswith('VK_FORMAT_'):
        out = format.lower()[10:]
    else:
        out = format.lower()[3:]
    if out in ['byte', 'short', 'int', 'long', 'float', 'double']:
        out = f'{out}_'
    return out


def extensions_to_list(extensions: list):
    if extensions is None:
        return None
    return f'''"{' '.join(extensions[0])}"sv'''


def version_to_hexadecimal(version: str):
    if version is None:
        return None
    major, minor = version.split('.')
    return f'0x{major}{minor}0'


def generate_struct(texture_type: dict):
    format = texture_type['glInternalFormat']

    if format is None:
        return

    compressed_struct = 'std::nullopt'
    raw_struct = 'std::nullopt'

    if 'COMPRESSED' in format or texture_type['blockWidth'] != 1:
        comp_count, layout = extract_compressed_components(format)
        compressed_struct = f'''COMPRESSED_FORMAT_TYPE{{
        .format          = texture_layout_t::{layout},
        .block_width     = {texture_type['blockWidth']},
        .block_height    = {texture_type['blockHeight']},
        .block_size      = {int(compressed_block_size(format) / 8)},
        .bpp             = {compressed_block_size(format)},
        .component_count = {comp_count},
        .srgb            = {'true' if is_srgb(format) else 'false'},
    }}'''
    else:
        comp_count, _ = extract_compressed_components(format)
        if texture_type['type'] == 'PACKED':
            pixel_size = int(texture_type['typeSize'])
        elif texture_type['type'] == 'RAW':
            pixel_size = int(comp_count * texture_type['typeSize'])
        else:
            raise ValueError(f'undefined type {texture_type["type"]}')
        raw_struct = f'''RAW_FORMAT_TYPE{{
        .bit_layout      = {extract_component_bit_layout(format)},
        .type            = texture_type_t::{format_as_enum(texture_type['glType'])},
        .format          = texture_layout_t::{format_as_enum(texture_type['glFormat'])},
        .pixel_size      = {pixel_size},
        .component_count = {comp_count},
        .component_size  = {texture_type['typeSize']},
        .srgb            = {'true' if is_srgb(format) else 'false'},
        .floating_point  = {'true' if is_floating_point(texture_type['glType']) else 'false'},
    }}'''

    core_version = version_to_hexadecimal(texture_type['glVersion'])
    es_version = version_to_hexadecimal(texture_type['glEsVersion'])
    web_version = version_to_hexadecimal(texture_type['glWebVersion'])

    core_extensions = texture_type['glExtensions']
    es_extensions = texture_type['glEsExtensions']
    web_extensions = texture_type['glWebExtensions']

    if texture_type['vkFormat'].endswith('_EXT'):
        vk_format = 'static_cast<vk_format_t>(0)'
    else:
        vk_format = f'vk_format_t::{format_as_enum(texture_type["vkFormat"])}'

    extra_ifdef = ''
    if 'glFormat' in texture_type and texture_type['glFormat'] is not None:
        extra_ifdef = extra_ifdef + f' && defined({texture_type["glFormat"]})'+\
                                    f' && defined({texture_type["glType"]})'

    return f'''#if defined({format}){extra_ifdef}
{{
    .type = format_t::{format_as_enum(format)},
    .vk_type = {vk_format},
    .compression = {compressed_struct},
    .raw_format = {raw_struct},
    .version = {{
        .core = {core_version or 'std::nullopt'},
        .es = {es_version or 'std::nullopt'},
        .web = {web_version or 'std::nullopt'},
    }},
    .extensions = {{
        .core = {extensions_to_list(core_extensions) or '{}'},
        .es = {extensions_to_list(es_extensions) or '{}'},
        .web = {extensions_to_list(web_extensions) or '{}'},
    }},
}},
#endif'''


def generate_structs(all_types: list):
    for texture_type in all_types:
        struct = generate_struct(texture_type)
        if struct is None:
            continue
        yield struct


if __name__ == '__main__':
    all_types = parse_types()
    with open('texture_formats.inl', 'w+') as formats:
        for struct in generate_structs(all_types):
            formats.write(f'{struct}\n')
    with open('vk_formats.inl', 'w+') as vk_formats:
        for enum in set([x["vkFormat"] for x in all_types]):
            if enum.endswith('_EXT'):
                continue
            vk_formats.write(f'{format_as_enum(enum)} = {enum},\n')

#    print('\n'.join([x for x in generate_structs(all_types)]))
