# syntax = docker/dockerfile:1.2
# linux arm64 not supported by Qt https://doc.qt.io/qt-6/supported-platforms.html
FROM --platform=linux/amd64 ubuntu:jammy
LABEL maintainer=rectalogic

ENV container docker
ENV DEBIAN_FRONTEND=noninteractive
ENV LANG=en_US.UTF-8
ENV LC_ALL=C.UTF-8
RUN apt-get -y update \
    && apt-get -y install sudo build-essential cmake ninja-build git clang clang-13 clang-format iwyu \
    libglx-mesa0 libglvnd-dev libxkbcommon-dev libxkbcommon-x11-0 libpulse-dev libxcb1 libx11-xcb1 libxcb-glx0 libxcb-cursor0 \
    libxcb-icccm4 libxcb-image0 libxcb-render-util0 libxcb-keysyms1 \
    python3 python3-pip \
    curl xz-utils xvfb pkg-config \
    fontconfig fonts-liberation

ARG QT_VER=6.6.0

RUN pip3 install aqtinstall \
    && python3 -m aqt install-qt linux desktop ${QT_VER} -m qtmultimedia qtquick3d qtshadertools qtquicktimeline --outputdir /usr/local/Qt

ENV PATH /usr/local/Qt/${QT_VER}/gcc_64/bin:$PATH
ENV QT_VER=${QT_VER}

RUN ldconfig

COPY entrypoint.sh /entrypoint.sh

RUN useradd -ms /bin/bash -d /mediafx/build/linux -g root -G sudo mediafx
RUN echo "mediafx ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/mediafx && chmod 0440 /etc/sudoers.d/mediafx
VOLUME /mediafx
WORKDIR /mediafx/build/linux
USER mediafx

ENTRYPOINT ["/usr/bin/xvfb-run", "--error-file", "/dev/stderr", "/entrypoint.sh"]
