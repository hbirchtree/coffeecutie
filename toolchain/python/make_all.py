#!/usr/bin/env python3

from os import makedirs

import python.make_config_parser.variable_templates as var_templates
import python.make_config_parser.name_generator as ng
import python.make_config_parser.from_yaml as yml
from python.make_components.command import *
from python.make_components.comment import *
from python.make_components.conditional_block import *
from python.make_components.var_decl import *

#
# YAML is silly, it converts ON/OFF to True/False
# We use ON/OFF as symbols for CMake
#
def convert_to_cmake(variables):
    def convert_value(sub_value):
        if type(sub_value) == bool:
            if sub_value == True:
                return 'ON'
            else:
                return 'OFF'
        return sub_value

    for var in variables:
        value = variables[var]
        if type(value) == dict:
            convert_to_cmake(value)
        elif type(value) == list:
            for i, subval in enumerate(value):
                value[i] = convert_value(subval)
        else:
            variables[var] = convert_value(value)
    return variables

def create_global_variables(precompiled_deps, base_config):
    #
    # Create variable sets, for switching between bare-metal and Dockerized builds
    #
    global_vars = convert_to_cmake(yml.read_yaml('build-globals.yml'))
    blocks = []

    for var in sorted(list(global_vars['globals'].keys())):
        var_desc = VarDecl()
        var_desc.name = var
        var_desc.var = global_vars['globals'][var]
        var_desc.var = var_templates.resolve_variable(base_config, var_desc.var)[0]
        var_desc.flag = VARFLAGS.DEFAULTS

        blocks.append(var_desc)

    for block_desc_name in sorted(list(global_vars['global-sets'].keys())):
        block_desc = global_vars['global-sets'][block_desc_name]

        var_block = StatementBlock()
        var_block.conditional = 'eq ($(env:BUILD_MODE), %s)' % block_desc_name

        for var in sorted(list(block_desc.keys())):
            var_desc = VarDecl()
            var_desc.name = var
            var_desc.var  = block_desc[var]
            var_desc.var = var_templates.resolve_variable(base_config, block_desc[var])[0]

            var_block.statements.append(var_desc)

        blocks.append(var_block)

    return blocks

def create_target_definitions(precompiled_deps, base_config, targets, force_target, config_name='coffee'):
    # Load toolchain configuration, includes project configuration
    toolchain_data = convert_to_cmake(yml.read_yaml('toolchain-config.yml'))
    coffee_data = convert_to_cmake(yml.read_yaml('%s-config.yml' % (config_name,)))

    make_targets = []

    for target in targets:
        # Create base environment
        vars = yml.create_variable_template(toolchain_data, yml.deepcopy(base_config), target)
        vars = yml.create_variable_template(coffee_data, vars, target)
        vars['target-name'] = [target]

        # Resolve all variables before creating Makefile entry
        var_templates.resolve_variables(vars)

        make_targets += [
            Comment(var_templates.resolve_variable(vars, '$(description)')[0]),
            Comment(var_templates.resolve_variable(vars, 'Compiler: $(compiler)')[0])
        ]

        # Command to run the Makefile with CMake
        cmd = Command()
        cmd.command = var_templates.resolve_variable(
            vars, "make -f $(env:ROOT_DIR)/$(runner-choice) $(target)")[0]

        cmd.command += "\n-e EXTRA_OPTIONS=\""
        for opt in sorted(list(vars['cmake-opts'])):
            cmd.command += "\n" + opt
        cmd.command += "\""

        # Only include Docker options if an image is chosen
        if 'container' in vars:
            cmd.command += "\n-e DOCKER_EXTRA_OPTIONS=\""
            for opt in vars['container-opts']:
                cmd.command += "\n" + opt.replace('"', '\\"')
            cmd.command += "\""

            # Container might be a separate image, or a tag on the hbirch/coffeecutie:... image
            if sum([':' in cnt for cnt in vars['container']]) > 0:
                cmd.command += "\n-e DOCKER_CONTAINER=$(container)"
            else:
                cmd.command += "\n-e DOCKER_CONFIG=$(container)"

        cmd.command += "\n-e CMAKE_BUILD_DIR=$(cmake.build)"
        cmd.command += "\n-e OUTPUT_DIR=$(env:BUILD_DIR)"
        cmd.command += "\n-e BUILD_NAME=$(target-name)"

        if 'environment-vars' in vars:
            for opt in vars['environment-vars']:
                cmd.command += "\n" + opt.replace('"', '\\"')

        cmd.command = var_templates.resolve_variable(vars, cmd.command)[0].replace(';', '\\;')

        # Multiple targets means it runs in a two-phase process
        for i, cmake_target in enumerate(vars['cmake-target']):
            if i == 0:
                if len(vars['cmake-target']) == 1:
                    cmd.command += "\n-e CMAKE_TARGET=%s" % cmake_target
            elif i == 1:
                cmd.command += "\n-e CMAKE_SECOND_TARGET=%s" % cmake_target
            else:
                raise RuntimeError("don't know what to do with target: " + cmake_target)

        # Generate Makefile entry
        compile_target = Target()
        compile_target.target_name = target
        compile_target.dependencies = [force_target]
        compile_target.commands.append(cmd)
        compile_target.source = vars

        make_targets.append(compile_target)

    return make_targets

