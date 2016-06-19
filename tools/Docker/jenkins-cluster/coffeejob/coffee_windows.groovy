WORKSPACE_LOC="/tmp/coffeebuild_win"
PROJ_NAME="coffee_lin"

REPO_URL="https://github.com/hbirchtree/coffeecutie.git"
REPO_BRANCH="master"

CMAKE_GENERATOR="Visual Studio 14 2015 Win64"

PLATFORM_NAME="Windows"
PLAT_LABEL="windows && x64"

PIPELINE_NAME="${PLATFORM_NAME}-pipeline"

deliveryPipelineView("${PIPELINE_NAME}") {
  allowPipelineStart(true)
  showTotalBuildTime(true)
  pipelines {
    component("${PLATFORM_NAME}","1.0.${PLATFORM_NAME}-debug-compile")
  }
}

job("0.0.${PLATFORM_NAME}-dep-setup") {
  label("${PLAT_LABEL}")
  customWorkspace("${WORKSPACE_LOC}")

  steps {
    batchFile("""
      mkdir ${WORKSPACE_LOC}/build-debug
      mkdir ${WORKSPACE_LOC}/build-release
      mklink /J ${WORKSPACE_LOC}/build-debug/libs ${WORKSPACE_LOC}/libs
      mklink /J ${WORKSPACE_LOC}/build-release/libs ${WORKSPACE_LOC}/libs
    """)
  }
}

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
    upstream("0.0.${PLATFORM_NAME}-dep-setup",'SUCCESS')
    scm('H/10 * * * *')
  }
  
  steps {
    cmake {
      generator("${CMAKE_GENERATOR}")
      args("-DCMAKE_INSTALL_PREFIX=${WORKSPACE_LOC}/libs")
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
    upstream("0.0.${PLATFORM_NAME}-dep-setup",'SUCCESS') 
    scm('H/10 * * * *')
    githubPush()
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

job("1.0.${PLATFORM_NAME}-debug-compile") {
  label("${PLAT_LABEL}")
  customWorkspace("${WORKSPACE_LOC}")
  deliveryPipelineConfiguration("${PIPELINE_NAME}",'Debug building')
  
  scm {
    git {
      remote {
        name('origin')
        url("${REPO_URL}")
      }
      branch("${REPO_BRANCH}")
      extensions {
        relativeTargetDirectory('src')
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
  
  properties {
    environmentVariables {
      keepSystemVariables(true)
      keepBuildVariables(true)
    }
  }
  steps {
    cmake {
      generator("${CMAKE_GENERATOR}")
      args('-DCMAKE_INSTALL_PREFIX=out')
      sourceDir('src')
      buildDir('build-debug')
      buildType('Debug')
      buildToolStep {
        useCmake(true)
        args('--target install')
      }
    }
  }
  
  publishers {
    archiveArtifacts {
      pattern('build-debug/out/**')
      onlyIfSuccessful()
    }
  }
}

job("2.0.${PLATFORM_NAME}-release-compile") {
  label("${PLAT_LABEL}")
  customWorkspace("${WORKSPACE_LOC}")
  deliveryPipelineConfiguration("${PIPELINE_NAME}",'Release building')
  
  triggers {
    upstream("1.0.${PLATFORM_NAME}-debug-compile",'SUCCESS')
  }
  
  properties {
    environmentVariables {
      keepSystemVariables(true)
      keepBuildVariables(true)
    }
  }
  
  steps {
    cmake {
      generator("${CMAKE_GENERATOR}")
      args('-DCMAKE_INSTALL_PREFIX=out')
      sourceDir('src')
      buildDir('build-release')
      buildType('Release')
      buildToolStep {
        useCmake(true)
        args('--target install')
      }
    }
  }
  
  publishers {
    archiveArtifacts {
      pattern('build-release/out/**')
      onlyIfSuccessful()
    }
  }
}
