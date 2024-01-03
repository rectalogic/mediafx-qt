#!/usr/bin/env bash

OLD_UID=$(id -u mediafx)
NEW_UID=$(stat -c "%u" /mediafx/CMakeLists.txt)
if [ ! -z "$NEW_UID" -a "$NEW_UID" != "0" -a "$OLD_UID" != "$NEW_UID" ]; then
    usermod -u "$NEW_UID" -o mediafx
fi

mkdir -p /mediafx/build/Linux && chown -R mediafx /mediafx/build/Linux
exec gosu mediafx /entrypoint/xvfb.sh "$@"