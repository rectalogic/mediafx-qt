#!/usr/bin/env bash
# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

CURRENT=${BASH_SOURCE%/*}
source "$CURRENT/../versions"
docker buildx build --build-arg QT_VER=${QT_VER:?} --platform linux/amd64 --memory-swap -1 --load --tag ghcr.io/rectalogic/mediafx:$(git branch --show-current) "$CURRENT"
