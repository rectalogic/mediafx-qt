#!/usr/bin/env bash

ffmpeg -f lavfi -i testsrc=duration=${DURATION:-12}:size=${SIZE:-640x360}:rate=${FRAMERATE:-30}:decimals=2,drawbox=color=${COLOR:-red}:t=ih/16,drawtext=text=f%{frame_num}:x=w/10:y=x/dar:shadowx=3:shadowy=3:fontsize=h/3.75:fontcolor=yellow@0.9 -f mov -vcodec rawvideo -pix_fmt uyvy422 -framerate ${FRAMERATE:-30} -vtag yuvs -y ${1:?specify output mov filename}