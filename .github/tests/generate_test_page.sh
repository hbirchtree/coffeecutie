#!/bin/bash

ARTIFACT_DIR=${1:-artifacts/}
OUT_FILE=$ARTIFACT_DIR/generated/results.html
mkdir $ARTIFACT_DIR/generated

$(dirname ${0})/test_page.py ${ARTIFACT_DIR} ${OUT_FILE}
