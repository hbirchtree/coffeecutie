from python.ci.common import flatten_map, get_deploy_info, create_target_matrix, get_script_locations
from python.common import try_get_key
from collections import defaultdict

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
#    linux_strategy['os'] = 'ubuntu-18.04'

    macos_strategy = defaultdict(list)
#    macos_strategy['os'] = 'macos-10.15'

    windows_strategy = defaultdict(list)
#    windows_strategy['os'] = 'windows-2019'

    for val in linux_targets.values():
        for key in val:
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
                        'matrix': linux_strategy
                    },
                    'env': linux_env.copy(),
                    'steps': [
                    {
                        'uses': 'actions/checkout@v2',
                        'with': {
                            'submodules': True,
                            'path': 'source'
                        }
                    },
                    {
                        'name': 'Select Docker container',
                        'run': '${{ github.workspace }}/source/.github/cmake/select/${{ matrix.variant }}.sh'
                    },
                    {
                        'name': 'Building project',
                        'uses': 'lukka/run-cmake@v2',
                        'with': {
                            'cmakeListsTxtPath': '${{ github.workspace }}/source/CMakeLists.txt',
                            'buildDirectory': '${{ github.workspace }}/build',
                            'cmakeAppendedArgs': '-C${{ github.workspace }}/source/.github/cmake/${{matrix.variant}}.preload.cmake'
                        }
                    }
                    ]
                },
                'Android': {
                    'runs-on': 'ubuntu-18.04',
                    'strategy': {
                        'matrix': {
                            'variant': ['android.armv8a.v29', 'android.armv7a.v29']
                        }
                    },
                    'container': {
                        'image': 'hbirch/android:r21',
                        'env': linux_env.copy(),
                        'volumes': [
                            '${{ github.workspace }}/source:/source',
                            '${{ github.workspace }}/build:/build'
                        ]
                    },
                    'steps': [
                    {
                        'uses': 'actions/checkout@v2',
                        'with': {
                            'submodules': True,
                            'path': '/source'
                        }
                    },
                    {
                        'name': 'Select Docker container',
                        'run': '${{ github.workspace }}/source/.github/cmake/select/${{ matrix.variant }}.sh'
                    },
                    {
                        'name': 'Building project',
                        'uses': 'lukka/run-cmake@v2',
                        'with': {
                            'cmakeListsTxtPath': '/source/CMakeLists.txt',
                            'buildDirectory': '/build',
                            'cmakeAppendedArgs': '-C/source/.github/cmake/${{ matrix.variant }}.preload.cmake'
                        }
                    }
                    ]
                },
                'macOS': {
                    'runs-on': 'macos-latest',
                    'strategy': {
                        'matrix': macos_strategy
                    },
                    'env': osx_env.copy(),
                    'steps': [
                    {
                        'uses': 'actions/checkout@v2',
                        'with': {
                            'submodules': True,
                            'path': 'source'
                        }
                    },
                    {
                        'name': 'Installing system dependencies',
                        'run': 'source/toolchain/ci/travis-deps.sh'
                    },
                    {
                        'name': 'Building project',
                        'uses': 'lukka/run-cmake@v2',
                        'with': {
                            'cmakeListsOrSettingsJson': 'CMakeListsTxtAdvanced',
                            'cmakeListsTxtPath': '${{ github.workspace }}/source/CMakeLists.txt',
                            'buildDirectory': '${{ github.workspace }}/build',
                            'cmakeAppendedArgs': '-GXcode -C${{ github.workspace }}/source/.github/cmake/${{ matrix.variant }}.preload.cmake'
                        }
                    }
                    ]
                },
                'Windows': {
                    'runs-on': 'windows-2019',
                    'strategy': {
                        'matrix': windows_strategy
                    },
                    'env': windows_env,
                    'steps': [
                    {
                        'uses': 'actions/checkout@v2',
                        'with': {
                            'submodules': True,
                            'path': 'source'
                        }
                    },
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
                    }
                    ]
                }
            }
        }
