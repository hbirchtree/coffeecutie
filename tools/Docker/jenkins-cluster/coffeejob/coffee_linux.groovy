WORKSPACE_LOC="/tmp/coffeebuild_lin"
PROJ_NAME="coffee_lin"
REPO_URL="https://github.com/hbirchtree/coffeecutie.git"
REPO_BRANCH="master"
PLATFORM_NAME="Linux"
PLAT_LABEL="ubuntu && x64"

PIPELINE_NAME="${PLATFORM_NAME}-pipeline"

deliveryPipelineView("${PIPELINE_NAME}") {
  allowPipelineStart(true)
  showTotalBuildTime(true)
  pipelines {
    component("${PLATFORM_NAME}","1.0.${PLATFORM_NAME}-debug-compile")
  }
}

def num_builds = 2
def JOB_NUM = 0
def builds = ['Debug','Release']

for(i in [1,num_builds])
{
  def JOB_RELEASE = builds[i]
  evaluate(new File("./coffee_buildstep.groove"))
  JOB_NUM = i
}

