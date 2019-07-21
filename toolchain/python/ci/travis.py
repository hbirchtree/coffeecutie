from python.ci.common import flatten_map, get_deploy_info, get_dep_list, create_target_matrix, get_script_locations
from python.common import try_get_key

def travis_gen_config(build_info, repo_dir):
    script_loc = try_get_key(build_info, 'script_location', 'ci')
    make_loc = try_get_key(build_info, 'makefile_location', 'ci')

    def create_travis_matrix(current, build_info):
        env = create_env_matrix(current, build_info)

        for i, var in enumerate(env):
            env[i] = 'BUILDVARIANT=%s' % var

        return env

    def create_build_matrix(targets, build_info):
        excludes = []
        matrices = []
        for target in targets:
            matrix = create_target_matrix(repo_dir + '/' + make_loc, target, build_info)
            for i, e in enumerate(matrix):
                matrix[i] = "BUILDVARIANT=%s" % e
            matrices += matrix
            for target2 in targets:
                if target2 != target:
                    for dist in matrix:
                        excludes.append({'os': target2, 'env': dist})
        return matrices, excludes

    build_matrix, excludes = create_build_matrix(['linux', 'osx'], build_info)

    if try_get_key(build_info, 'coverage', False):
        build_matrix.append('BUILDVARIANT=coverage')
        excludes.append({'os': 'osx', 'env': 'BUILDVARIANT=coverage'})

    deploy_data = get_deploy_info(build_info)

    dependencies = get_dep_list(build_info).replace(";", "%")

    script_locs = get_script_locations(build_info, 'unix')
    return {
        'language': 'cpp',
        'dist': 'trusty',
        'sudo': 'required',
        'services': ['docker'],
        'notifications': {
            'email': True
        },
        'os': ['linux', 'osx'],
        'compiles': ['clang'],
        'env':
            {
                'global': ['MAKEFILE_DIR=%s' % make_loc,
                           'DEPENDENCIES=%s' % dependencies,
                           'CONFIGURATION=Release'],
                'matrix': build_matrix
            },
        'matrix':
            {
                'exclude': excludes,
                'allow_failures': try_get_key(build_info, 'allow_fail', [])
            },
        'branches': {
            'only': deploy_data.build_branches
        },
        'apt': {'update': True},
        'addons': 
            {
                'homebrew': {'update': True, 'packages': ['sdl2', 'cmake', 'python3', 'python', 'jq']}
            },
        'before_script': script_locs.deps,
        'script': script_locs.build,
        'after_success': script_locs.deploy
    }
