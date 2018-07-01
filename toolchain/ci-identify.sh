#!/bin/bash

CI_PREFIX="continuous-integration"

OUTPUT_TYPE=${OUTPUT_TYPE:-text}

function create_identity()
{
    local build_id="$(echo $5 | sed -e 's/\./-/g')"

    if [ "${OUTPUT_TYPE}" == "text" ]; then
        echo "${1:-unknown/unknown}/${2:-continuous-integration/unknown-ci}/${4:-push}/${3:-master}/${build_id:-unknown}"
    else
        echo "{
    \"repo\": \"${1:-unknown/unknown}\",
    \"service\": \"${2:-continuous-integration/unknown-ci}\",
    \"event\": \"${3:-push}\",
    \"branch\": \"${4:-master}\",
    \"build_id\": \"${build_id:-unknown}\"
}"
    fi
 
}

# Extra slug from URL
function extract_repo_slug()
{
        echo "$1" | sed -rn 's|.*[:/]([A-Za-z0-9\.]+/[A-Za-z0-9\.]+)\.git$|\1|p'
}

function travis_get_path()
{
    function travis_repo_slug()
    {
        if [ ! -z $TRAVIS_REPO_URL ]; then
            extract_repo_slug $TRAVIS_REPO_URL 
            return
        fi
        if [ ! -z $TRAVIS_PULL_REQUEST_SLUG ]; then
            echo $TRAVIS_PULL_REQUEST_SLUG
            return
        fi
        if [ ! -z $TRAVIS_REPO_SLUG ]; then
            echo $TRAVIS_REPO_SLUG
            return
        fi
    }
    
    local repo_slug=$(travis_repo_slug)
    local repo_event=$TRAVIS_EVENT_TYPE
    local identifier=$BUILDVARIANT
    local branch="$TRAVIS_BRANCH"

    if [ -z $BUILDVARIANT ]; then
        local identifier="unknown"
    fi
    if [ -z $branch ]; then
        local branch=`git -C "$TRAVIS_BUILD_DIR" rev-parse --abbrev-ref HEAD`
    fi

    create_identity "$repo_slug" "$CI_PREFIX/travis-ci" "$TRAVIS_BRANCH" "$repo_event" "$identifier"
}

function jenkins_get_path()
{
    function jenkins_repo_slug()
    {
        extract_repo_slug $GIT_URL
    }

    local identifier=$BUILDVARIANT

    if [ -z $BUILDVARIANT ]; then
        local identifier="unkown"
    fi

    create_identity "$(jenkins_repo_slug)" "$CI_PREFIX/jenkins-ci" "$GIT_BRANCH" "push" "$identifier"
}

CI_IDENTITY="generic-repository/generic-ci/service/push"

if [ ! -z $TRAVIS_BRANCH ]; then
    travis_get_path
fi

if [ ! -z $JENKINS_URL ]; then
    jenkins_get_path
fi
