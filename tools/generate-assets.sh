#!/usr/bin/env bash
# Copyright (C) 2024 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later


BASE=${BASH_SOURCE%/*}
usage="$0 <sourcevideodir>"

SOURCEDIR=${1:-}

# Create a colored "testsrc" video/image asset, with no audio
for ASSET in 'mp4~blue~320x180~30~3' 'mp4~green~320x180~15~3' 'mp4~red~320x180~15~8' 'mp4~red~640x360~30~4' 'mp4~yellow~320x180~15~3' 'png~red~160x120'; do
    ${BASE}/create-asset.sh "${BASE}/../tests/fixtures/assets" "${ASSET}"
done

# Download source videos:
# bbb_sunflower_1080p_30fps_normal.mp4
#    https://download.blender.org/demo/movies/BBB/bbb_sunflower_1080p_30fps_normal.mp4.zip
# ed_hd.avi
#    https://archive.org/download/ElephantsDream/ed_hd.avi
# cosmos_laundromat_2k.mp4
#    https://pillar.blender.cloud/p/cosmos-laundromat/58b00b1b49932b2881e00757

# Blender videos used under CC By 4.0 license
# https://creativecommons.org/licenses/by/4.0/
# Trimmed, transcoded, resized, cropped and overlayed with timecode.
# https://studio.blender.org/films/

if [ -f "${SOURCEDIR}/ed_hd.avi" ]; then
    ${BASE}/encode-asset.sh "${SOURCEDIR}/ed_hd.avi" "${BASE}/../tests/fixtures/assets/ednotsafe-320x180-15fps-1.53s.mp4" scale=320x180 15 2:48.458 2:50.041
    ${BASE}/encode-asset.sh "${SOURCEDIR}/ed_hd.avi" "${BASE}/../tests/fixtures/assets/edquestions-320x180-15fps-2.4s.mp4" scale=320x180 15 7:00.791 7:03.249
    ${BASE}/encode-asset.sh "${SOURCEDIR}/ed_hd.avi" "${BASE}/../tests/fixtures/assets/edjustforyou-320x180-15fps-5.2s.mp4" scale=320x180 15 8:16.416 8:21.666
fi

if [ -f "${SOURCEDIR}/cosmos_laundromat_2k.mp4" ]; then
    ${BASE}/encode-asset.sh "${SOURCEDIR}/cosmos_laundromat_2k.mp4" "${BASE}/../tests/fixtures/assets/cosmoswolf-320x180-15fps-4.1s.mp4" 'crop=in_h*16/9,scale=320x180 15 3:22.917 3:27'
    ${BASE}/encode-asset.sh "${SOURCEDIR}/cosmos_laundromat_2k.mp4" "${BASE}/../tests/fixtures/assets/cosmosregret-320x134par-15fps-5.3s.mp4" scale=320x180:force_original_aspect_ratio=decrease 15 4:29.417 4:34.708
fi

if [ -f "${SOURCEDIR}/bbb_sunflower_1080p_30fps_normal.mp4" ]; then
    ${BASE}/encode-asset.sh "${SOURCEDIR}/bbb_sunflower_1080p_30fps_normal.mp4" "${BASE}/../tests/fixtures/assets/bbbjumprope-320x180-15fps-5.5s.mp4" scale=320x180 15 4:34.767 4:40.367
fi