WORKSPACE_LOC="/tmp/coffeebuild_osx"
PROJ_NAME="coffee_lin"
REPO_URL="https://github.com/hbirchtree/coffeecutie.git"
REPO_BRANCH="master"
PLATFORM_NAME="OS X"
PLAT_LABEL="macintosh && x64"

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
  deliveryPipelineConfiguration("${PIPELINE_NAME}",'Initial setup')
  
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
  
  steps {
    shell(
    """
      cd "${WORKSPACE_LOC}/src/desktop/osx/"
      bash "gen_icons.sh"
    """)
  }
}

job("1.0.${PLATFORM_NAME}-debug-compile") {
  label("${PLAT_LABEL}")
  customWorkspace("${WORKSPACE_LOC}")
  deliveryPipelineConfiguration("${PIPELINE_NAME}",'Debug building')
  
  triggers {
    upstream("0.0.${PLATFORM_NAME}-dep-setup",'SUCCESS')
  }
  
  steps {
    shell(
    """
      mkdir -p "${WORKSPACE_LOC}/build-debug"
      cd "${WORKSPACE_LOC}/build-debug"
      bash -c 'source ~/.bashrc; export PATH=$PATH;/usr/local/bin ; env; cmake -G Ninja -D CMAKE_INSTALL_PREFIX=out/ -D COFFEE_BUILD_OPENAL=OFF; cmake --build . --target install'
    """)
  }
}

job("1.1.${PLATFORM_NAME}-debug-test") {
  label("${PLAT_LABEL}")
  customWorkspace("${WORKSPACE_LOC}")
  deliveryPipelineConfiguration("${PIPELINE_NAME}",'Debug testing')
  
  triggers {
    upstream("1.0.${PLATFORM_NAME}-debug-compile",'SUCCESS')
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

job("2.0.${PLATFORM_NAME}-release-compile") {
  label("${PLAT_LABEL}")
  customWorkspace("${WORKSPACE_LOC}")
  deliveryPipelineConfiguration("${PIPELINE_NAME}",'Release building')
  
  triggers {
    upstream("1.1.${PLATFORM_NAME}-debug-test",'SUCCESS')
  }
  
  steps {
    cmake {
      generator('Ninja')
      args('-DCMAKE_INSTALL_PREFIX=out -DCOFFEE_BUILD_OPENAL=OFF')
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

job("2.1.${PLATFORM_NAME}-release-test") {
  label("${PLAT_LABEL}")
  customWorkspace("${WORKSPACE_LOC}")
  deliveryPipelineConfiguration("${PIPELINE_NAME}",'Release testing')
  
  triggers {
    upstream("2.0.${PLATFORM_NAME}-release-compile",'SUCCESS')
  }
  
  publishers {
    archiveArtifacts {
      pattern('build-release/out/**')
      onlyIfSuccessful()
    }
  }
  
  steps {
    shell('for test in build-release/out/bin/Unit_*; do exec $test 2>/dev/null; done')
  }
}
