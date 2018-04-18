#!/usr/bin/python3

from sys import argv
import re
from copy import deepcopy
from collections import defaultdict

from yaml import load, dump
try:
  from yaml import CLoader as Loader, CDumper as Dumper
except ImportError:
  from yaml import Loader, Dumper

# TODO: What is this again?
def gen_targs(prefix, target_struct):
  el = []
  if type(target_struct) == list:
    for part in  target_struct:
      if len(prefix) > 0:
        el.append((prefix + "." + part).replace('..', ''))
      else:
        el.append(part)
  elif type(target_struct) == dict:
    for k in  target_struct:
      if len(prefix) > 0:
        el += gen_targs(prefix + '.' + k,
                               target_struct[k])
      else:
        el += gen_targs(k,
                               target_struct[k])
  return el


# Something something insert into a dictionary a key
# Takes keys of the format:
# [=+][.*]
# Replacing the value if starting with =, otherwise append
def det_insert(dic, keys, value):
  root = dic
  for key in keys[:-1]:
    try:
      root = dic[key]
    except KeyError:
      dic[key] = {}
      root = dic[key]
  final_key = keys[-1]

  if type(value) != list:
    value = [value]

  if final_key[0] == '+':
    root[keys[-1][1:]] += value.copy()
  else:
    root[keys[-1][1:]] = value.copy()


# For a set of patterns, generates a hierarchy of variables.
# It separates variables that start with [=+], and creates patterns
#  with associated variables recursively
def gen_targ_data(templates, targets, definitions, verbose = True):
  properties = [k for k in templates if k[0] == '+' or k[0] == '=']
  patterns = [k for k in templates if k not in properties]

  properties.sort()
  properties.reverse()
  patterns.sort()

  for k in properties:
    for t in targets:
      det_insert(definitions, [t, k], templates[k])

  for k in patterns:
    rgx = re.compile('^' + k + '$')
    selection = []
    for t in targets:
      if rgx.match(t):
        selection += [t]
    if len(selection) == 0 and verbose:
      print(k, 'did not match anything')
    gen_targ_data(templates[k], selection, definitions, verbose)


# For dictionaries, collapses child dictionaries to the form
#  a[key1][key2] => a[key1.key2]
def collapse_dictionary(d):
  out = {}
  for f in d:
    v = d[f]
    if type(v) == dict:
      r = collapse_dictionary(v)
      for k in r:
        out.update({f + "." + k: r[k]})
    else:
      out.update({f: [v]})
  return out


# Generate dependency information describing pre-built build dependencies
# As an example, native-libs is used for all targets
def gen_deps(dependencies, dep_defs):
  for dep in dependencies:
    src = dependencies[dep]
    dep_defs[dep] = src.copy()


# Inserts  into var_defs the variables found in variables, after collapsing it
def gen_vars(variables, var_defs):
  var_defs.update(collapse_dictionary(variables))


# For all variables in definitions, resolves variables of the form
#  $(VAR)
#  and works until no more variables may be resolved.
# Variables of the form $(env:VAR) are not touched,
#  as they are transformed into the form $(VAR) for use in Makefile.
def resolve_vars(definitions):
  var_pattern = re.compile('\$\((.*?)\)')
  resolved = set()
  while len(definitions.keys()) != len(resolved):
    for var in definitions:
      if var in resolved:
        continue

      sum_matches = 0

      if type(definitions[var]) != list:
        definitions[var] = [definitions[var]]

      for i, value in enumerate(definitions[var]):
        value = str(value)
        matching = re.findall(var_pattern, value)
        matching = [x for x in matching if not x.startswith('env:')]
        sum_matches += len(matching)
        for m in matching:
          sub_value = definitions[m][0]
          value = value.replace('$(%s)' % (m,), str(sub_value))
        definitions[var][i] = value
      if sum_matches == 0:
        resolved.add(var)


# Recursively resolves variable sets for targets, such
#  that each targets has its own derived set of variables.
def resolve_targets(target_definitions):
  for target in target_definitions:
    resolve_vars(target_definitions[target])


