from python.ci.common import get_deploy_info, get_dep_list, get_deploy_info, create_target_matrix, get_script_locations
from python.common import try_get_key

def appveyor_gen_config(build_info, repo_dir):
    deploy_info = get_deploy_info(build_info)

    dependencies_list = get_dep_list(build_info)

    script_loc = build_info['script_location'].replace('/', '\\')
    make_loc = build_info['makefile_location'].replace('/', '\\')
    make_loc_native = build_info['makefile_location']

    allow_fails = []

    for f in try_get_key(build_info, 'allow_fail', []):
        e = f['env'].split('=')
        allow_fails += [{e[0]: e[1]}]

    deploy_patterns = ''

    secret = try_get_key(try_get_key(build_info, 'secrets', {}), 'appveyor', '')

    matrix = []
    for p in create_target_matrix(repo_dir + '/' + make_loc_native, 'windows', build_info):
        matrix += [{'BUILDVARIANT': p}]

    script_locations = get_script_locations(build_info, 'windows')

    return {
        'version': '{build}',
        'skip_tags': True,
        'branches': {
            'only': deploy_info.build_branches
        },
        'configuration': ['Debug'],
        'platform': 'x64',
        'clone_script': [
            {
                'cmd': 'git clone -q --recursive --branch=%APPVEYOR_REPO_BRANCH% https://github.com/%APPVEYOR_REPO_NAME%.git %SOURCE_DIR%',
            },
            {
                'cmd': 'cd %SOURCE_DIR% && git checkout -qf %APPVEYOR_REPO_COMMIT%'
            }
        ],
        'image': ['Visual Studio 2015', 'Visual Studio 2017'],
        'matrix': {
            'allow_failures': allow_fails,
            'exclude': [
                {
                    'BUILDVARIANT': 'uwp.amd64',
                    'image': 'Visual Studio 2015'
                }
                ]
        },
        'environment': {
            'matrix': matrix,
            'BUILD_DIR': 'C:\\projects\\%APPVEYOR_PROJECT_SLUG%',
            'SOURCE_DIR': 'C:\\projects\\%APPVEYOR_PROJECT_SLUG%\\src',
            'NOBUILD': 1,
            'SAME_BUILD_DIR': 1,
            'CMAKE_BIN': 'cmake.exe',
            'MAKEFILE_DIR': make_loc,
            'DEPENDENCIES': dependencies_list,
            'DEPLOY_PATTERNS': deploy_patterns,
            'GITHUB_TOKEN': {
                'secure': secret
            }
        },
        'install': [
            {'ps': script} for script in script_locations.deps
        ],
        'build': {
            'parallel': True,
            'verbosity': 'minimal',
            'project': '%s.sln' % (try_get_key(build_info, 'name', 'coffee'),)
        },
        'before_build': [
            {'ps': script} for script in script_locations.build
        ],
        'before_deploy': [
            {'cmd': 'cmake.exe --build %BUILD_DIR% --target install --config %CONFIGURATION%'}
        ],
        'deploy_script': [
            {'ps': script} for script in script_locations.deploy
        ]
    }
