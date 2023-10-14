#!/usr/bin/env bash

sudo mkdir -p /mediafx/build/linux && sudo chown mediafx /mediafx/build/ /mediafx/build/linux
exec "$@"
