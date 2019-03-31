def linux_targets = [@LINUX_TARGETS@]
def osx_targets = [@OSX_TARGETS@]
def windows_targets = [@WINDOWS_TARGETS@]

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

    job.with {
        steps {
            shell("git -C \"${srcDir}\" submodule update --init --recursive")
        }
    }

    if (platform == 'linux' || platform == 'osx')
    {
        cmd = "\"@UNIX_BUILD_SCRIPT@\""
        job.with {
            steps {
                environmentVariables {
                    env('TRAVIS_OS_NAME', platform)
                    env('TRAVIS_BUILD_DIR', srcDir)
                    env('MAKEFILE_DIR', '@MAKEFILE_DIR@')
                    env('DEPENDENCIES', '@DEPENDENCIES_NIX@')
                    env('GITHUB_TOKEN', '${GH_API_TOKEN}')
                    env('MANUAL_DEPLOY', '1')
                    env('MANUAL_CONTEXT', 'continuous-integration/jenkins')
                }
            }
        }
    }else if(platform == 'windows')
    {
        cmd = "powershell \"@WINDOWS_BUILD_SCRIPT@\""

        job.with {
            steps {
                environmentVariables {
                    env('APPVEYOR_BUILD_FOLDER', srcDir)
                    env('BUILD_DIR', '${WORKSPACE}/build_' + target)
                    env('CMAKE_BIN', 'C:\\Program Files\\CMake\\bin\\cmake.exe')
                    env('MAKEFILE_DIR', '@MAKEFILE_DIR@')
                    env('DEPENDENCIES', '@DEPENDENCIES@')
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
    def j = job("@PROJECT_NAME@_${it}")

    def srcDir = '${WORKSPACE}'

    GetSourceStep(j, '@REPO_URL@', srcDir)
    GetBuildStep(j, srcDir, 'linux', linux_label, it)
}

osx_targets.each {
    def j = job("@PROJECT_NAME@_${it}")

    def srcDir = '${WORKSPACE}'

    GetSourceStep(j, '@REPO_URL@', srcDir)
    GetBuildStep(j, srcDir, 'osx', osx_label, it)
}

windows_targets.each {
    def j = job("@PROJECT_NAME@_${it}")

    def srcDir = '${WORKSPACE}'

    GetSourceStep(j, '@REPO_URL@', srcDir)
    GetBuildStep(j, srcDir, 'windows', windows_label, it)
}
