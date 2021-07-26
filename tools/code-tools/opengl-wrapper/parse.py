#!/usr/bin/env python3

from collections import defaultdict
import re
import xml.etree.ElementTree as etree

from os.path import abspath, dirname


WHITELISTED_APIS       = ['gl', 'gles2']
WHITELISTED_EXTENSION_NAMESPACES = ['AMD', 'ARB', 'ARM', 'EXT', 'IMG', 'INTEL', 'KHR', 'NV', 'NVX', 'OES', 'QCOM']

WHITELISTED_EXTENSIONS = [
    'GL_EXT_texture_compression_dxt1',
    'GL_ANGLE_texture_compression_dxt3',
    'GL_ANGLE_texture_compression_dxt5',

    'GL_EXT_texture_compression_dxt1',
    'GL_EXT_texture_compression_s3tc',
    'GL_EXT_texture_compression_s3tc_srgb',

    'GL_EXT_texture_compression_rgtc',
    'GL_EXT_texture_compression_bptc'
    'GL_OES_texture_compression_astc',
]
BLACKLISTED_EXTENSIONS = [
    'GL_ARB_transpose_matrix',
    'GL_EXT_separate_shader_objects',
    'GL_EXT_texture_env_add',
    'GL_EXT_texture_env_combine',
    'GL_EXT_texture_env_dot3',
]

def parse_registry(file='gl.xml'):
    basedir = dirname(abspath(__file__))
    return etree.parse(f'{basedir}/{file}').getroot()


def text_or_none(val):
    if val is None:
        return None
    return val.text


def segment_command_name(func_name):
#    segments = re.split(r'([0-9A-Z][a-z]+)', func_name)
#    return [ seg for seg in segments if len(seg) > 0 ]
    return func_name


def commands_for(requirements, registry):
    all_commands = registry.find('commands')
    for req in requirements:
        all_version_commands = [ value.get('name') for value in req.findall('command') ]
        api = req.get('api')
        for command in all_commands.findall('command'):
            proto = command.find('proto')
            return_type = [ x for x in ' '.join(proto.itertext()).split(' ') if x != '' ]
            return_type = ' '.join(return_type[:-1])
            return_group = proto.get('group')
            func_name = proto.find('name').text
            params = command.findall('param')

            params_out = [ [
                text_or_none(p.find('name')),
                ' '.join([ v for v in ' '.join(p.itertext()).split(' ')[:-1] if len(v) > 0]),
                [p.get('group'), p.get('len')]]
                for p in params
            ]
            if func_name in all_version_commands:
                func_name = func_name
                func_name = segment_command_name(func_name)

                yield func_name, (return_type, return_group), params_out, api


def all_enums(registry):
    enums = defaultdict(list)
    enum_types = {}
    for enum_group in registry.findall('enums'):
        type = enum_group.get('type')
        for enum in enum_group.findall('enum'):
            name = enum.get('name')
            memberships = enum.get('group')
            if memberships is None:
                continue
            memberships = memberships.split(',')
            for membership in memberships:
                enums[membership].append([name, type])
                if type != None:
                    enum_types[membership] = type
    for v in enums.values():
        v.sort()
    return [ 
        (name, enums[name], [enum_types[name] if name in enum_types else None]) 
        for name in enums.keys() 
    ]


def enums_for(requirements, registry):
    required_values = defaultdict(list)
    for req in requirements:
        enum_values = [ value.get('name') for value in req.findall('enum') ]
        if req.get('api') is None:
            required_values['all'] = required_values['all'] + enum_values
        else:
            required_values[req.get('api')] = required_values[req.get('api')] + enum_values

    for enum_group in registry.findall('enums'):
        for enum in enum_group.findall('enum'):
            name = enum.get('name')
            for api in required_values:
                if name not in required_values[api]:
                    continue
                groups = enum.get('group').split(',') if enum.get('group') is not None else [None]
                for group in groups:
                    yield name, enum.get('value'), [group, api]


def features_of(registry, apis: list):
    version_expression = r'^([0-9]).([0-9])$'
    for feature in registry.findall('feature'):
        api = feature.get('api')

        if api not in apis:
            continue

        api = 'es' if api == 'gles2' else api
        api = 'core' if api == 'gl' else api

        version = re.findall(version_expression, feature.get('number'))
        assert(len(version) == 1)
        version = version[0]
        assert(len(version) == 2)
        version = [ api, version ]
        yield version, feature


def overlaps(a1, a2):
    num_matches = 0
    for v in a1:
        if v in a2:
            num_matches = num_matches + 1
    return num_matches > 0


def extensions_supported_by(apis, registry):
    extensions = registry.find('extensions')
    vendor_expression = r'^GL_([0-9A-Z]+)_'
    for extension in extensions.findall('extension'):
        if not overlaps(apis, extension.get('supported').split('|')):
            continue
        name = extension.get('name')
        vendor = re.findall(vendor_expression, name)
        assert(len(vendor) == 1)
        vendor = vendor[0]
        if vendor not in WHITELISTED_EXTENSION_NAMESPACES or name in BLACKLISTED_EXTENSIONS:
            continue
        yield extension, name
