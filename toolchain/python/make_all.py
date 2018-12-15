#!/usr/bin/env python3

import python.make_config_parser.variable_templates as var_templates
import python.make_config_parser.name_generator as ng
import python.make_config_parser.from_yaml as yml
from python.make_components.command import *
from python.make_components.comment import *
from python.make_components.conditional_block import *
from python.make_components.var_decl import *

#
# Check that all dependencies are present by probing the environment
#
def verify_dependencies(vars, dependency_list):
    for dep in dependency_list:
        if 'dependencies.%s.target' % dep not in vars:
            raise RuntimeError('missing dependency: ' + dep)

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

def create_dependencies(precompiled_deps, base_config):
    #
    # Create dependency targets, for acquiring them through Git
    #
    make_targets = []

    for dep in sorted(list(precompiled_deps.keys())):
        dep_info = precompiled_deps[dep]
        dep_type = dep_info['type']

        if dep_type not in ['git']:
            continue

        vars = yml.create_plain_variables({
            'dep.target': dep,
            'dep.source': '$(env:BUILD_DIR)/dependencies/$(dep.target)',
            'dep.source_root': dep_info['root'],
            'dep.url': dep_info['source'],
            'dep.type': dep_info['type'],
            'dependencies': {
                dep: dep_info
            }
        }, yml.deepcopy(base_config))

        var_templates.resolve_variables(vars)

        mkdir_cmd = Command()
        mkdir_cmd.command = 'mkdir -p $(dep.source_root)'

        clone_cmd = Command()
        clone_cmd.flags = COMMAND_FLAGS.IGNORED
        clone_cmd.command = '$(dep.type) clone --recursive --depth 1 $(dep.url) $(dep.source_root)'

        update_cmd = Command()
        update_cmd.flags = COMMAND_FLAGS.IGNORED
        update_cmd.command = '$(dep.type) -C $(dep.source_root) pull'

        mkdir_cmd.command = var_templates.resolve_variable(vars, mkdir_cmd.command)[0]
        clone_cmd.command = var_templates.resolve_variable(vars, clone_cmd.command)[0]
        update_cmd.command = var_templates.resolve_variable(vars, update_cmd.command)[0]

        dep_target = Target()
        dep_target.target_name = dep
        dep_target.commands = [mkdir_cmd, clone_cmd, update_cmd]

        make_targets.append(dep_target)

    return make_targets

def create_global_variables(precompiled_deps, base_config):
    #
    # Create variable sets, for switching between bare-metal and Dockerized builds
    #
    global_vars = convert_to_cmake(yml.read_yaml('build-globals.yml'))
    blocks = []

    global_var_variables = yml.create_plain_variables(precompiled_deps, yml.deepcopy(base_config), 'dependencies')

    var_templates.resolve_variables(global_var_variables, loose_resolve=True)

    for var in sorted(list(global_vars['globals'].keys())):
        var_desc = VarDecl()
        var_desc.name = var
        var_desc.var = global_vars['globals'][var]
        var_desc.var = var_templates.resolve_variable(global_var_variables, var_desc.var)[0]
        var_desc.flag = VARFLAGS.DEFAULTS

        blocks.append(var_desc)

    for block_desc_name in sorted(list(global_vars['global-sets'].keys())):
        block_desc = global_vars['global-sets'][block_desc_name]

        var_block = StatementBlock()
        var_block.conditional = 'eq ($(env:BUILD_MODE), %s)' % block_desc_name

        for var in sorted(list(block_desc.keys())):
            var_desc = VarDecl()
            var_desc.name = var
            var_desc.var = var_templates.resolve_variable(global_var_variables, block_desc[var])[0]

            var_block.statements.append(var_desc)

        blocks.append(var_block)

    return blocks

def create_target_definitions(precompiled_deps, base_config, targets, force_target):
    # Load toolchain configuration, includes project configuration
    toolchain_data = convert_to_cmake(yml.read_yaml('toolchain-config.yml'))
    coffee_data = convert_to_cmake(yml.read_yaml('coffee-config.yml'))

    make_targets = []

    for target in targets:
        # Create base environment
        vars = yml.create_variable_template(toolchain_data, yml.deepcopy(base_config), target)
        vars = yml.create_variable_template(coffee_data, vars, target)
        vars['target-name'] = [target]

        # Add specified dependencies to environment
        target_dependencies = vars['dependencies']
        for dep in target_dependencies:
            yml.create_plain_variables(precompiled_deps[dep], vars, 'dependencies.%s' % dep)
        verify_dependencies(vars, target_dependencies)

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
        compile_target.dependencies = [force_target] + [
            Target(name) for name in target_dependencies
            if precompiled_deps[name]['type'] != 'empty'
        ]
        compile_target.commands.append(cmd)

        make_targets.append(compile_target)

    return make_targets

if __name__ == '__main__':
    precompiled_deps = yml.read_yaml('build-dependencies.yml')['binary-dependencies']
    convert_to_cmake(precompiled_deps)

    # Load pre-compiled dependencies and variables
    base_config = {}
    base_config = yml.create_plain_variables(convert_to_cmake(yml.read_yaml('build-variables.yml')), base_config)
    var_templates.resolve_variables(base_config, loose_resolve=True)

    blocks = create_global_variables(precompiled_deps, base_config)

    force_target = Target()
    force_target.target_name = 'FORCE'
    make_targets = [force_target]
    make_targets += create_dependencies(precompiled_deps, base_config)

    linux_targets = yml.create_target_listing('build-targets-linux.yml')
    linux_make_targets = yml.deepcopy(make_targets)
    linux_make_targets += create_target_definitions(precompiled_deps, base_config, linux_targets, force_target)

    mac_targets = yml.create_target_listing('build-targets-osx.yml')
    mac_make_targets = yml.deepcopy(make_targets)
    mac_make_targets += create_target_definitions(precompiled_deps, base_config, mac_targets, force_target)

    with open('Makefile.linux', 'w') as mak:
        for block in blocks:
            mak.write(var_templates.finalize_var(block.serialize(indent=0)) + "\n")

        for target in linux_make_targets:
            mak.write(var_templates.finalize_var(target.serialize()) + "\n")

    with open('Makefile.mac', 'w') as mak:
        for block in blocks:
            mak.write(var_templates.finalize_var(block.serialize(indent=0)) + "\n")

        for target in mac_make_targets:
            mak.write(var_templates.finalize_var(target.serialize()) + "\n")
