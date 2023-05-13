#!/usr/bin/env python3

from enum import unique
import re
from collections import defaultdict
from os import makedirs, mkdir

from parse import all_enums
from parse import commands_for
from parse import enums_for
from parse import extensions_supported_by
from parse import features_of
from parse import parse_registry

from codegen import enum_create_name, generate_enum, generate_function, generate_header, snakeify_underscores


def clear_file(filename: str):
    with open(filename, 'w') as f:
        f.truncate(0)


def append_file(filename: str, code: str = ''):
    with open(filename, 'a') as f:
        f.write(code + '\n')


def generate_command_docstring(func_name, params, return_type: str, brief: str):
    param_list = '\n'.join([ f' * \\param {name} {type}' for name, type, ex in params ])
    return f'''/*!
 * \\brief {brief}
''' + param_list + f'''
 * \\return {return_type[1] or return_type[0]}
 */'''

def generate_version_commands(
        registry,
        api: str,
        versions: dict,
        command_file: str,
        code_file: str,
        compatibility_symbols: dict,
        function_defs: dict,
        usages: dict):
    
    makedirs('versions', exist_ok=True)

    api_ns = 'es' if api == 'gles2' else 'core'

    append_file(command_file, f'''
template<typename Current>
struct {api_ns}
{{
    using version = Current;''')
    append_file(code_file, f'namespace {api_ns} {{')

    is_first = True
    for version, feature in features_of(registry, [api]):
        version_suffix = 'es' if version[0] == 'es' else ''
        major, min = version[1]
        # namespace_name = f'v{version[1][0]}{version[1][1]}{version_suffix}'
        versions[version_suffix].append(
            f'{major}{min}{version_suffix}')
        guard = 'GL_ES' if version_suffix == 'es' else 'GL'
        guard = f'{guard}_VERSION_{major}_{min}'

        ver_file = f'versions/ver_{major}{min}_{api_ns}.h'

        clear_file(ver_file)
        append_file(ver_file, f'#ifdef {guard}')
        for func_name, return_type, params, _ in commands_for(feature.findall('require'), registry):
            if func_name in compatibility_symbols[version[0]]:
                continue
            if func_name in function_defs[version[0]]:
                continue
            function_defs[version[0]].append(func_name)
            for chunk in generate_function((func_name[2:], return_type, params), usages, version):
                if 'STATICINLINE' in chunk:
                    # Insert the comment right after requires clause
                    append_file(ver_file, generate_command_docstring(
                        func_name,
                        params,
                        return_type,
                        f'Wraps around {func_name}. Introduced in GL {version[0]} {major}.{min}'))
                append_file(ver_file, chunk)
        append_file(ver_file, f'#endif // {guard}')

        append_file(command_file, f'#include "{ver_file}"')
        if is_first:
            append_file(code_file, f'''constexpr bool enabled =
#ifdef {guard}
    true
#else
    false
#endif
    ;
''')
        append_file(code_file, f'using v{major}{min} = ::gl::impl::{api_ns}<Version<{major}, {min}>>;')

        is_first = False

    for i, feature in enumerate(reversed([ x for x in features_of(registry, [api]) ])):
        version, feature = feature
        guard = 'GL_ES' if version[0] == 'es' else 'GL'
        guard = f'{guard}_VERSION_{version[1][0]}_{version[1][1]}'
        if i == 0:
            append_file(code_file, f'#ifdef {guard}')
        else:
            append_file(code_file, f'#elif defined({guard})')
        append_file(code_file, f'using highest = Version<{version[1][0]}, {version[1][1]}>;')
    append_file(code_file, '#endif')

    for i, feature in enumerate(features_of(registry, [api])):
        version, feature = feature
        append_file(code_file, f'using lowest = Version<{version[1][0]}, {version[1][1]}>;')
        break
    
    append_file(command_file, f'''
}}; // struct {api_ns}''')
    append_file(code_file, f'}} // {api_ns}')


