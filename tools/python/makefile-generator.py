#!/usr/bin/python3

from sys import argv
import re

from collections import defaultdict

from yaml import load, dump
try:
  from yaml import CLoader as Loader, CDumper as Dumper
except ImportError:
  from yaml import Loader, Dumper

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
def gen_targ_data(templates, targets, definitions):
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
    if len(selection) == 0:
      print(k, 'did not match anything')
    gen_targ_data(templates[k], selection, definitions)


# For dictionaries, collapses child dictionaries to the form
#  a[key1][key2] => key1.key2
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


def replace_definitions(vars, definitions):
  var_find = re.compile('\$\((.*?)\)')
  for v in vars:
    var_string = str(vars[v])
    matches = re.findall(var_find, var_string)
    if len(matches) > 0:
      for m in matches:
        if m.startswith('env:'):
          continue
        var_string = var_string.replace('$(%s)' % m, definitions[m][0])
    var_string = var_string.replace('$(env:', '$(')
    vars[v] = var_string


# Generate Makefile rule string from target defintion
def generate_rule(defs):
  try:
    defs['dependencies'] = stringify(defs['dependencies'])
  except KeyError:
    defs['dependencies'] = ''

  mapping = {
    'container-opts': 'DOCKER_EXTRA_OPTIONS',
    'cmake-opts': 'EXTRA_OPTIONS',
    'toolchain': 'TOOLCHAIN',
    'target-name': 'BUILD_NAME',
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
\t@make -f $(ROOT_DIR)/$(RUNNER) %s \
"""
%
(defs['description'][0],
 defs['compiler'][0],
 defs['target-name'][0],
 defs['dependencies'],
 defs['target'][0])
  )

  for v in sorted(mapping.keys()):
    try:
      var += "\\\n\t\t-e %s=\"%s\" " % (mapping[v], stringify(defs[v]))
    except KeyError:
      pass

  extra_vars = {
    'CMAKE_BUILD_DIR': '$(CMAKE_BUILD_DIR)',
    'OUTPUT_DIR': '$(BUILD_DIR)'
  }

  for v in sorted(extra_vars.keys()):
    var += "\\\n\t\t-e %s=\"%s\"" % (v, extra_vars[v])

  var = var\
    .replace(';', '\;')\
    .replace('False', 'OFF')\
    .replace('True', 'ON')\
    .replace('$(env:', '$(')

  return var


# Generate dependency rule for getting a Git dependency
def generate_dep_rule(name, defs, var_definitions):
  replace_definitions(defs, var_definitions)

  if defs['type'] == 'git':
    var = (
"""
%s.mkdir:
\tmkdir -p "%s"
""" % (name, defs["root"]) )

    var += (
"""
%s: %s.mkdir
\t[ ! -d %s/.git ] && \\
\tgit clone '%s' %s || \\
\ttrue
""" % (name, name, defs['root'], defs['source'], defs['root'])
    )
    return var
  else:
    return ""


# Generate conditional variables for Makefile
def generate_conditional_variables(name, vars, definitions):
  var = "\nifeq ($(BUILD_MODE), %s)" % name

  replace_definitions(vars, definitions)

  for v in sorted(vars.keys()):
    var += "\n\t%s\t\t\t:= %s" % (v, vars[v])

  var += '\nendif\n'

  return var


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

  # Initialize target structures
  for t in targets:
    target_definitions[t] = {}
    root = target_definitions[t]
    root.update(variables)
    root.update(collapse_dictionary({'dependencies': dependencies}))
    root.update({'target-name': [t]})

  # Generate variables for targets defined earlier
  gen_targ_data(data['templates'], targets, target_definitions)

  # Resolve variables to their basic form
  resolve_targets(target_definitions)

  # Get one target as a sample, used to resolve dependency targets
  yoink_definitions = target_definitions[list(target_definitions.keys())[0]]

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
      target_list = target_list + "\\n" + t

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