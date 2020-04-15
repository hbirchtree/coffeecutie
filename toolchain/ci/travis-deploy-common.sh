#!/bin/bash

source $(dirname "$0")/travis-common.sh

[ -z "${BUILDVARIANT}" ] && die "No BUILDVARIANT specified"

#DEPLOY_ASSET="libraries_$BUILDVARIANT.tar.gz"
#DEPLOY_BINS="binaries_$BUILDVARIANT.tar.gz"

function github_api()
{
    if [ -z "${GITHUB_TOKEN}" ]; then
        echo "DRY: ${GITHUBPY}" $@
        return
    fi

    case ${TRAVIS_OS_NAME} in
#    "osx")
#        $GITHUBPY --api-token ${GITHUB_TOKEN} $@
#    ;;
    *)
        python3 "${GITHUBPY}" --api-token "${GITHUB_TOKEN}" $@
    ;;
    esac
}

function get_deploy_slug()
{
    TARGET_TAG=$(github_api list tag "$TRAVIS_REPO_SLUG" "^$TRAVIS_COMMIT$" | cut -d'|' -f 2)

    [ -z "${TARGET_TAG}" ] && die " * Could not find tag, will not deploy"

    TARGET_RELEASE=$(github_api list release "$TRAVIS_REPO_SLUG" "^$TARGET_TAG$" | cut -d'|' -f 1)

    [ -z "${TARGET_RELEASE}" ] && github_api push release "$TRAVIS_REPO_SLUG" "$TARGET_TAG" "Autorelease" "Autorelease"

    echo "${TRAVIS_REPO_SLUG}:${TARGET_TAG}"
}

function collect_coverage()
{
    if [[ ${BUILDVARIANT} = "coverage" ]]; then
        SEARCH_DIR=${SOURCE_DIR}/multi_build/coverage
        mv "${SEARCH_DIR}"/coverage.info.cleaned "${SEARCH_DIR}"/coverage.info
        bash <(curl -s https://codecov.io/bash) -X gcov -s "${SEARCH_DIR}" || echo "Codecov did not collect coverage reports"
    fi
}

function deploy_asset()
{
    local TARGET_SLUG="$2"
    local SOURCE_ASSET="$1"

    notify "Deploying $SOURCE_ASSET to $TARGET_SLUG"
    github_api push asset "$TARGET_SLUG" "$SOURCE_ASSET"
}

#github_api push asset "$TRAVIS_REPO_SLUG:$TARGET_TAG" "$DEPLOY_ASSET"
#github_api push asset "$TRAVIS_REPO_SLUG:$TARGET_TAG" "$DEPLOY_BINS"
