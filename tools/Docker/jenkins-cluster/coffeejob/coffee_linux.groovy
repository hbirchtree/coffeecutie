WORKSPACE_LOC="/tmp/coffeebuild_lin"
PROJ_NAME="coffee_lin"
REPO_URL="https://github.com/hbirchtree/coffeecutie.git"
REPO_BRANCH="master"

deliveryPipelineView('linux-build') {
  allowPipelineStart(true)
}

job('1.0.coffeebuild_lin-debug') {
  customWorkspace("${WORKSPACE_LOC}")
  deliveryPipelineConfiguration('linux-build','debug-compilation')
  
  scm {
    git {
      remote {
        name('origin')
        url("${REPO_URL}")
      }
      branch("${REPO_BRANCH}")
    }
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

job('1.1.coffeebuild_lin-debug-test') {
  customWorkspace("${WORKSPACE_LOC}")
  deliveryPipelineConfiguration('linux-build','debug-testing')
  
  triggers {
    upstream("1.0.coffeebuild_lin-debug",'SUCCESS')
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

job("2.0.coffeebuild_lin-release") {
  customWorkspace("${WORKSPACE_LOC}")
  deliveryPipelineConfiguration('linux-build','release-compilation')
  
  triggers {
    upstream("1.1.coffeebuild_lin-debug-test",'SUCCESS')
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

job('2.1.coffeebuild_lin-release-test') {
  customWorkspace("${WORKSPACE_LOC}")
  deliveryPipelineConfiguration('linux-build','release-testing')
  
  triggers {
    upstream("2.0.coffeebuild_lin-release",'SUCCESS')
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
