from python.ci.common import flatten_map, get_deploy_info, create_target_matrix, get_script_locations
from python.common import try_get_key
from collections import defaultdict
from copy import deepcopy

def merge(dict1, dict2):
    return {**dict1, **dict2}

def github_gen_config(build_info, repo_dir):
    script_loc = try_get_key(build_info, 'script_location', 'ci')
    make_loc = try_get_key(build_info, 'makefile_location', 'ci')

    linux_targets = create_target_matrix(repo_dir + '/' + make_loc, 'linux', build_info)
    macos_targets = create_target_matrix(repo_dir + '/' + make_loc, 'osx', build_info)
    windows_targets = create_target_matrix(repo_dir + '/' + make_loc, 'windows', build_info)

    linux_targets = {x: {'variant': x} for x in linux_targets }
    macos_targets = {x: {'variant': x} for x in macos_targets }
    windows_targets = {x: {'variant': x} for x in windows_targets }

    include_branches = [x['name'] for x in try_get_key(build_info, 'branches', [])]

    unix_env = {
                'CONFIGURATION': 'Release',
                'GENERATE_PROGRAMS': 'ON',
                'BUILDVARIANT': '${{ matrix.variant }}',
                'ACTIONS': '1',
                'MAKEFILE_DIR': 'toolchain/makers',
                'SOURCE_DIR': '${{ github.workspace }}/source',
                'BUILD_DIR': '${{ github.workspace }}/build',
                'BUILD_REPO_URI': '${{ github.repository }}',
                'BUILD_REPO_BRANCH': '${{ github.ref }}',
                'BUILD_REPO_EVENT': 'push',
                'BUILD_REPO_ID': '',
                'BUILD_REPO_URL': 'https://github.com/${{ github.repository }}',
                'TRAVIS_COMMIT': '${{ github.sha }}',
                'TRAVIS_REPO_SLUG': '${{ github.repository }}',
                'GITHUB_TOKEN': '${{ secrets.GITHUB_TOKEN }}'
        }
    linux_env = unix_env.copy()
    osx_env = unix_env.copy()

    linux_env['TRAVIS_OS_NAME'] = 'linux'
    osx_env['TRAVIS_OS_NAME'] = 'osx'

    windows_env = {
            'AZURE_IMAGE': 'vs2019-win2019',
            'VSVERSION': '2019',
            'OPENSSL_ROOT_DIR': '$(Build.SourcesDirectory)/openssl-libs/',
            'ACTIONS': '1',
            'BUILD_REPO_URI': '${{ github.repository }}',
            'BUILD_REPO_BRANCH': '${{ github.ref }}',
            'BUILD_REPO_EVENT': 'push',
            'BUILD_REPO_ID': '${{ matrix.variant }}',
            'BUILD_REPO_URL': 'https://github.com/${{ github.repository }}',
            'GITHUB_TOKEN': '${{ secrets.GITHUB_TOKEN }}',
            'CMAKE_BIN': 'cmake.exe',
            'MAKEFILE_DIR': 'toolchain/makers',
            'SAME_BUILD_DIR': '1',
            'NOBUILD': '1',
            'SOURCE_DIR': '${{ github.workspace }}/source',
            'BUILD_DIR': '${{ github.workspace }}/build',
            'APPVEYOR_BUILD_FOLDER': '${{ github.workspace }}/build',
            'APPVEYOR_REPO_NAME': '${{ github.repository }}',
            'APPVEYOR_REPO_COMMIT': '${{ github.sha }}',
            'BUILDVARIANT': '${{ matrix.variant }}',
            'GENERATE_PROGRAMS': 'ON',
            'CONFIGURATION': 'Debug'
        }

    linux_strategy = defaultdict(list)
    macos_strategy = defaultdict(list)
    windows_strategy = defaultdict(list)
    android_strategy = defaultdict(list)

    for val in linux_targets.values():
        for key in val:
            if val[key].startswith('android.'):
                android_strategy[key].append(val[key])
            else:
                linux_strategy[key].append(val[key])
    for val in macos_targets.values():
        for key in val:
            macos_strategy[key].append(val[key])
    for val in windows_targets.values():
        for key in val:
            windows_strategy[key].append(val[key])

    linux_strategy = dict(linux_strategy)
    macos_strategy = dict(macos_strategy)
    windows_strategy = dict(windows_strategy)
    android_strategy = dict(android_strategy)

    checkout_step = {
            'uses': 'actions/checkout@v2',
            'with': {
                'submodules': True,
                'path': 'source'
                }
            }
    package_step = [
            {
                'name': 'Compress executables',
                'run': 'source/cb compress-usr-dir bin'
            },
            {
                'name': 'Compress libraries',
                'run': 'source/cb compress-usr-dir libraries'
            },
            {
                'name': 'Uploading artifacts',
                'uses': 'actions/upload-artifact@v2',
                'with': {
                    'name': '${{matrix.variant}}',
                    'path': '*.tar.bz2'
                }
            },
            {
                'name': 'Uploading assets',
                'run': 'source/cb push-asset tar.bz2'
            }
            ]

    return {
            'name': 'CMake Build',
            'on': {
                'push': {
                    'branches': ['master', 'testing', 'feature**']
                    }
                },
            'jobs': {
                'Linux': {
                    'runs-on': 'ubuntu-18.04',
                    'strategy': {
                        'fail-fast': False,
                        'matrix': linux_strategy
                    },
                    'env': linux_env.copy(),
                    'steps': [
                    deepcopy(checkout_step),
                    {
                        'name': 'Building project',
                        'run': 'source/cb docker-build -GNinja'
                    }
                    ] + deepcopy(package_step)
                },
                'Coverage': {
                    'runs-on': 'ubuntu-18.04',
                    'strategy': {
                        'fail-fast': False,
                        'matrix': {'variant': ['coverage']}
                    },
                    'env': linux_env.copy(),
                    'steps': [
                    deepcopy(checkout_step),
                    {
                        'name': 'Building project',
                        'run': 'source/cb docker-build -GNinja'
                    },
                    {
                        'name': 'Running tests',
                        'env': { 'BUILD_TARGET': 'CoverageTest' },
                        'run': 'source/cb docker-build -GNinja'
                    },
                    {
                        'name': 'Gathering coverage info',
                        'uses': 'codecov/codecov-action@v1'
                    }
                    ]
                },
                'Android': {
                    'runs-on': 'ubuntu-18.04',
                    'strategy': {
                        'fail-fast': False,
                        'matrix': android_strategy
                    },
                    'env': linux_env.copy(),
                    'steps': [
                    deepcopy(checkout_step),
                    {
                        'name': 'Building project',
                        'run': 'source/cb docker-build -GNinja'
                    }
                    ] + deepcopy(package_step)
                },
                'macOS': {
                    'runs-on': 'macos-latest',
                    'strategy': {
                        'fail-fast': False,
                        'matrix': macos_strategy
                    },
                    'env': osx_env.copy(),
                    'steps': [
                    deepcopy(checkout_step),
                    {
                        'name': 'Installing system dependencies',
                        'run': 'source/toolchain/ci/travis-deps.sh'
                    },
                    {
                        'name': 'Building project',
                        'run': 'source/cb ci-build -GXcode',
                        'env': {
                            'BUILD_TARGET': 'ALL_BUILD'
                        }
                    }
                    ] + deepcopy(package_step)
                },
                'Windows': {
                    'runs-on': 'windows-2019',
                    'strategy': {
                        'fail-fast': False,
                        'matrix': windows_strategy
                    },
                    'env': windows_env,
                    'steps': [
                    deepcopy(checkout_step),
                    {
                        'run': 'source/toolchain/ci/appveyor-deps.ps1',
                        'shell': 'powershell',
                        'name': 'Downloading dependencies'
                    },
                    {
                        'run': 'echo "::add-path::C:/Program Files/Nasm"',
                        'name': 'Add Nasm to PATH'
                    },
                    {
                        'run': 'source/toolchain/ci/appveyor-build.ps1',
                        'shell': 'powershell',
                        'name': 'Configuring project'
                    },
                    {
                        'run': '& cmake.exe --build $env:BUILD_DIR --target install --config $env:CONFIGURATION',
                        'shell': 'powershell',
                        'name': 'Building project'
                    },
                    {
                        'run': 'source/toolchain/ci/appveyor-deploy.ps1',
                        'shell': 'powershell',
                        'name': 'Deploying artifacts',
                        'continue-on-error': True
                    },
                    {
                        'name': 'Uploading artifacts',
                        'uses': 'actions/upload-artifact@v2',
                        'with': {
                            'name': '${{matrix.variant}}',
                            'path': 'build/*.7z'
                        }
                    }
                    ]
                }
            }
        }
