#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
#
# This file is part of mediaFX.
#
# mediaFX is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software Foundation,
# either version 3 of the License, or (at your option) any later version.
#
# mediaFX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with mediaFX.
# If not, see <https://www.gnu.org/licenses/>.

MOUNT="$(dirname $(dirname "$(cd "$(dirname "${BASH_SOURCE[0]}")"; pwd)"))"

# opengl or vulkan
TARGET=${TARGET:-opengl}
docker run ${DOCKER_OPTS} -e QSG_RHI_BACKEND=${TARGET} --rm --init \
    --mount="type=bind,src=${MOUNT},dst=/mediafx,consistency=cached" ghcr.io/rectalogic/mediafx:$(git branch --show-current) "$@"
