#!/bin/bash

CI_PREFIX="continuous-integration"

OUTPUT_TYPE=${OUTPUT_TYPE:-text}

function create_identity()
{
   if [ "${OUTPUT_TYPE}" == "text" ]; then
        echo "$1/$2/$3/$4/$5"
    else
        echo "{
    \"repo\": \"$1\",
    \"service\": \"$2\",
    \"event\": \"$3\",
    \"branch\": \"$4\",
    \"build_id\": \"$5\"
}"
    fi
 
}

function travis_get_path()
{
    function travis_repo_slug()
    {
        if [ ! -z $TRAVIS_ ]; then
            echo $TRAVIS_PULL_REQUEST_SLUG
        else
            if [ ! -z $TRAVIS_REPO_SLUG ]; then
                echo $TRAVIS_REPO_SLUG
            fi
        fi
    }
    
    local repo_slug=$(travis_repo_slug)
    local repo_event=$TRAVIS_EVENT_TYPE
    local identifier=$BUILDVARIANT

    if [ -z $BUILDVARIANT ]; then
        local identifier="unknown"
    fi

    create_identity "$repo_slug" "$CI_PREFIX/travis-ci" "$TRAVIS_BRANCH" "$repo_event" "$identifier"
}

function jenkins_get_path()
{
    function jenkins_repo_slug()
    {
        echo "$GIT_URL" | sed -rn 's|.*[:/]([A-Za-z0-9\.]+/[A-Za-z0-9\.]+)\.git$|\1|p'
    }

    local identifier=$BUILDVARIANT

    if [ -z $BUILDVARIANT ]; then
        local identifier="unkown"
    fi

    create_identity "$(jenkins_repo_slug)" "$CI_PREFIX/jenkins-ci" "$GIT_BRANCH" "push" "$identifier"
}

CI_IDENTITY="generic-repository/generic-ci/service/push"

if [ ! -z $TRAVIS ]; then
    travis_get_path
fi

if [ ! -z $JENKINS_URL ]; then
    jenkins_get_path
fi