# Quick function to print target information
def print_target_data(target_defs):
  for t in sorted(target_defs.keys()):
    print(t)
    for f in sorted(target_defs[t].keys()):
      print('\t', f, target_defs[t][f])


# Stringifies Python structures into Makefile-consumable format
# Dictionaries are not supported
def stringify(var):
  if type(var) == list:
    out = ""
    for e in var:
      out += str(e)
      out += " "
    return out.strip()
  else:
    return str(var)


# On variable set vars, use definitions to resolve the variable names
# Runs until all variables are fully resolved
def replace_definitions(vars, definitions):
  var_find = re.compile('\$\((.*?)\)')
  for v in vars:
    var_string = str(stringify(vars[v]))
    matches = re.findall(var_find, var_string)
    if len(matches) > 0:
      for m in matches:
        if m.startswith('env:'):
          continue
        var_string = var_string.replace('$(%s)' % m, definitions[m][0])
    var_string = var_string.replace('$(env:', '$(')
    vars[v] = var_string


# Generate Makefile rule string from target defintion
# Is based on creating a CMake command string,
#  interpreting defs as an already-resolved set of variables
# Gives a string representation back
# Also transforms environment variables to
#  Make environment variables
def generate_rule(defs):
  try:
    defs['dependencies'] = stringify(defs['dependencies'])
  except KeyError:
    defs['dependencies'] = ''

  mapping = {
    'container-opts': 'DOCKER_EXTRA_OPTIONS',
    # 'toolchain': 'TOOLCHAIN',
    'target-name': 'BUILD_NAME',
    'cmake-opts': 'EXTRA_OPTIONS',
  }

  if len(defs['cmake-target']) == 2:
    defs['cmake-second-target'] = defs['cmake-target'][1].split('@')[0]
    mapping['cmake-second-target'] = 'CMAKE_SECOND_TARGET'
    defs['cmake-target'] = defs['cmake-target'][0]

  try:
    if stringify(defs['container']).startswith(':'):
      mapping['container'] = 'DOCKER_CONTAINER'
      defs['container'] = stringify(defs['container'])[1:]
    else:
      mapping['container'] = 'DOCKER_CONFIG'
  except KeyError:
    pass

  var = (
"""

# %s
# Compiler: %s
%s: %s FORCE
\t@make -f $(ROOT_DIR)/%s %s \
"""
%
(defs['description'][0],
 defs['compiler'][0],
 defs['target-name'][0],
 defs['dependencies'],
 defs['runner-choice'][0],
 defs['target'][0])
  )

  for v in sorted(mapping.keys()):
    try:
      var += "\\\n\t\t-e %s=\"%s\" " % (mapping[v], stringify(defs[v]))
    except KeyError:
      pass

  extra_vars = {
    'CMAKE_BUILD_DIR': stringify(defs['program.workdir']),
  }

  if 'program.exec' in defs:
    # extra_vars.clear()
    extra_vars['CUSTOM_COMMAND'] = '%s %s' % (stringify(defs['program.exec']),
                                              stringify(defs['program.arguments']))

  extra_vars['OUTPUT_DIR'] = '$(BUILD_DIR)'

  for v in sorted(extra_vars.keys()):
    var += "\\\n\t\t-e %s=\"%s\"" % (v, extra_vars[v])

  var = var\
    .replace(';', '\;')\
    .replace('False', 'OFF')\
    .replace('True', 'ON')\
    .replace('$(env:', '$(')

  return var


# Generate dependency rule for getting a Git dependency
# defs contains the unresolved target description
# var_definitions is the environment
def generate_dep_rule(name, defs, var_definitions):
  replace_definitions(defs, var_definitions)

  t = defs['type']

  if defs['type'] == 'git' or defs['type'] == 'hg':
    var = (
"""
%s.mkdir:
\tmkdir -p "%s"
""" % (name, defs["root"]) )

    var += (
"""
%s: %s.mkdir
\t[ ! -d %s/.%s ] && \\
\t%s clone --recursive --depth 1 '%s' %s || \\
\ttrue
""" % (name, name, defs['root'], t, t, defs['source'], defs['root'])
    )
    return var
    return var
  else:
    return ""


