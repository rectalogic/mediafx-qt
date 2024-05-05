#!/usr/bin/env bash

# Find packages required for missing shared libraries
# apt install apt-file && apt-file update

ldd "${@}" | grep "not found" | cut  -f 2 | cut -d ' ' -f 1 \
    | apt-file search --from-file - | egrep ' /lib/x86_64-linux-gnu/lib| /usr/lib/x86_64-linux-gnu/lib' | cut -d ':' -f 1 | uniq
