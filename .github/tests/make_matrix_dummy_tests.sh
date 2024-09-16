#!/bin/bash

GL_CONFIGS=$(jq -c .graphics.gl[] dummy_plug.json)
ES_CONFIGS=$(jq -c .graphics.gles[] dummy_plug.json)

for CFG in $GL_CONFIGS; do
    MAJOR=$(jq .major <<< "$CFG")
    MINOR=$(jq .minor <<< "$CFG")
    jq ".graphics.gl |= [$CFG] | .graphics.gles = []" dummy_plug.json > dummy_plug_core_$MAJOR$MINOR.json
done

for CFG in $ES_CONFIGS; do
    MAJOR=$(jq .major <<< "$CFG")
    MINOR=$(jq .minor <<< "$CFG")
    jq ".graphics.gles |= [$CFG] | .graphics.gl = []" dummy_plug.json > dummy_plug_es_$MAJOR$MINOR.json
done
