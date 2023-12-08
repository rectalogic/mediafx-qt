#!/usr/bin/env bash

sudo mkdir -p /mediafx/build/Linux && sudo chown mediafx /mediafx/build/ /mediafx/build/Linux
exec "$@"
