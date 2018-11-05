#!/usr/bin/env python3

from collections import defaultdict
from collections import namedtuple
import xml.etree.ElementTree as etree

import re

from commands import extract_commands
from commands import arguments_to_string
from commands import GLExtension
from command_filtering import translate_type
from command_filtering import translate_cmd_name
from command_filtering import translate_arg_set
from versioning import GLVersion
from versioning import accepted_version
from versioning import is_accepted_extension
from versioning import is_accepted_api
from versioning import EXTENSION_BLACKLIST

# Parse the Khronos Registry
registry = etree.parse('gl.xml').getroot()
#

extensions = {} # accepted extensions, str type
commands = {} # accepted commands, GLCommand type

for ext in registry.find('extensions').findall('extension'):
    ext_name = ext.get('name')
    if not is_accepted_extension(ext_name):
        continue

    if ext_name in EXTENSION_BLACKLIST:
        continue

    extension = GLExtension()
    extension.name = ext_name
    extension.api += ext.get('supported').split('|')
    
    extensions[ext_name] = extension

for el in registry.findall('feature'):
    api = el.get('api')
    
    if not is_accepted_api(api):
        continue
    
    version_desc = accepted_version(el.get('name'))
    
    level_commands = []
    
    for feature in el.findall('require'):
        if feature.get('profile') == 'compatibility':
            continue
        level_commands += [x.get('name') for x in feature.findall('command')]
    
    # Find deprecations/removal
    for removal in el.findall('remove') + el.findall('deprecate'):
        for cmd in removal.findall('command'):
            cmd_name = cmd.get('name')
            cmd_obj = commands[cmd_name]
            cmd_obj.max_api[0] = version_desc
    
    extract_commands(registry, commands, level_commands, version_desc)

extension_registry = [
        x for x in registry.find('extensions').findall('extension')
        if x.get('name') in extensions
    ]

for ext in extension_registry:
    requirements = ext.findall('require')
    if requirements is None:
        continue
    ext_cmds = []
    for req_set in requirements:
        ext_cmds += [cmd.get('name') for cmd in req_set.findall('command')]
    if not ext_cmds:
        continue
    
    extract_commands(registry, commands, ext_cmds, GLVersion())

    for cmd in ext_cmds:
        cmd_obj = commands[cmd]
        cmd_obj.extensions += [extensions[ext.get('name')]]

TARGET_API = GLVersion()
TARGET_API.major = 3
TARGET_API.minor = 3

buckets = defaultdict(list)
version_buckets = defaultdict(list)
version_buckets_es = defaultdict(list)

for cmd in sorted(list(commands.keys())):
    cmd_obj = commands[cmd]

    if cmd_obj.max_api[0].islowerthan(TARGET_API):
        continue

    # Ignore all value-based Uniform* functions
    if re.findall(r'^.*?Uniform(Handle)?[0-9]?[uifds^v]+(64)([^v]*)$', cmd_obj.original_name) and \
            'UniformBlock' not in cmd_obj.original_name:
        continue

    # Translate command name and return type, simply
    cmd_obj.ret_type = translate_type(cmd_obj.ret_type)
    cmd_obj.name = translate_cmd_name(cmd_obj.name)
            
    # Translate argument types
    for arg in cmd_obj.args:
        arg.atype = translate_type(arg.atype)
    
    # Now perform a transform on the full argument set
    cmd_obj.args = translate_arg_set(cmd_obj.args, cmd_obj.name, cmd_obj.original_name)
    #print(cmd_obj.param_string('STATICINLINE ',
    #    function_arguments=arguments_to_string(cmd_obj.args[0]), 
    #    command_arguments=arguments_to_string(cmd_obj.args[1], typed=False))
    #    )

    if cmd_obj.is_extension():
        alias = cmd_obj.source_element.find('alias')
        if alias is not None:
            for other_cmd_key in sorted(list(commands)):
                other_cmd = commands[other_cmd_key]
                if other_cmd.original_name != alias.get('name'):
                    continue
                other_cmd.fallback += [(cmd_obj, cmd_obj.extensions[0])]

    if cmd_obj.is_extension():
        buckets[cmd_obj.extensions[0].name] += [cmd_obj]
    else:
        version_buckets[cmd_obj.min_api[0].template_str()] += [cmd_obj]
        version_buckets_es[cmd_obj.min_api[1].template_str()] += [cmd_obj]

#exit(0)

def get_cmd_deref(cmd):
    return cmd.param_string('STATICINLINE ',
            function_arguments=arguments_to_string(cmd.args[0]),
            command_arguments=arguments_to_string(cmd.args[1], typed=False))

def gen_feature_bucket(bucket):
    added_commands = []
    prev_version = None
    
    for b in sorted(list(bucket)):
        if b == 'GLVER_99':
            continue

        version_name = 'CGL_' + b.replace('GLVER_', '').replace('GLESVER_', '')

        print('template<typename ReqVer>')
        print('struct %s' % version_name)

        if prev_version is not None:
            print('    : %s<ReqVer>' % prev_version)
        print('{')

        if prev_version is not None:
            print('using Parent = %s<ReqVer>;' % prev_version)

        used_func = []

        for cmd in added_commands:
            for other_cmd in bucket[b]:
                if cmd == other_cmd.name and cmd not in used_func:
                    used_func += [cmd]
                    print('using Parent::%s;' % cmd)
                    break
                elif cmd in used_func:
                    break

        for cmd in bucket[b]:
            print(get_cmd_deref(cmd))
            added_commands += [cmd.name]

        print('};')
        prev_version = version_name

print('#pragma once')

print('#include "all_levels_include.h"')
print()
print('#if defined(COFFEE_GCC)')
print('#pragma GCC diagnostic push')
print('#pragma GCC diagnostic ignored "-Wpointer-bool-conversion"')
print('#endif')
print()
print('namespace Coffee{')
print('namespace CGL{')
print('#if GL_VERSION_VERIFY(0x100, GL_VERSION_NONE)')
gen_feature_bucket(version_buckets)
print('#elif GL_VERSION_VERIFY(GL_VERSION_NONE, 0x200)')
gen_feature_bucket(version_buckets_es)
print('#endif')


for b in sorted(list(buckets)):
    print('#if defined(%s) && %s' % (b, b))
    print('template<typename ReqVer>')
    print('struct CGL_%s' % b[3:])
    print('{')
    
    for cmd in buckets[b]:
        print(get_cmd_deref(cmd))

    print('};')
    print('#endif // %s' % b)
    print()

print('} // CGL')
print('} // Coffee')
print()
print('#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)')
print('#pragma GCC diagnostic pop')
print('#endif')
