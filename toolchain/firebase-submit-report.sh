#!/bin/bash

BASEDIR=$(dirname $0)

IDENTITY="$($BASEDIR/ci-identify.sh)"

OUTPUT_TYPE=json $BASEDIR/ci-identify.sh

echo "
********************************************************************************
******************** Submitting build status to Firebase ***********************
********************************************************************************
"

if [ -z $IDENTITY ]; then
    echo " * Failed to establish CI identity"
    exit 1
fi

echo " * Submitting request to:"
echo "   $IDENTITY"

$BASEDIR/firebase-report.sh PUT "$IDENTITY" $@

JSON_DATA="$(OUTPUT_TYPE=json $BASEDIR/ci-identify.sh)"

export MSG_TITLE="$(echo $JSON_DATA | jq .service)"
export MSG_BODY="$(echo $JSON_DATA | jq .repo):$(echo $JSON_DATA | jq .build_id) build failed on $(echo $JSON_DATA | jq .branch)"
export MSG_TAG="$(echo $JSON_DATA | jq .repo):$(echo $JSON_DATA | jq .branch):$(echo $JSON_DATA | jq .build_id)"
export MSG_DESCRIPTION="The $(echo $JSON_DATA | jq .build_id) build failed"

$BASEDIR/firebase-pop-notification.sh

exit $?
