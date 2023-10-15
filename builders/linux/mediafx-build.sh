#!/usr/bin/env bash

cd /webvfx/build/linux
cmake --install-prefix /usr/local/Qt/${QT_VER}/gcc_64 ../.. && cmake --build . && sudo cmake --install .
