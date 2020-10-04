#!/bin/bash

set -e

WDIR="$(dirname "$(realpath "$0")")"

function die()
{
	echo $@
	exit 1
}

[ -z "$1" ] && die "-- Release type not specified"

RELEASE_NAME="$("$WDIR"/version.py "$1")"

git tag -a -m "Version $RELEASE_NAME" "$RELEASE_NAME"
git push --follow-tags
