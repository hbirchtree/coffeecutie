#!/bin/bash

[ -z "$FIREBASE_ENDPOINT" ] && echo " * Cannot connect to Firebase: no endpoint" && exit 1
[ -z "$FIREBASE_TOKEN" ]    && echo " * Cannot submit to Firebase, no token"     && exit 1

function api_call()
{
    local method=$1
    local resource=$2.json
    local data=$3

    case "$method" in
    "GET")
        curl -s "$FIREBASE_ENDPOINT"/"$2"
    ;;
    "PUT")
        curl -s -X PUT -d "$data" "$FIREBASE_ENDPOINT"/"$resource"?auth="$FIREBASE_TOKEN"
    ;;
    "DELETE")
        curl -s -X DELETE "$FIREBASE_ENDPOINT"/"$resource"?auth="$FIREBASE_TOKEN"
    ;;
    esac
}

api_call $@
echo
