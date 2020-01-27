from python.ci.common import flatten_map, get_deploy_info, create_target_matrix, get_script_locations
from python.common import try_get_key

def pipelines_gen_config(build_info, repo_dir):
    script_loc = try_get_key(build_info, 'script_location', 'ci')
    make_loc = try_get_key(build_info, 'makefile_location', 'ci')

    linux_targets = create_target_matrix(repo_dir + '/' + make_loc, 'linux', build_info)
    macos_targets = create_target_matrix(repo_dir + '/' + make_loc, 'osx', build_info)
    windows_targets = create_target_matrix(repo_dir + '/' + make_loc, 'windows', build_info)

    linux_targets = {x: {'variant': x} for x in linux_targets }
    macos_targets = {x: {'variant': x} for x in macos_targets }
    windows_targets = {x: {'variant': x} for x in windows_targets }

    include_branches = [x['name'] for x in try_get_key(build_info, 'branches', [])]

    return {
            'trigger': {
                'paths': {
                    'include': ['examples/*', 'src/*', 'tests/*', 'toolchain/*']
                    },
                'branches': {
                    'include': include_branches
                    }
                },
            'jobs': [
                {
                    'job': 'Linux',
                    'pool': {'vmImage': 'ubuntu-18.04'},
                    'strategy': {
                        'matrix': linux_targets
                    },
                    'steps': [
                    {
                        'checkout': 'self',
                        'submodules': True
                    },
                    {
                        'script': './toolchain/ci/travis-deps.sh',
                        'displayName': 'Downloading dependencies',
                        'env': {
                            'TRAVIS_OS_NAME': 'linux',
                            'PIPELINES': '1'
                        }
                    },
                    {
                        'script': './cb quick-build $(variant)',
                        'displayName': 'Building project',
                        'env': { 
                            'CONFIGURATION': 'Release',
                            'TRAVIS_OS_NAME': 'linux',
                            'BUILDVARIANT': '$(variant)',
                            'PIPELINES': '1',
                            'BUILD_REPO_URI': '$(Build.Repository.Uri)',
                            'BUILD_REPO_BRANCH': '$(Build.SourceBranch)',
                            'BUILD_REPO_EVENT': '$(Build.Reason)',
                            'BUILD_REPO_ID': '$(variant)',
                            'BUILD_REPO_URL': ''
                        }
                    }
                    ]
                },
                {
                    'job': 'macOS_and_iOS',
                    'pool': {'vmImage': 'macos-10.14'},
                    'strategy': {
                        'matrix': macos_targets
                    },
                    'steps': [
                    {
                        'checkout': 'self',
                        'submodules': True
                    },
                    {
                        'script': './toolchain/ci/travis-deps.sh',
                        'displayName': 'Downloading dependencies',
                        'env': { 
                            'TRAVIS_OS_NAME': 'osx',
                            'PIPELINES': '1'
                        }
                    },
                    {
                        'script': './cb quick-build $(variant)',
                        'displayName': 'Building project',
                        'env': { 
                            'CONFIGURATION': 'Release',
                            'TRAVIS_OS_NAME': 'osx',
                            'BUILDVARIANT': '$(variant)',
                            'PIPELINES': '1',
                            'BUILD_REPO_URI': '$(Build.Repository.Uri)',
                            'BUILD_REPO_BRANCH': '$(Build.SourceBranch)',
                            'BUILD_REPO_EVENT': '$(Build.Reason)',
                            'BUILD_REPO_ID': '$(variant)',
                            'BUILD_REPO_URL': ''
                        }
                    }
                    ]
                },
                {
                    'job': 'Windows',
                    'pool': { 'vmImage': 'windows-2019' },
                    'strategy': {
                        'matrix': windows_targets
                    },
                    'steps': [
                    {
                        'checkout': 'self',
                        'submodules': True
                    },
                    {
                        'powershell': './toolchain/ci/appveyor-deps.ps1',
                        'displayName': 'Downloading dependencies',
                        'env': { 
                            'AZURE_IMAGE': 'vs2019-win2019', 
                            'OPENSSL_ROOT_DIR': '$(Build.SourcesDirectory)/openssl-libs/' ,
                            'PIPELINES': '1'
                        }
                    },
                    {
                        'powershell': './cb.ps1 quick-build $(variant)',
                        'displayName': 'Building project',
                        'env': { 
                            'AZURE_IMAGE': 'vs2019-win2019',
                            'OPENSSL_ROOT_DIR': '$(Build.SourcesDirectory)/openssl-libs/',
                            'PIPELINES': '1',
                            'BUILD_REPO_URI': '$(Build.Repository.Uri)',
                            'BUILD_REPO_BRANCH': '$(Build.SourceBranch)',
                            'BUILD_REPO_EVENT': '$(Build.Reason)',
                            'BUILD_REPO_ID': '$(variant)',
                            'BUILD_REPO_URL': ''
                        }
                    }
                    ]
                }
            ]
    }

    pass
