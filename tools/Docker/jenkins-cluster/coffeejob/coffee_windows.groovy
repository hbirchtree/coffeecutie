WORKSPACE_LOC="/tmp/coffeebuild_lin"
PROJ_NAME="coffee_lin"
REPO_URL="https://github.com/hbirchtree/coffeecutie.git"
REPO_BRANCH="master"
PLATFORM_NAME="Windows"
PLAT_LABEL="windows && x64"

PIPELINE_NAME="${PLATFORM_NAME}-pipeline"

deliveryPipelineView("${PIPELINE_NAME}") {
  allowPipelineStart(true)
  showTotalBuildTime(true)
  pipelines {
    component("${PLATFORM_NAME}",'1.0.debug-build')
  }
}

job('1.0.debug-compile') {
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
        submoduleOptions {
          recursive(true)
          tracking(true)
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
  
  properties {
    environmentVariables {
      keepSystemVariables(true)
      keepBuildVariables(true)
    }
  }
  steps {
    cmake {
      generator('Ninja')
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
}

job('1.1.debug-test') {
  label("${PLAT_LABEL}")
  customWorkspace("${WORKSPACE_LOC}")
  deliveryPipelineConfiguration("${PIPELINE_NAME}",'Debug testing')
  
  triggers {
    upstream("1.0.debug-compile",'SUCCESS')
  }
  
  publishers {
    archiveArtifacts {
      pattern('build-debug/out/**')
      onlyIfSuccessful()
    }
  }
  
  steps {
    shell('for test in build-debug/out/bin/Unit_*; do exec $test; done')
  }
}

job("2.0.release-compile") {
  label("${PLAT_LABEL}")
  customWorkspace("${WORKSPACE_LOC}")
  deliveryPipelineConfiguration("${PIPELINE_NAME}",'Release building')
  
  triggers {
    upstream("1.1.debug-test",'SUCCESS')
  }
  
  properties {
    environmentVariables {
      keepSystemVariables(true)
      keepBuildVariables(true)
    }
  }
  
  steps {
    cmake {
      generator('Ninja')
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
}

job('2.1.release-test') {
  label("${PLAT_LABEL}")
  customWorkspace("${WORKSPACE_LOC}")
  deliveryPipelineConfiguration("${PIPELINE_NAME}",'Release testing')
  
  triggers {
    upstream("2.0.release-compile",'SUCCESS')
  }
  
  publishers {
    archiveArtifacts {
      pattern('build-release/out/**')
      onlyIfSuccessful()
    }
  }
  
  steps {
    shell('for test in build-release/out/bin/Unit_*; do exec $test; done')
  }
}
