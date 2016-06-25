def REPO_BRANCH = 'master'
def REPO_URL = 'https://github.com/hbirchtree/coffeecutie.git'

def NUM_PLATFORMS = 4
def NUM_FLAVORS = 2

def CMAKE_GENERATORS = ['Ninja', 'Visual Studio 14 2015 Win64', 'Ninja', 'Ninja']
def CMAKE_OPTIONS    = [
    '',
    '',
    '',
    '-DCOFFEE_BUILD_OPENAL=OFF'
    ]
def PLATFORM_STRING  = ['Linux', 'Windows', 'Android', 'OS X']
def HAS_DEPENDENCIES = [false,   false,     true,      false]
def PLATFORM_FLAG    = [0,       3,         1,         2]
def PLATFORM_LABELS  = ['ubuntu && x64', 'windows && x64', 'android', 'macintosh && x64']


def RELEASE_TYPES = ['Debug', 'Release']

gitCheckoutProc = {
            git {
              remote {
                name('origin')
                url("${REPO_URL}")
              }
              branch("${REPO_BRANCH}")
              extensions {
                relativeTargetDirectory('src')
                submoduleOptions {
                  recursive(true)
                }
                cloneOptions {
                  shallow(true)
                }
              }
            }
          }
gitTrigger = {
            scm('H/10 * * * *')
            githubPush()
          }

