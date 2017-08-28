def linux_targets = ['ubuntu.i686', 'ubuntu.amd64', 'steam.amd64', 'maemo.armel', 'fedora.amd64', 'emscripten.wasm', 'emscripten.asmjs', 'android.armv8a', 'android.armv7a.kitkat', 'android.armv7a', ]
def osx_targets = ['osx', 'ios.x86_64', ]
def windows_targets = ['win32.amd64', 'uwp.amd64', ]

String linux_label = 'linux && docker'
String osx_label = 'osx'
String windows_label = 'windows'

void GetSourceStep(job, repoUrl, srcDir)
{
    job.with {
        wrappers {
            credentialsBinding {
                string("GH_API_TOKEN", "GithubToken")
            }
        }
        parameters {
            stringParam('GH_BRANCH', "master", 'Name of Git ref to build')
        }
        scm {
            git {
                remote {
                    name('origin')
                    url(repoUrl)
                }
                branch('${GH_BRANCH}')
                extensions {
                    relativeTargetDirectory(srcDir)
                    submoduleOptions {
                        recursive(true)
                    }
                    cloneOptions {
                        shallow(true)
                    }
                }
            }
        }
    }
}

void GetBuildStep(job, srcDir, platform, targetLabel, target)
{
    def cmd = ""

    job.with {
        steps {
            environmentVariables {
                env('BUILDVARIANT', target)
            }
        }
    }

    if (platform == 'linux' || platform == 'osx')
    {
        cmd = "\"${srcDir}/tools/ci/travis-build.sh\""
        job.with {
            steps {
                environmentVariables {
                    env('TRAVIS_OS_NAME', platform)
                    env('TRAVIS_BUILD_DIR', srcDir)
                    env('MAKEFILE_DIR', 'tools/makers')
                    env('DEPENDENCIES', '')
                    env('GITHUB_TOKEN', '${GH_API_TOKEN}')
                    env('MANUAL_DEPLOY', '1')
                    env('MANUAL_CONTEXT', 'continuous-integration/jenkins')
                }
            }
        }
    }else if(platform == 'windows')
    {
        cmd = "powershell \"${srcDir}\\tools/ci\\appveyor-build.ps1\""

        job.with {
            steps {
                environmentVariables {
                    env('APPVEYOR_BUILD_FOLDER', srcDir)
                    env('BUILD_DIR', '${WORKSPACE}/build_' + target)
                    env('CMAKE_BIN', 'C:\\Program Files\\CMake\\bin\\cmake.exe')
                    env('MAKEFILE_DIR', 'tools/makers')
                    env('DEPENDENCIES', '')
                    env('GITHUB_TOKEN', '${GITHUB_TOKEN}')
                    env('BUILDVARIANT', target)
                    env('MANUAL_DEPLOY', '1')
                    env('MANUAL_CONTEXT', 'continuous-integration/jenkins')
                }
            }
        }
    }

    job.with {
        label(targetLabel)
        steps {
            shell(cmd)
        }
    }
}

linux_targets.each {
    def j = job("Coffee_${it}")

    def srcDir = '${WORKSPACE}'

    GetSourceStep(j, 'ssh://git@github.com/hbirchtree/coffeecutie.git', srcDir)
    GetBuildStep(j, srcDir, 'linux', linux_label, it)
}

osx_targets.each {
    def j = job("Coffee_${it}")

    def srcDir = '${WORKSPACE}'

    GetSourceStep(j, 'ssh://git@github.com/hbirchtree/coffeecutie.git', srcDir)
    GetBuildStep(j, srcDir, 'osx', osx_label, it)
}

windows_targets.each {
    def j = job("Coffee_${it}")

    def srcDir = '${WORKSPACE}'

    GetSourceStep(j, 'ssh://git@github.com/hbirchtree/coffeecutie.git', srcDir)
    GetBuildStep(j, srcDir, 'windows', windows_label, it)
}