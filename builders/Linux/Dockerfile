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

# syntax = docker/dockerfile:1.2
# linux arm64 not supported by Qt https://doc.qt.io/qt-6/supported-platforms.html
FROM --platform=linux/amd64 ubuntu:jammy as base
LABEL maintainer=rectalogic

ENV container docker
ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=en_US.UTF-8
ENV LC_ALL=C.UTF-8
RUN apt-get -y update \
    && apt-get -y install sudo build-essential cmake ninja-build git clang clang-13 clang-format iwyu \
    libglx-mesa0 libglvnd-dev libxkbcommon-dev libxkbcommon-x11-0 libpulse-dev libxcb1 libx11-xcb1 libxcb-glx0 libxcb-cursor0 \
    libxcb-icccm4 libxcb-image0 libxcb-render-util0 libxcb-keysyms1 libicu70 \
    python3 python3-pip \
    curl xz-utils xvfb pkg-config ffmpeg \
    fontconfig fonts-liberation

ARG QT_VER

RUN pip3 install aqtinstall \
    && python3 -m aqt install-qt linux desktop ${QT_VER} -m qtmultimedia qtquick3d qtshadertools qtquicktimeline debug_info --archives icu qtbase qtdeclarative qtmultimedia qtquick3d qtshadertools qtquicktimeline qttools --outputdir /usr/local/Qt

ENV PATH /usr/local/Qt/${QT_VER}/gcc_64/bin:$PATH
ENV QT_VER=${QT_VER}

RUN ldconfig

COPY entrypoint.sh /entrypoint.sh

RUN useradd -ms /bin/bash -d /mediafx/build/Linux -g root -G sudo mediafx
RUN echo "mediafx ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/mediafx && chmod 0440 /etc/sudoers.d/mediafx
VOLUME /mediafx
WORKDIR /mediafx/build/Linux

ENTRYPOINT ["/usr/bin/xvfb-run", "--server-args", "-screen 0 640x480x24", "--error-file", "/dev/stderr", "/entrypoint.sh"]


FROM base as mediafx
USER mediafx


FROM base as mediafx-vulkan
ENV QSG_RHI_BACKEND=vulkan
RUN apt-get -y install libvulkan-dev mesa-vulkan-drivers
USER mediafx