# Generate conditional variables for Makefile
# vars and definitions is used to resolve variable names
# var is the string output
def generate_conditional_variables(name, vars, definitions):
  var = "\nifeq ($(BUILD_MODE), %s)" % name

  replace_definitions(vars, definitions)

  for v in sorted(vars.keys()):
    var += "\n\t%s\t\t\t:= %s" % (v, vars[v])

  var += '\nendif\n'

  return var


# A target that, using information about target toolchains,
#  strips all build dependencies of debug information.
# This is really important for Assimp.
# Assimp is THICC at 80MB withot stripping.
def gen_strip_target(target_info):
  target_info = deepcopy(target_info)

  target_info['cmake-opts'] = []
  target_info['dependencies'] = []
  target_info['target-path'] = ['/target-defs']
  target_info['target'] = ['custom']
  target_info['container-opts'].append('-v $(build-dep.root):$(target-path):rw')

  arch_name = target_info['target-name'][0].split('.', 1)[1]
  target_info['target-name'] = ['strip.' + arch_name]

  target_info['program.exec'] = ['sh -c']
  target_info['program.arguments'] = [
    # '\'cd $(container.src) && cmake -LA\''
    '\'\\`cmake -LA $(container.src) | grep CMAKE_STRIP | cut -d= -f2\\` -g $(target-path)/*/install/%s/lib/*.a\'' % arch_name
  ]

  resolve_vars(target_info)

  return generate_rule(target_info)


# Transform build dependency description to something
#  that can be consumed by:
#  - generate_dep_rule (for Git checkout)
#  - generate_rule (for CMake builds,
#               do something else for automake
#                and custom ones)
def transform_build_deps(build_deps, template_def, build_template, skeleton, targets):

  targets = [v.replace('coffee.', '') for v in targets]

  skeleton = deepcopy(skeleton)
  a_skeleton = {'docs': skeleton}
  for t in targets:
    a_skeleton[t] = deepcopy(skeleton)

  gen_targ_data(template_def, ['docs'] + targets, a_skeleton, False)

  target_defs = ""

  def add_build_dep_template(bt, out):
    for e in build_template:
      p = re.compile(e)
      if p.match(bt) is not None:
        gen_targ_data(build_template, [bt], {bt: out}, False)

  for target in sorted(list(build_deps.keys())):
    # Target description, quite simple
    td = build_deps[target]

    out = a_skeleton['docs']

    out.update({
      'dep-target': [target],
      'build-arch': ['docs'],
      'automake.opts': [''],
    })

    add_build_dep_template(td['type'], out)

    try:
      src = td['source']
    except:
      src = ''

    try:
      src_type, src = src.split(':', 1)
    except:
      src_type = ''

    dep_rule = {
      'root': '$(bdep.root)',
      'source': src,
      'type': src_type
    }
    resolve_vars(out)
    target_defs = target_defs + generate_dep_rule('%s.source' % (target), dep_rule, out)

    target_defs = target_defs

  for build_target in targets:
    gen_once = False

    for lib_target in sorted(list(build_deps.keys())):
      td = build_deps[lib_target]

      if 'platforms' in td:
        m = re.compile(td['platforms'])
        if not re.match(m, build_target):
          continue

      out = deepcopy(a_skeleton[build_target])

      out.update({
        'build-arch': [build_target],
        'dep-target': [lib_target]
      })

      add_build_dep_template(td['type'], out)

      if not gen_once and td['type'] == 'cmake':
        target_defs = target_defs + gen_strip_target(out)
        gen_once = True

      out['dependencies'] = ['%s.source' % lib_target,
                             '%s.%s.build_dir' % (lib_target, build_target),
                             '%s.%s.install_dir' % (lib_target, build_target)]

      if td['type'] == 'cmake':
        out['cmake-opts'] += td['cmake-opts']
      elif td['type'] == 'automake':
        out['automake.opts'] = [stringify(td['ac-opts'])]
        out['cmake-opts'] = []
      elif td['type'] == 'make':
        out['cmake-opts'] = []
      elif td['type'] == 'openssl':
        out['cmake-opts'] = []
      else:
        continue

      build_dirs = {
        'build_dir': '$(build-dep.root)/$(dep-target)/build/$(build-arch)',
        'install_dir': '$(build-dep.root)/$(dep-target)/install/$(build-arch)'
      }

      resolve_vars(out)
      replace_definitions(build_dirs, out)

      mkdir_deps = ''

      for f in sorted(list(build_dirs.keys())):
        mkdir_deps = mkdir_deps + '''\n\n
%s.%s.%s:
\tmkdir -p %s
''' % (lib_target, build_target, f, build_dirs[f])


      target_defs = target_defs + mkdir_deps + generate_rule(out).replace('\\\'', '\'')

  return target_defs


