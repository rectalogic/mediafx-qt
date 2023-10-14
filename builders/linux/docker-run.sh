#!/usr/bin/env bash

MOUNT="$(dirname $(dirname "$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)"))"

docker run ${DOCKER_OPTS} --rm --init \
    --mount="type=bind,src=${MOUNT},dst=/mediafx,consistency=cached" mediafx "$@"
