#!/usr/bin/env bash

pushd $(dirname $0)

if [ ! -f key.pem ]; then
    openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 30 -nodes
fi

popd

$(dirname $0)/wasm-http-server.py $@