if __name__ == '__main__':
  target_defs = argv[1]

  data = None
  with open(target_defs, 'r') as f:
      data = load(f.read())

  dependencies = dict()
  variables = dict()
  target_definitions = dict()
  targets = data['targets']

  # Generate variable listings
  gen_vars(data['variables'], variables)
  gen_deps(data['dependencies'], dependencies)

  # Generate targets from JSON
  targets = sorted(gen_targs('', targets))

  build_deps = data['build-dependencies']

  # Initialize target structures
  for t in targets:
    target_definitions[t] = {}
    root = target_definitions[t]
    root.update(deepcopy(variables))
    root.update(collapse_dictionary({'dependencies': dependencies}))
    root.update({'target-name': [t]})

  # Generate variables for targets defined earlier
  gen_targ_data(data['templates'], targets, target_definitions)

  for t in targets:
    target_definitions[t]['target-name'] = [v.replace('coffee.', '') for v in target_definitions[t]['target-name']]

  # Resolve variables to their basic form
  resolve_targets(target_definitions)

  # Get one target as a sample, used to resolve dependency targets
  yoink_definitions = target_definitions[list(target_definitions.keys())[0]]

  temp_skeleton = {}
  temp_skeleton.update(variables)
  temp_skeleton.update(collapse_dictionary({'dependencies': dependencies}))
  build_deps = transform_build_deps(build_deps,
                                    data['templates'],
                                    data['build-dep-template'],
                                    temp_skeleton, targets)

  with open(argv[2], 'w') as f:
    f.write(
"""
#
# Build parameters
#
# BUILD_MODE:
#   - containerized: run build in Docker
#   - bare: run without Docker, no wrappers
# 
# GENERATE_PROGRAMS:
#   - ON: build programs and unit tests
#   - OFF: build only libraries, no unit tests or programs
#
# CMAKE: specify path to CMake executable if necessary.
#     If containerized, use the path inside the container.
#

"""
    )

    replace_definitions(data['globals'], yoink_definitions)

    for var in sorted(data['globals'].keys()):
      f.write(('%s ?= %s\n' % (var, data['globals'][var]))\
                .replace(';', '\;')\
                .replace('False', 'OFF')\
                .replace('True', 'ON')\
                .replace('$(env:', '$('))

    try:
      for set in sorted(data['global-sets'].keys()):
        f.write(generate_conditional_variables(set, data['global-sets'][set],
                                               yoink_definitions))
    except KeyError:
      pass

    f.write(
"""
# We use this to always run targets
FORCE:
"""
    )

    for t in sorted(dependencies.keys()):
      f.write(generate_dep_rule(t, dependencies[t], yoink_definitions))

    target_list = ""
    for t in sorted(list(target_definitions.keys())):
      target_list = target_list + "\\n" + t.replace('coffee.', '')

    for lib in sorted(list(data['build-dependencies'].keys())) + ['strip']:
      for target in [x for x in sorted(list(target_definitions.keys())) if 'docs' not in x]:
        if lib in data['build-dependencies'] and 'platforms' in data['build-dependencies'][lib]:
          m = re.compile(data['build-dependencies'][lib]['platforms'])
          if not re.match(m, target):
            continue
        target_list = target_list + "\\n" + lib + '.' + target.replace('coffee.', '')

    f.write(
"""

# Create a help option
list-targets:
	@echo "%s"

help: list-targets

list: list-targets
""" % target_list[2:])

    for t in sorted(target_definitions.keys()):
      f.write(generate_rule(target_definitions[t]))

    f.write('\n\n')

    f.write(build_deps)

    f.write('\n\n')
