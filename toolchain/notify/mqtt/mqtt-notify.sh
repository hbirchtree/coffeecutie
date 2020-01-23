#!/bin/bash

[[ -z "$MQTT_USERNAME" ]] && exit 0

SRC_DIR=$(dirname "$0")

MQTT_PAYLOAD="{\"title\":\"$MQTT_TITLE\", \"body\":\"$MQTT_BODY\",\"icon\":\"$MQTT_ICON\",\"timestamp\":\"$(date +%s)\",\"contentUrl\":\"$MQTT_URL\"}"

"${SRC_DIR}"/send-msg.py \
    "$MQTT_TOPIC" "$MQTT_PAYLOAD" \
    --username "$MQTT_USERNAME" --password "$MQTT_PASSWORD"

