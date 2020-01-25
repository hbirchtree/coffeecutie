#!/bin/bash

[ -z "$FCM_TOKEN" ]    && echo " * Cannot submit to FCM: no token"     && exit 1

curl -s -X POST \
    -d "{
    \"to\": \"/topics/${MSG_TOPIC:-test}\",
    \"priority\": \"high\",
    \"notification\": {
        \"title\": \"${MSG_TITLE:-Generic Message}\",
        \"body\":  \"${MSG_BODY:-Generic Message Body}\",
        \"tag\": \"${MSG_TAG:-default}\",
        \"color\": \"${MSG_COLOR:-#4286f4}\"
    },
    \"data\": {
        \"description\": \"${MSG_DESCRIPTION}\",
        \"long-description\": \"${MSG_LDESCRIPTION}\",
        \"big-icon\": \"${MSG_ICON:-https://coffee.birchtrees.me/favicon.png}\"
    }
}" \
    "${FCM_ENDPOINT:-https://fcm.googleapis.com/fcm/send}" \
    --header "Content-Type: application/json" \
    --header "Authorization: key=$FCM_TOKEN"
echo
