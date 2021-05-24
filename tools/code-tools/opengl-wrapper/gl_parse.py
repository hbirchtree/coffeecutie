#!/usr/bin/env python3

import re
from collections import defaultdict

from parse import WHITELISTED_EXTENSION_NAMESPACES, all_enums
from parse import commands_for
from parse import enums_for
from parse import extensions_supported_by
from parse import features_of
from parse import parse_registry

from codegen import enum_create_name, generate_enum, generate_function, generate_header, map_function_name, snakeify, snakeify_underscores


def clear_file(filename: str):
    with open(filename, 'w') as f:
        f.truncate(0)


def append_file(filename: str, code: str = ''):
    with open(filename, 'a') as f:
        f.write(code + '\n')


def main():
    registry = parse_registry()
    usages = {}

    versions = defaultdict(list)

    code_file = 'glw.h'
    command_file = 'commands.h'
    group_file = 'groups.h'

    clear_file(code_file)
    clear_file(command_file)
    clear_file(group_file)

    append_file(command_file, generate_header([], []))
    append_file(command_file, '''
namespace gl {
''')

    append_file(group_file, '''#pragma once

#include "base.h"
#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>

namespace gl::groups {''')

    append_file(code_file, '''#pragma once

#include "groups.h"
#include "commands.h"

namespace gl {''')

    function_defs = defaultdict(list)
    compatibility_symbols = defaultdict(set)

    for version, feature in features_of(registry):
        for section in feature.findall('remove'):
            if section.get('profile') != 'core':
                continue
            for symbol in section:
                compatibility_symbols[version[0]].add(symbol.get('name'))

    for version, feature in features_of(registry):
        version_suffix = 'es' if version[0] == 'es' else ''
        namespace_name = f'v{version[1][0]}{version[1][1]}{version_suffix}'
        versions[version_suffix].append(
            f'{version[1][0]}{version[1][1]}{version_suffix}')
        guard = 'GL_ES' if version_suffix == 'es' else 'GL'
        guard = f'{guard}_VERSION_{version[1][0]}_{version[1][1]}'
        append_file(command_file, f'#ifdef {guard}')
        append_file(code_file, f'#ifdef {guard}')
        append_file(code_file, f'namespace {namespace_name} {{')

        append_file(command_file, f'namespace {version[0]} {{')

        for func_name, return_type, params in commands_for(feature.findall('require'), registry):
            if func_name in compatibility_symbols[version[0]]:
                continue
            if func_name in function_defs[version[0]]:
                continue
            function_defs[version[0]].append(func_name)
            append_file(command_file, f'/* Introduced in GL {version[0]} {version[1][0]}.{version[1][1]} */')
            for chunk in generate_function((func_name[2:],  return_type, params), usages, version):
                append_file(command_file, chunk)

        append_file(command_file, f'}} // {version[0]}')

        active_functions = set()
        [ active_functions.add(map_function_name(x[2:])) for x in function_defs[version[0]] if x not in compatibility_symbols[version[0]] ]
        active_functions = list(active_functions)
        active_functions.sort()
        for definition in active_functions:
            append_file(code_file, f'using {version[0]}::{definition};')

        append_file(command_file, f'#endif // {guard}')

        append_file(code_file, f'}} // {namespace_name}')
        append_file(code_file, f'#endif // {guard}')
        append_file(code_file)

    append_file(command_file, '''
} // gl''')

    # for extension, ext_name in extensions_supported_by('gl', registry):
    #         ns_name = snakeify_underscores(ext_name)
    #         append_file(code_file, f'#ifdef {ext_name}')
    #         append_file(code_file, f'namespace gl::{ns_name} {{')
    #         for name, value, _ in enums_for(extension.findall('require'), registry):
    #             name = snakeify_underscores(name)
    #             append_file(code_file, f'constexpr u32 {name} = {value};')
    #         for func_name, rtype, params in commands_for(extension.findall('require'), registry):
    #             for chunk in generate_function((func_name[2:], rtype, params), usages):
    #                 append_file(code_file, chunk)
    #         append_file(code_file, f'}} // gl::{ns_name}')
    #         append_file(code_file, f'#endif // {ext_name}')

    for version in versions.keys():
        namespace_name = 'core' if version == '' else version
        append_file(code_file, f'namespace {namespace_name}::all {{')

        for namespace in versions[version]:
            prefix = '_ES' if version == 'es' else ''
            append_file(code_file, f'#ifdef GL{prefix}_VERSION_{namespace[0]}_{namespace[1]}')
            append_file(code_file, f'using namespace ::gl::v{namespace};')
            append_file(code_file, '#endif')

        append_file(code_file, f'}} // {namespace_name}::all')

    append_file(code_file, '} // gl')

    for enum in all_enums(registry):
        for line in generate_enum(enum, usages, compatibility_symbols):
            append_file(group_file, line)
    
    append_file(group_file, '''
} // gl::groups''')

if __name__ == '__main__':
    main()
