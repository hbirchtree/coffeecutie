import re
from collections import defaultdict
from copy import copy

def target_matches(pattern, target):
    matcher = re.compile(pattern)
    return re.match(matcher, target) is not None

def add_variable(output, key, value):
    operator = key[0]
    key = key[1:]

    if operator == '=':
        if type(value) == list:
            output[key] = [str(val) for val in value]
        else:
            output[key] = [str(value)]
    elif operator == '+':
        if key not in output:
            output[key] = []

        if type(value) == list:
            output[key] += value
        else:
            output[key] += [str(value)]
    else:
        raise RuntimeError("invalid operator: " + operator)

def key_is_pattern(var):
    return var[0] != '=' and var[0] != '+'

#
#
# Actual variable substitution
#
#

_resolve_pattern = re.compile(r'[^\\]?\$\((.*?)\)')

def extract_variables_unfiltered(value):
    return list(re.findall(_resolve_pattern, value))

def extract_variables(value):
    return [x for x in extract_variables_unfiltered(value) if not x.startswith('env:')]

#
# Detect cycles in variable
#
def contains_cycles(variable_store, target, loose_resolve):
    dependencies = {}

    def detect_cycle(root):
        def detect_internal(root, key_deps):
            for var in root:
                if var in key_deps:
                    raise RuntimeError("cycle detected: %s <- %s" % (str(key_deps), var))
                key_deps.add(var)
                detect_internal(root[var], copy(key_deps))

        detect_internal(root, set())

    def traverse_impl(sub_target, root):
        if sub_target not in variable_store:
            if loose_resolve:
                return
            else:
                raise RuntimeError("variable dependency not found: " + sub_target + " for " + target)

        vars = []
        for sub_var in variable_store[sub_target]:
            vars += extract_variables(sub_var)

        for var in vars:
            detect_cycle(dependencies)
            root[var] = {}
            traverse_impl(var, root[var])

    traverse_impl(target, dependencies)

def resolve_single_variable(variable_store, value, resolved, loose_resolve = False):
    matching = extract_variables(value)
    sum_matches = len(matching)
    for m in matching:
        contains_cycles(variable_store, m, loose_resolve)

        if m not in variable_store and loose_resolve:
            sum_matches -= 1
            continue
        if m not in resolved:
            continue
        if len(variable_store[m]) != 1:
            raise RuntimeError("ambiguous or missing reference: " + str(matching) + "\n" + str(variable_store[m]))
        value = value.replace('$(%s)' % m, variable_store[m][0])

    return (value, sum_matches)

def resolve_variable(variable_store, var):
    resolved = set()
    for varvar in extract_variables(var):
        resolved.add(varvar)
    return resolve_single_variable(variable_store, var, resolved)

def resolve_variables(variable_store, loose_resolve=False):
    resolved = set()

    while len(variable_store) != len(resolved):
        for var in variable_store:
            if var in resolved:
                continue

            sum_matches = 0

            if type(variable_store[var]) != list:
                raise TypeError("invalid key value: " + var + ": " + variable_store[var])

            for i, value in enumerate(variable_store[var]):
                if type(value) != str:
                    raise TypeError("bad type")

                value, value_sum = resolve_single_variable(variable_store, value,
                                                           resolved=resolved, loose_resolve=loose_resolve)
                sum_matches += value_sum
                variable_store[var][i] = value

            if sum_matches == 0:
                resolved.add(var)

#
#
# Transition protected environment variables to Makefile format
#
#

def finalize_var(value):
    out_value = value
    for env_var in extract_variables_unfiltered(value):
        if env_var[:4] != 'env:':
            continue
        out_value = out_value.replace('$(%s)' % env_var, '$(%s)' % env_var[4:])
    return out_value

def finalize_environment(variable_store):
    # Do prepass to check for unresolved variables
    for var in variable_store:
        for value in variable_store[var]:
            for any_var in extract_variables_unfiltered(value):
                if not any_var.startswith('env:'):
                    raise RuntimeError("unsafe finalizing: non-environment variables not resolved: " + variable_store)

    # and NOW replace the variables, from env:SOMETHING to SOMETHING
    for var in variable_store:
        for i, value in enumerate(variable_store[var]):
            variable_store[var][i] = finalize_var(env_var)

if __name__ == '__main__':
    print(target_matches('.*fedora.*', 'fedora.amd64'))
    print(target_matches('.*fedora.*', 'steam.amd64'))

    output = defaultdict(list)
    add_variable(output, '=abc.test', 'abc')
    add_variable(output, '+abc.test2', 'abc')
    add_variable(output, '+abc.test2', 'abc')
    add_variable(output, '+abc.test2', ['def', 'ghi'])

    print(output)

    print(key_is_pattern('+abc'))
    print(key_is_pattern('=abc'))
    print(key_is_pattern('.*abc'))

    variable_set = {
        'abc.test': ['something $(nothing)'],
        'ack.test': ['abc $(abc.test)']
    }

    resolve_variables(variable_set, loose_resolve=True)
    print(variable_set)

    variable_set['nothing'] = ['s']
    resolve_variables(variable_set)
    print(variable_set)

    # cyclic dependency test
    # variable_set = {
    #     'abc.test': ['something $(nothing)'],
    #     'ack.test': ['abc $(abc.test)'],
    #     'nothing': ['$(ack.test)']
    # }
    # resolve_variables(variable_set)
    # print(variable_set)

    variable_set = {
        'abc': ['$(env:HOME)/something']
    }

    finalize_environment(variable_set)
    print(variable_set)
