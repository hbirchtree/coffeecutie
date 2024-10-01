#!/bin/bash

ARTIFACT_DIR=${1:-artifacts/}

TEST_VARIANTS=$(ls $ARTIFACT_DIR)

for TEST_VARIANT in $TEST_VARIANTS; do
    echo $ARTIFACT_DIR/$TEST_VARIANT
    for IMG in $(find $ARTIFACT_DIR/$TEST_VARIANT/ -name *.jpg); do
        echo " - Appending $IMG"
    done
    for LOG in $(find $ARTIFACT_DIR/$TEST_VARIANT/ -name *.log); do
        echo " - Appending $LOG"
    done
done