def create_cmake_preload(source):
    def dissolve_env(var):
        import re
        res = re.findall(r'^.*\$\(env:(.+)\).*$', var)
        if len(res) == 0:
            return var.replace('_INTERNAL', '').replace("'", '')
        return dissolve_env(var.replace('$(env:%s)' % res[0], '$ENV{%s}' % res[0]))

    target_name = source['target-name'][0]

    ci_source = convert_to_cmake(yml.read_yaml('ci-config.yml'))
    ci_vars = yml.create_variable_template(ci_source, source, target_name)

    var_templates.resolve_variables(ci_vars)
    variables = ci_vars['cmake-opts']

    vars = [ var[2:] for var in variables if var.startswith('-D') ]
    vars = [ dissolve_env(var) for var in vars ]

    with open('../../.github/cmake/%s.preload.cmake' % target_name, 'w') as out:
        for var in vars:
            name, value = var.split('=')
            out.write('set ( %s "%s" CACHE STRING "" )\n' % (name, value))

if __name__ == '__main__':
    # Load pre-compiled dependencies and variables
    base_config = {}
    base_config = yml.create_plain_variables(convert_to_cmake(yml.read_yaml('build-variables.yml')), base_config)
    var_templates.resolve_variables(base_config, loose_resolve=True)

    blocks = create_global_variables({}, base_config)

    force_target = Target()
    force_target.target_name = 'FORCE'
    make_targets = [force_target]

    linux_targets = yml.create_target_listing('build-targets-linux.yml')
    linux_make_targets = yml.deepcopy(make_targets)
    linux_make_targets += create_target_definitions({}, base_config, linux_targets, force_target)

    mac_targets = yml.create_target_listing('build-targets-osx.yml')
    mac_make_targets = yml.deepcopy(make_targets)
    mac_make_targets += create_target_definitions({}, base_config, mac_targets, force_target)

    windows_targets = yml.create_target_listing('build-targets-windows.yml')
    windows_make_targets = yml.deepcopy(make_targets)
    windows_make_targets += create_target_definitions({}, base_config, windows_targets, force_target)

    makedirs('../../.github/cmake/select', exist_ok=True)

    for target in linux_make_targets + mac_make_targets + windows_make_targets:
        if isinstance(target, Target) and target.source is not None:
            create_cmake_preload(target.source)

            if 'container' not in target.source:
                continue

            if ':' not in target.source['container'][0]:
                target.source['container'] = 'hbirch/coffeecutie:' + target.source['container'][0]
            else:
                target.source['container'] = target.source['container'][0]

            with open('../../.github/cmake/select/%s.sh' % target.target_name, 'w') as selector:
                selector.write('echo "::set-env name=CONTAINER::%s"\n' % target.source['container'])
                selector.write('CONTAINER=%s' % target.source['container'])

    with open('Makefile.linux', 'w') as mak:
        for block in blocks:
            mak.write(var_templates.finalize_var(block.serialize(indent=0)) + "\n")

        for target in linux_make_targets:
            mak.write(var_templates.finalize_var(target.serialize()).replace('$(shell)', '.shell') + "\n")

    with open('Makefile.mac', 'w') as mak:
        for block in blocks:
            mak.write(var_templates.finalize_var(block.serialize(indent=0)).replace('$(shell)', '.shell') + "\n")

        for target in mac_make_targets:
            mak.write(var_templates.finalize_var(target.serialize()) + "\n")