for(i in 0..(NUM_PLATFORMS-1)) {
    def PLATFORM_NAME = PLATFORM_STRING[i]
    def DEPENDS = HAS_DEPENDENCIES[i]
    def FLAG = PLATFORM_FLAG[i]
    def PLAT_LABEL = PLATFORM_LABELS[i]
    def CMAKE_GENERATOR = CMAKE_GENERATORS[i]

    def PIPELINE_NAME = "${PLATFORM_NAME}"
    def WORKSPACE_LOC = "/tmp/${PLATFORM_NAME}/"

    def GIT_CLONE = gitCheckoutProc
    def GIT_TRIGGER = gitTrigger
    
    def PREV_STEP = ""
    def DEPENDENCY_STEP = "0.0.${PLATFORM_NAME}-dep-setup"
    
    def DEP_STEPS = {}
    def DEP_STATUS = 'SUCCESS'

    deliveryPipelineView("${PIPELINE_NAME}") {
      allowPipelineStart(true)
      showTotalBuildTime(true)
      pipelines {
        component("${PLATFORM_NAME}",DEPENDENCY_STEP)
      }
    }
    
    if(FLAG == 2) {
      DEP_STEPS = {
        shell(
          """
            cd "${WORKSPACE_LOC}/src/desktop/osx/"
            bash "gen_icons.sh"
          """)
      }
      
    job(DEPENDENCY_STEP) {
      label("${PLAT_LABEL}")
      customWorkspace("${WORKSPACE_LOC}")
      deliveryPipelineConfiguration("${PIPELINE_NAME}","Dependency stage")
      
      steps {
        shell(
          """
            cd "${WORKSPACE_LOC}/src/desktop/osx/"
            bash "gen_icons.sh"
          """)
      }
      
      scm {
      git {
        remote {
          name('origin')
          url("${REPO_URL}")
        }
        branch("${REPO_BRANCH}")
        extensions {
          relativeTargetDirectory('src')
          submoduleOptions {
            recursive(true)
          }
          cloneOptions {
            shallow(true)
          }
        }
      }
    }

      triggers {
      scm('H/10 * * * *')
      githubPush()
    }
    }
    }else if(FLAG == 3) {
      DEP_STATUS = 'FAILURE'
      
      job(DEPENDENCY_STEP) {
      label("${PLAT_LABEL}")
      customWorkspace("${WORKSPACE_LOC}")
      deliveryPipelineConfiguration("${PIPELINE_NAME}","Dependency stage")
      
      steps {
        batchFile(
          """
            mkdir build-Debug
            mkdir build-Release
            mklink /J build-Debug\\libs libs
            mklink /J build-Release\\libs libs
          """)
      }
      
      scm {
      git {
        remote {
          name('origin')
          url("${REPO_URL}")
        }
        branch("${REPO_BRANCH}")
        extensions {
          relativeTargetDirectory('src')
          submoduleOptions {
            recursive(true)
          }
          cloneOptions {
            shallow(true)
          }
        }
      }
    }

      triggers {
      scm('H/10 * * * *')
      githubPush()
    }
    }
    }else {


    job(DEPENDENCY_STEP) {
      label("${PLAT_LABEL}")
      customWorkspace("${WORKSPACE_LOC}")
      deliveryPipelineConfiguration("${PIPELINE_NAME}","Dependency stage")
      
      scm {
      git {
        remote {
          name('origin')
          url("${REPO_URL}")
        }
        branch("${REPO_BRANCH}")
        extensions {
          relativeTargetDirectory('src')
          submoduleOptions {
            recursive(true)
          }
          cloneOptions {
            shallow(true)
          }
        }
      }
    }

      triggers {
      scm('H/10 * * * *')
      githubPush()
    }
    }
    
    }

    PREV_STEP = DEPENDENCY_STEP
    
    if(DEPENDS) {
        job("0.1.${PLATFORM_NAME}-dep-SDL2") {
          label("${PLAT_LABEL}")
          customWorkspace("${WORKSPACE_LOC}")

          scm {
            hg('https://hg.libsdl.org/SDL') {
              tag('release-2.0.4')
              subdirectory('SDL2')
            }
          }

          triggers {
            upstream(DEPENDENCY_STEP,DEP_STATUS)
          }

          steps {
            cmake {
              generator("${CMAKE_GENERATOR}")
              args("-DCMAKE_INSTALL_PREFIX=${WORKSPACE_LOC}/libs -DSDL_SHARED=ON -DSDL_STATIC=ON -DSDL_STATIC_PIC=ON -DSDL_ATOMIC=OFF -DSDL_TIMERS=OFF -DSDL_AUDIO=OFF -DSDL_THREADS=OFF -DSDL_CPUINFO=OFF")
              sourceDir('SDL2')
              buildDir('SDL2_build')
              buildType('Release')
              buildToolStep {
                useCmake(true)
                args('--target install')
              }
            }
          }
        }

        job("0.2.${PLATFORM_NAME}-dep-openal-soft") {
          label("${PLAT_LABEL}")
          customWorkspace("${WORKSPACE_LOC}")

          scm {
            git {
              remote {
                name('origin')
                url("https://github.com/kcat/openal-soft.git")
              }
              branch("master")
              extensions {
                relativeTargetDirectory('openal-soft')
                cloneOptions {
                  shallow(true)
                }
              }
            }
          }

          triggers {
            upstream("0.1.${PLATFORM_NAME}-dep-SDL2",'SUCCESS')
          }

          steps {
            cmake {
              generator("${CMAKE_GENERATOR}")
              args("-DCMAKE_INSTALL_PREFIX=${WORKSPACE_LOC}/libs")
              sourceDir('openal-soft')
              buildDir('openal-soft_build')
              buildType('Release')
              buildToolStep {
                useCmake(true)
                args('--target install')
              }
            }
          }
        }

        job("0.9.${PLATFORM_NAME}-dep-complete") {
          label("${PLAT_LABEL}")

          triggers {
            upstream("0.2.${PLATFORM_NAME}-dep-openal-soft",'SUCCESS')
          }
        }

        PREV_STEP = "0.9.${PLATFORM_NAME}-dep-complete"
        DEP_STEP = 'SUCCESS'
    }

    for(j in 0..(NUM_FLAVORS-1)) {
        def JOB_NUM = j + 1
        def JOB_RELEASE = RELEASE_TYPES[j]

        cmakeTypicalBuild = {
                    cmake {
                      generator("${CMAKE_GENERATOR}")
                      args('-DCMAKE_INSTALL_PREFIX=out')
                      sourceDir('src')
                      buildDir("build-${JOB_RELEASE}")
                      buildType("${JOB_RELEASE}")
                      buildToolStep {
                        useCmake(true)
                            args('--target install')
                      }
                    }
                  }
        cmakeTypicalTest = {
                    cmake {
                      generator("${CMAKE_GENERATOR}")
                      args('-DCMAKE_INSTALL_PREFIX=out')
                      sourceDir('src')
                      buildDir("build-${JOB_RELEASE}")
                      buildType("${JOB_RELEASE}")
                      buildToolStep {
                        useCmake(true)
                        args('--target test')
                      }
                    }
                  }
        
        def BUILDSTEPS = cmakeTypicalBuild
        def TESTSTEPS = cmakeTypicalTest
        
        def COMPILE_STEP = "${JOB_NUM}.0.${PLATFORM_NAME}-${JOB_RELEASE}-compile"
        def TEST_STEP = "${JOB_NUM}.1.${PLATFORM_NAME}-${JOB_RELEASE}-test"

        job(COMPILE_STEP) {
          label("${PLAT_LABEL}")
          customWorkspace("${WORKSPACE_LOC}")
          deliveryPipelineConfiguration("${PIPELINE_NAME}","${JOB_RELEASE} build")

          triggers {
            upstream(PREV_STEP,DEP_STATUS)
          }

          steps {
                    cmake {
                      generator("${CMAKE_GENERATOR}")
                      args('-DCMAKE_INSTALL_PREFIX=out')
                      sourceDir('src')
                      buildDir("build-${JOB_RELEASE}")
                      buildType("${JOB_RELEASE}")
                      buildToolStep {
                        useCmake(true)
                            args('--target install')
                      }
                    }
                  }

          properties {
            environmentVariables {
              keepSystemVariables(true)
              keepBuildVariables(true)
            }
          }
        }

        if(FLAG != 3){
            job(TEST_STEP) {
              label("${PLAT_LABEL}")
              customWorkspace("${WORKSPACE_LOC}")
              deliveryPipelineConfiguration("${PIPELINE_NAME}","${JOB_RELEASE} test")

              triggers {
                upstream(COMPILE_STEP,'SUCCESS')
              }

              steps {
                        cmake {
                          generator("${CMAKE_GENERATOR}")
                          args('-DCMAKE_INSTALL_PREFIX=out')
                          sourceDir('src')
                          buildDir("build-${JOB_RELEASE}")
                          buildType("${JOB_RELEASE}")
                          buildToolStep {
                            useCmake(true)
                            args('--target test')
                          }
                        }
                      }

              publishers {
                textFinder('The following tests FAILED','',true,false,true)
                archiveArtifacts {
                  pattern("build-${JOB_RELEASE}/out/**")
                }
              }
            }
        }else{
            job(TEST_STEP) {
              label("${PLAT_LABEL}")
              customWorkspace("${WORKSPACE_LOC}")
              deliveryPipelineConfiguration("${PIPELINE_NAME}","${JOB_RELEASE} test")

              triggers {
                upstream(COMPILE_STEP,'SUCCESS')
              }

              steps {
                        cmake {
                          generator("${CMAKE_GENERATOR}")
                          args('-DCMAKE_INSTALL_PREFIX=out')
                          sourceDir('src')
                          buildDir("build-${JOB_RELEASE}")
                          buildType("${JOB_RELEASE}")
                          buildToolStep {
                            useCmake(true)
                            args('--target RUN_TESTS')
                          }
                        }
                      }

              publishers {
                textFinder('The following tests FAILED','',true,false,true)
                archiveArtifacts {
                  pattern("build-${JOB_RELEASE}/out/**")
                }
              }
            }
        }

        PREV_STEP = TEST_STEP
    }
}
