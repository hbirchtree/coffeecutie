#!/bin/bash

SOURCE_DIR="$PWD"
INFOPY="$SOURCE_DIR/buildinfo.py"
SCRIPT_DIR="$SOURCE_DIR/$($INFOPY --source-dir "$SOURCE_DIR" script_location)"

HELPER="$SCRIPT_DIR/travis-helper.py"
GITHUBPY="$SCRIPT_DIR/github_api.py"

DEPLOY_ASSET="libraries_$BUILDVARIANT.tar.gz"

function github_api()
{
    $GITHUBPY --api-token ${GITHUB_TOKEN} $@
}

TARGET_TAG=$(github_api list tag "$TRAVIS_REPO_SLUG" "^$TRAVIS_COMMIT$" | cut -d'|' -f 2)

if [[ -z $TARGET_TAG ]]; then
    echo " * Could not find tag, will not deploy"
    exit 0
fi

TARGET_RELEASE=$(github_api list release "$TRAVIS_REPO_SLUG" "^$TARGET_TAG$" | cut -d'|' -f 1)

if [[ -z $TARGET_RELEASE ]]; then
    github_api push release "$TRAVIS_REPO_SLUG" "$TARGET_TAG" "Autorelease" "Autorelease"
fi

echo " * Deploying $DEPLOY_ASSET to $TARGET_TAG"
github_api push asset "$TRAVIS_REPO_SLUG:$TARGET_TAG" "$DEPLOY_ASSET"