#!/bin/bash

function gen_information()
{
    local url=""

    if [ ! -z $TRAVIS ]; then
        local url="https://travis-ci.org/$REPO_SLUG/jobs/$TRAVIS_JOB_ID"
    fi

    if [ ! -z $ ]; then
        local url="$JENKINS_URL/job/$JOB_NAME/$BUILD_NUMBER"
    fi

    if [ "$url" == "" ]; then
        local url="https://github.com/$REPO_SLUG/commit/$(git rev-parse HEAD)"
    fi

    echo "{\"status\":$1,\"url\":\"$url\"}"
}

BASEDIR=$(dirname $0)
IDENTITY="$($BASEDIR/../../ci-identify.sh)"
JSON_DATA="$(OUTPUT_TYPE=json $BASEDIR/ci-identify.sh)"

function get_q()
{
    echo "$JSON_DATA" | jq $1 | cut -d '"' -f2
}

echo "
################################################################################
#################### Submitting build status to Firebase #######################
################################################################################
"
echo $JSON_DATA | jq

if [ -z $IDENTITY ]; then
    echo " * Failed to establish CI identity"
    exit 1
fi

echo " * Submitting request to:"
echo "   $IDENTITY"

export REPO_SLUG="$(get_q .repo)"

$BASEDIR/firebase-report.sh PUT "$IDENTITY" "$(gen_information $@)"

export MSG_TITLE="$(get_q .service)"
export MSG_BODY="$(get_q .repo):$(get_q .build_id) build failed on $(get_q .branch)"
export MSG_TAG="$(get_q .repo):$(get_q .branch):$(get_q .build_id)"
export MSG_DESCRIPTION="The $(get_q .build_id) build failed"

# Some builds allow failure, just let them pass, no notification
[ ! -z $TRAVIS ] && [ "$TRAVIS_ALLOW_FAILURE" == "true" ] && exit 0

$BASEDIR/firebase-pop-notification.sh

