# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

# syntax = docker/dockerfile:1.4

ARG UBUNTU=noble

FROM ubuntu:${UBUNTU} as qt

ARG QT_VER
ENV container docker
ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=en_US.UTF-8
ENV LC_ALL=C.UTF-8
RUN apt-get -y update && apt-get -y install python3 python3-pip python3-venv git curl

COPY aqtinstall-requirements.txt /tmp/aqtinstall-requirements.txt
ARG TARGETPLATFORM
RUN <<EOF
    if [ "${TARGETPLATFORM}" = "linux/arm64" ]; then
        ARCH=gcc_arm64
        HOST=linux_arm64
    else
        ARCH=gcc_64
        HOST=linux
    fi
    python3 -m venv venv && venv/bin/pip3 install --requirement /tmp/aqtinstall-requirements.txt \
    && venv/bin/python3 -m aqt install-qt ${HOST} desktop ${QT_VER} --modules qtmultimedia qtquick3d qtshadertools qtquicktimeline qtwebengine qtwebchannel qtpositioning debug_info --archives qtbase icu qttools qtdeclarative qtmultimedia qtquick3d qtquicktimeline --outputdir /usr/local/Qt \
    && venv/bin/python3 -m aqt install-doc linux ${QT_VER} --modules qtmultimedia --archives qtqml qtquick qtmultimedia --outputdir /usr/local/Qt \
    && find /usr/local/Qt/Docs/Qt-${QT_VER} -type f -and -not -name '*.index' -delete \
    && curl -o iwyu-mapgen-qt.py https://raw.githubusercontent.com/include-what-you-use/include-what-you-use/clang_17/mapgen/iwyu-mapgen-qt.py \
    && python3 iwyu-mapgen-qt.py /usr/local/Qt/${QT_VER}/${ARCH}/include > "/usr/local/Qt/${QT_VER}/qt.imp"
EOF


FROM ubuntu:jammy as jammy


FROM ubuntu:noble as noble
RUN apt-get -y update && apt-get -y install \
    clang-format \
    clang-tidy \
    clazy \
    iwyu


FROM ${UBUNTU} as mediafx
LABEL maintainer=rectalogic
ARG QT_VER

ENV GALLIUM_DRIVER=softpipe
ENV LIBGL_ALWAYS_SOFTWARE=1
ENV DRI_NO_MSAA=1

ENV container docker
ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=en_US.UTF-8
ENV LC_ALL=C.UTF-8
RUN apt-get -y update  && apt-get -y install \
    bash \
    build-essential \
    curl \
    ffmpeg \
    fontconfig \
    fonts-liberation \
    git \
    gosu \
    libavcodec-dev \
    libavfilter-dev \
    libavformat-dev \
    libavutil-dev \
    libglvnd-dev \
    libglx-mesa0 \
    libnss3 \
    libpulse-dev \
    libvulkan-dev \
    libx11-xcb1 \
    libxcb-cursor0 \
    libxcb-glx0 \
    libxcb-icccm4 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-render-util0 \
    libxcb-shape0 \
    libxcb1 \
    libxcomposite1 \
    libxdamage1 \
    libxi6 \
    libxkbcommon-dev \
    libxkbcommon-x11-0 \
    libxrandr2 \
    libxrender1 \
    libxtst6 \
    mesa-vulkan-drivers \
    ninja-build \
    pkg-config \
    python3 \
    python3-pip \
    sudo \
    xvfb \
    xz-utils

RUN curl -L https://github.com/Kitware/CMake/releases/download/v3.29.2/cmake-3.29.2-linux-$(uname --machine).tar.gz | tar xfz - --strip-components=1 -C /usr/local

COPY --from=qt /usr/local/Qt /usr/local/Qt/

ARG TARGETPLATFORM
RUN <<EOF
if [ "${TARGETPLATFORM}" = "linux/arm64" ]; then
    QTDIR=/usr/local/Qt/${QT_VER}/gcc_arm64
else
    QTDIR=/usr/local/Qt/${QT_VER}/gcc_64
fi
mkdir -p /entrypoint
echo QTDIR=${QTDIR} >> /entrypoint/environment
echo PATH=${QTDIR}/bin:$PATH >> /entrypoint/environment
chmod +x /entrypoint/environment
EOF

ENV QT_VER=${QT_VER}

RUN ldconfig

RUN useradd -ms /bin/bash -d /mediafx/build/Linux -g root -G sudo mediafx
RUN echo "mediafx ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/mediafx && chmod 0440 /etc/sudoers.d/mediafx
VOLUME /mediafx
WORKDIR /mediafx/build/Linux

COPY Linux/entrypoint.sh Linux/xvfb.sh /entrypoint/

ENTRYPOINT ["/entrypoint/entrypoint.sh"]
CMD ["/mediafx/builders/mediafx-build.sh"]
