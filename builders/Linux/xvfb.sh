#!/usr/bin/env bash

exec /usr/bin/xvfb-run --server-args="-screen 0 640x480x24 -nolisten unix" --error-file=/tmp/xvfb.err "$@"
[ -f /tmp/xvfb.err ] && cat /tmp/xvfb.err