def main():
    registry = parse_registry()
    usages = {}

    versions = defaultdict(list)

    code_file = 'glw.h'
    command_file = 'commands.h'

    clear_file(code_file)
    clear_file(command_file)

    append_file(command_file, generate_header([], []))
    append_file(command_file, '''
namespace gl::impl {
''')

    append_file(code_file, '''#pragma once

#include "groups.h"
#include "commands.h"

namespace gl {''')

    function_defs = defaultdict(list)
    compatibility_symbols = defaultdict(set)

    for version, feature in features_of(registry, ['gl', 'gles2']):
        for section in feature.findall('remove'):
            if section.get('profile') != 'core':
                continue
            for symbol in section:
                compatibility_symbols[version[0]].add(symbol.get('name'))

    #
    # Generate versioned headers
    #
    for api in ['gl', 'gles2']:
        generate_version_commands(
            registry, api, versions, command_file, code_file,
            compatibility_symbols, function_defs, usages)

    append_file(command_file, '''
} // gl::impl''')
        
    append_file(code_file, '} // gl')

    makedirs('extensions', exist_ok=True)

    #
    # Generate extension headers
    #
    for extension, ext_name in extensions_supported_by(['gl', 'gles2'], registry):
            current_file_stem = f'{ext_name[3:]}.h'
            current_file = f'extensions/{current_file_stem}'
            clear_file(current_file)
            ns_components = [ comp.lower() for comp in ext_name.split('_')[1:] ]
            ns = ns_components[0]
            ns_components = ns_components[1:]
            ns_name = '_'.join(ns_components)

            append_file(current_file, f'''#pragma once

#ifdef {ext_name}''')

            enum_classes = set()
            [ enum_classes.add(meta[0]) for name, value, meta in enums_for(extension.findall('require'), registry) if meta[0] is not None and name != 'GL_NONE' ]
            enum_classes = list(enum_classes)
            enum_classes.sort()
            for meta in enum_classes:
                usages[meta] = 1
                append_file(current_file, f'#include "../enums/{meta}.h"')

            append_file(current_file, f'namespace gl::{ns}::{ns_name} {{')

            for meta in enum_classes:
                meta = enum_create_name(meta)
                append_file(current_file, f'using gl::group::{meta};')

            append_file(current_file, 'namespace values {')
            for name, value, meta in enums_for(extension.findall('require'), registry):
                name = snakeify_underscores(name)
                if re.match(r'^[0-9]', name):
                    name = f'n{name}'
                if name.endswith(ns):
                    name = name[:-len(f'_{ns}')]
                if meta[0] is not None:
                    continue
                elif meta[1] != 'all':
                    guard = 'GL_ES_VERSION_2_0' if meta[1] == 'gles2' else 'GL_VERSION_1_0'
                    append_file(current_file, f'#if defined({guard})')
                append_file(current_file, f'constexpr u32 {name} = {value};')
                if meta[1] != 'all':
                    append_file(current_file, '#endif')
            append_file(current_file, '} // values')

            for func_name, rtype, params, api in commands_for(extension.findall('require'), registry):
                visible_name = func_name[2:]
                if func_name.endswith(ns.upper()):
                    visible_name = func_name[2:-len(ns)]
                if api is not None:
                    guard = 'GL_ES_VERSION_2_0' if api == 'gles2' else 'GL_VERSION_1_0'
                    append_file(current_file, f'#if defined({guard})')
                for chunk in generate_function((func_name[2:], rtype, params), usages, override_name=visible_name):
                    if 'STATICINLINE' in chunk:
                        # Insert the comment right after requires clause
                        append_file(current_file, generate_command_docstring(
                            func_name,
                            params,
                            rtype,
                            f'Part of {ext_name}'))
                    append_file(current_file, chunk)
                if api is not None:
                    append_file(current_file, '#endif')
            append_file(current_file, f'}} // gl::{ns}::{ns_name}')
            append_file(current_file, f'#endif // {ext_name}')

            append_file(current_file, f'''namespace gl::{ns}::{ns_name} {{
constexpr auto name = "{ext_name}";
}}''')

    common_file = 'enums/common.h'
    group_file = 'groups.h'

    makedirs('enums', exist_ok=True)

    clear_file(group_file)
    clear_file(common_file)

    append_file(common_file, '''#pragma once

#include "../base.h"
#include <peripherals/enum/helpers.h>
#include <peripherals/libc/types.h>''')

    append_file(group_file, '''#pragma once
''')

    for enum in all_enums(registry):
        name, _, _ = enum
        enum_file = f'enums/{name}.h'
        clear_file(enum_file)
        append_file(enum_file, '''#pragma once

#include "common.h"

namespace gl::group {''')
        for line in generate_enum(enum, usages, compatibility_symbols):
            append_file(enum_file, line)
        
        append_file(enum_file, '\n} // namespace gl::group')
        append_file(group_file, f'#include "{enum_file}"')

if __name__ == '__main__':
    main()
