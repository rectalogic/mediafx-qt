# Copyright (C) 2023 Andrew Wason
# SPDX-License-Identifier: GPL-3.0-or-later

# syntax = docker/dockerfile:1.2
# linux arm64 not supported by Qt https://doc.qt.io/qt-6/supported-platforms.html
FROM --platform=linux/amd64 ubuntu:jammy as build

ARG QT_VER
ENV container docker
ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=en_US.UTF-8
ENV LC_ALL=C.UTF-8
RUN apt-get -y update \
    && apt-get -y install python3 python3-pip

COPY aqtinstall-requirements.txt /tmp/aqtinstall-requirements.txt
RUN pip3 install --requirement /tmp/aqtinstall-requirements.txt \
    && python3 -m aqt install-qt linux desktop ${QT_VER} --modules qtmultimedia qtquick3d qtshadertools qtquicktimeline debug_info --archives qtbase icu qttools qtdeclarative qtmultimedia qtquick3d qtquicktimeline --outputdir /usr/local/Qt \
    && python3 -m aqt install-doc linux ${QT_VER} --modules qtmultimedia --archives qtquick qtmultimedia --outputdir /usr/local/Qt \
    && find /usr/local/Qt/Docs/Qt-${QT_VER} -type f -and -not -name '*.index' -delete


FROM --platform=linux/amd64 ubuntu:jammy as mediafx
LABEL maintainer=rectalogic
ARG QT_VER

ENV container docker
ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=en_US.UTF-8
ENV LC_ALL=C.UTF-8
# iwyu & clazy needs clang-13
RUN apt-get -y update \
    && apt-get -y install sudo gosu build-essential cmake ninja-build git \
    clang-15 clang-format-15 clang-tidy-15 clang-13 iwyu clazy \
    libglx-mesa0 libglvnd-dev libxkbcommon-dev libxkbcommon-x11-0 libpulse-dev libxcb1 libx11-xcb1 libxcb-glx0 libxcb-cursor0 \
    libxcb-icccm4 libxcb-image0 libxcb-render-util0 libxcb-keysyms1 libicu70 \
    libvulkan-dev mesa-vulkan-drivers \
    python3 python3-pip \
    curl xz-utils xvfb pkg-config libavutil-dev libswresample-dev libffms2-dev ffmpeg \
    fontconfig fonts-liberation bc

COPY --from=build /usr/local/Qt /usr/local/Qt/
ENV QTDIR /usr/local/Qt/${QT_VER}/gcc_64
ENV PATH /usr/local/Qt/${QT_VER}/gcc_64/bin:$PATH
ENV QT_VER=${QT_VER}

RUN ldconfig

RUN useradd -ms /bin/bash -d /mediafx/build/Linux -g root -G sudo mediafx
RUN echo "mediafx ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/mediafx && chmod 0440 /etc/sudoers.d/mediafx
VOLUME /mediafx
WORKDIR /mediafx/build/Linux

COPY Linux/entrypoint.sh Linux/xvfb.sh /entrypoint/

ENTRYPOINT ["/entrypoint/entrypoint.sh"]
CMD ["/mediafx/builders/mediafx-build.sh"]