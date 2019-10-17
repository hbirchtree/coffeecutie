from os.path import dirname
from yaml import load, dump
from python.common import try_get_key
from collections import namedtuple


DeployInfo = namedtuple('DeployInfo', 'deploy_branches, build_branches')
TargetContainer = namedtuple('TargetContainer', 'supports, real_targets')
ScriptLocations = namedtuple('ScriptLocations', 'deps, build, deploy')

MAKEFILE_DIR = None


def assertm(condition, message='BAD ASSERT'):
    if not condition:
        raise AssertionError(message)


def parse_yaml(source_file):
    with open(source_file, mode='r') as f:
        data = load(f.read())
        return data


def render_yaml(source):
    yaml_struct = dump(source, default_flow_style=False)
    assert ( type(yaml_struct) == str)
    return yaml_struct


def get_deploy_info(build_info):
    deploy_data = DeployInfo([], [])

    for e in try_get_key(build_info, 'branches', []):
        name = 'master'
        if 'name' in e:
            name = e['name']
        if 'build' in e:
            if e['build']:
                deploy_data.build_branches.append(name)
        if 'deploy' in e:
            if e['deploy']:
                deploy_data.deploy_branches.append(name)
        else:
            pass

    if len(deploy_data.deploy_branches) == 0:
        deploy_data.deploy_branches.append('master')

    return deploy_data


def get_dep_list(build_info):
    dependencies = ''
    deps = try_get_key(build_info, 'dependencies', [])
    for dep in deps:
        dependencies = '%s;%s' % (dep, dependencies)
    return dependencies


def flatten_map(root, include_intermediary=False):
    root_type = type(root)

    if root_type == dict:
        for k in root:
            if include_intermediary:
                yield k
            val = root[k]
            for sub in flatten_map(val, include_intermediary=include_intermediary):
                if sub == '.' and not include_intermediary:
                    yield k
                elif sub != '.':
                    yield k + "." + sub
    elif root_type == list:
        for v in root:
            yield v
    else:
        yield root


def load_targets(makers_dir, target_base):
    file_base = '%s/build-targets-%s.yml' % (makers_dir, target_base)

    return TargetContainer(
            [x for x in sorted(list(flatten_map(parse_yaml(file_base), include_intermediary=True))) if not x.endswith('$(shell)')],
            [x for x in sorted(list(flatten_map(parse_yaml(file_base), include_intermediary=False))) if not x.endswith('$(shell)')])


def intersect_targets(source_targets, source_support_targets, selected_targets):
    def pick_source(selection):
        # Check for exact match, and pick that
        exact = [t for t in source_targets if t == selection]

        if exact:
            return exact

        # Otherwise select matching prefix
        return [t for t in source_targets if t.startswith(selection)]

    targets = []
    for source in source_support_targets:
        for selected in selected_targets:
            if source == selected:
                targets += pick_source(selected)

    return targets


def create_target_matrix(makers_dir, target_base, build_info):
    targets = load_targets(makers_dir, target_base)
    platforms = list(flatten_map(try_get_key(build_info, 'platforms', {})))

    return intersect_targets(targets.real_targets, targets.supports, platforms)


def get_script_locations(build_info, platform):
    locs = ScriptLocations([], [], [])

    script_loc = try_get_key(build_info, 'script_location', 'toolchain/ci')

    if platform == 'windows':
        locs.deps.append(script_loc + '/appveyor-deps.ps1')
        locs.build.append(script_loc + '/appveyor-build.ps1')
        locs.deploy.append(script_loc + '/appveyor-deploy.ps1')

        locs.deps[0] = locs.deps[0].replace('/', '\\')
        locs.build[0] = locs.build[0].replace('/', '\\')
        locs.deploy[0] = locs.deploy[0].replace('/', '\\')
    elif platform == 'unix':
        locs.deps.append(script_loc + '/travis-deps.sh')
        locs.build.append(script_loc + '/travis-build.sh')
        locs.deploy.append(script_loc + '/travis-deploy.sh')

    return locs


if __name__ == '__main__':
    [print(v) for v in flatten_map(
            { 'a': {
                'b': ['c', 'd', 'e'],
                'c': ['.', 'f', 'g']
            }
            })]
    print()
    [print(v) for v in flatten_map(
        {'a': {
            'b': ['c', 'd', 'e'],
            'c': ['.', 'f', 'g']
        }
        }, include_intermediary=True)]
    print()
    [print(v) for v in flatten_map('a')]

    linux_target = load_targets('%s/../../makers' % dirname(__file__), 'linux')
    mac_target = load_targets('%s/../../makers' % dirname(__file__), 'mac')
    windows_target = load_targets('%s/../../makers' % dirname(__file__), 'windows')

    print(linux_target)

    target_def = list(flatten_map({
        'ubuntu': '.',
        'osx': '.',
        'ios': '.',
        'win32': '.',
        'uwp': '.'
    }))
    print(intersect_targets(linux_target.real_targets, linux_target.supports, target_def))
    print(intersect_targets(mac_target.real_targets, mac_target.supports, target_def))
    print(intersect_targets(windows_target.real_targets, windows_target.supports, target_def))

