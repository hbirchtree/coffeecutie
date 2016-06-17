PROJ_NAME="coffee_lin"
REPO_URL="https://github.com/hbirchtree/coffeecutie.git"

job("1.cmakebuild") {
  scm {
    git {
      remote {
        name('origin')
        url("${REPO_URL}")
      }
      branch('master')
    }
  }
  properties {
    environmentVariables {
      keepSystemVariables(true)
      keepBuildVariables(true)
    }
  }
  triggers {
    scm('H/10 * * * *')
  }
  steps {
    
  }
}
