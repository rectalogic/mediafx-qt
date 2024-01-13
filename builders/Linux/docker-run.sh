#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

MOUNT="$(dirname $(dirname "$(cd "${BASH_SOURCE%/*}"; pwd)"))"

# opengl or vulkan
TARGET=${TARGET:-opengl}
docker run ${DOCKER_OPTS} -e QSG_RHI_BACKEND=${TARGET} --platform linux/amd64 --rm --init \
    --mount="type=bind,src=${MOUNT},dst=/mediafx,consistency=cached" ghcr.io/rectalogic/mediafx:$(git branch --show-current) "$@"
