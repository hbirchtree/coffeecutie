import python.make_config_parser.variable_templates as var_templates
import python.make_config_parser.name_generator as ng
from python.make_config_parser.name_generator import conditional_append
from python.make_config_parser.yaml_config import load, Loader

from copy import deepcopy

def read_yaml(source_file):
    with open(source_file) as f:
        return load(f.read(), Loader=Loader)

def create_target_listing(source_file):
    data = read_yaml(source_file)

    return list(ng.generate_target_names(data))

def create_plain_variables(variables, base_variables, prefix = ''):
    out_variables = base_variables

    def get_vars(root, prefix):
        if type(root) == dict:
            for k in root:
                get_vars(root[k], conditional_append(prefix, k))
        else:
            out_variables[prefix] = [str(root)]

    get_vars(variables, prefix)

    return out_variables


def create_variable_template(source_data, base, target):
    variables = base

    def conditional_get(values, key):
        if key in values:
            out = values[key]
            if type(out) != list:
                return [out]
            return out
        return []

    def insert_vars(root):
        # Two-pass, get variables first
        for var in root:
            if not var_templates.key_is_pattern(var):
                var_templates.add_variable(variables, var, conditional_get(root, var))

        for var in root:
            if var_templates.key_is_pattern(var):
                if var_templates.target_matches(var, target):
                    insert_vars(root[var])

    insert_vars(source_data)

    return variables

if __name__ == '__main__':
    targets = create_target_listing('build-targets-linux.yml')

    dependencies = create_plain_variables(read_yaml('build-dependencies.yml')['binary-dependencies'], {})
    new_dependencies = {}
    for k in dependencies:
        new_dependencies[conditional_append('dependencies', k)] = dependencies[k]
    dependencies = new_dependencies

    base_variables = create_plain_variables(read_yaml('build-variables.yml'), deepcopy(dependencies))
    var_templates.resolve_variables(base_variables, loose_resolve=True)

    base_variables['target-name'] = ['coffee']

    toolchain_data = read_yaml('toolchain-config.yml')

    for target in [ conditional_append('coffee', target) for target in targets]:
        print(target)
        vars = create_variable_template(toolchain_data, deepcopy(base_variables), target)
        var_templates.resolve_variables(vars)
        var_templates.finalize_environment(vars)

        print(vars['cmake-opts'])
