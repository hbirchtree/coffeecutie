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
                'BUILDVARIANT': '${{ matrix.variant }}',
                'PIPELINES': '1',
                'MAKEFILE_DIR': 'toolchain/makers',
                'BUILD_REPO_URI': '$(Build.Repository.Uri)',
                'BUILD_REPO_BRANCH': '$(Build.SourceBranch)',
                'BUILD_REPO_EVENT': '$(Build.Reason)',
                'BUILD_REPO_ID': '$(variant)',
                'BUILD_REPO_URL': '',
                'TRAVIS_COMMIT': '$(Build.SourceVersion)',
                'TRAVIS_REPO_SLUG': '$(Build.Repository.Name)',
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
            'PIPELINES': '1',
            'BUILD_REPO_URI': '$(Build.Repository.Uri)',
            'BUILD_REPO_BRANCH': '$(Build.SourceBranch)',
            'BUILD_REPO_EVENT': '$(Build.Reason)',
            'BUILD_REPO_ID': '${{ matrix.variant }}',
            'BUILD_REPO_URL': '',
            'GITHUB_TOKEN': '$(Github.Token)',
            'CMAKE_BIN': 'cmake.exe',
            'MAKEFILE_DIR': 'toolchain/makers',
            'SAME_BUILD_DIR': '1',
            'NOBUILD': '1',
            'SOURCE_DIR': '$(Build.SourcesDirectory)',
            'BUILD_DIR': '$(Build.SourcesDirectory)/build',
            'APPVEYOR_BUILD_FOLDER': '$(Build.SourcesDirectory)/build',
            'APPVEYOR_REPO_NAME': '$(Build.Repository.Name)',
            'APPVEYOR_REPO_COMMIT': '$(Build.SourceVersion)',
            'BUILDVARIANT': '$(variant)',
            'CONFIGURATION': 'Debug',
            'PATH': '$(Path);C:/Program Files/NASM'
        }

    linux_strategy = defaultdict(list)
    linux_strategy['os'] = 'ubuntu-18.04'

    macos_strategy = defaultdict(list)
    macos_strategy['os'] = 'macos-10.15'

    windows_strategy = defaultdict(list)
    windows_strategy['os'] = 'windows-2019'

    for val in linux_targets.values():
        for key in val:
            linux_strategy[key].append(val[key])
    for val in macos_targets.values():
        for key in val:
            macos_strategy[key].append(val[key])
    for val in windows_targets.values():
        for key in val:
            windows_strategy[key].append(val[key])

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
                        'run': './toolchain/ci/travis-deps.sh',
                        'shell': 'sh',
                        'name': 'Downloading dependencies'
                    },
                    {
                        'run': './toolchain/ci/travis-build.sh',
                        'shell': 'sh',
                        'name': 'Building project'
                    },
                    {
                        'run': './toolchain/ci/travis-deploy.sh',
                        'shell': 'sh',
                        'name': 'Deploying artifacts',
                        'continue-on-error': True
                    }
                    ]
                },
                'macOS': {
                    'runs-on': 'macos-10.15',
                    'strategy': {
                        'matrix': macos_strategy
                    },
                    'env': osx_env.copy(),
                    'steps': [
                    {
                        'run': './toolchain/ci/travis-deps.sh',
                        'shell': 'sh',
                        'name': 'Downloading dependencies'
                    },
                    {
                        'run': './toolchain/ci/travis-build.sh',
                        'shell': 'sh',
                        'name': 'Building project'
                    },
                    {
                        'run': './toolchain/ci/travis-deploy.sh',
                        'shell': 'sh',
                        'name': 'Deploying artifacts',
                        'continue-on-error': True
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
                        'run': './toolchain/ci/appveyor-deps.ps1',
                        'shell': 'powershell',
                        'name': 'Downloading dependencies'
                    },
                    {
                        'run': './toolchain/ci/appveyor-build.ps1',
                        'shell': 'powershell',
                        'name': 'Configuring project'
                    },
                    {
                        'run': '& cmake.exe --build $env:BUILD_DIR --target install --config $env:CONFIGURATION',
                        'shell': 'powershell',
                        'name': 'Building project'
                    },
                    {
                        'run': './toolchain/ci/appveyor-deploy.ps1',
                        'shell': 'powershell',
                        'name': 'Deploying artifacts',
                        'continue-on-error': True
                    }
                    ]
                }
            }
        }
