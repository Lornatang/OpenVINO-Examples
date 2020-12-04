#!/bin/bash

# Copyright (c) 2020 Dakewe Biotech Corporation. All rights reserved.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

# Get user instructions
params=$1

# If the command is correct, the installation will be skipped.
yes_or_no() {
    if [ "$params" == "-y" ]; then
        return 0
    fi

    while true; do
        # shellcheck disable=SC2162
        read -p "Add third-party Nux Dextop repository and install FFmpeg package (y) / Skip this step (N)" yn
        case $yn in
            [Yy]*) return 0 ;;
            [Nn]*) return 1 ;;
        esac
    done
}

# install dependencies
if [ -f /etc/lsb-release ]; then
    # Ubuntu 18.04/20.04
    host_cpu=$(uname -m)
    if [ "$host_cpu" = "x86_64" ]; then
        x86_64_specific_packages="gcc-multilib g++-multilib"
    else
        x86_64_specific_packages=""
    fi

    sudo -E apt update
    sudo -E apt-get install -y \
            build-essential \
            curl \
            wget \
            libssl-dev \
            ca-certificates \
            git \
            libboost-regex-dev \
            $x86_64_specific_packages \
            libgtk2.0-dev \
            pkg-config \
            unzip \
            automake \
            libtool \
            autoconf \
            shellcheck \
            python \
            libcairo2-dev \
            libpango1.0-dev \
            libglib2.0-dev \
            libgtk2.0-dev \
            libswscale-dev \
            libavcodec-dev \
            libavformat-dev \
            libgstreamer1.0-0 \
            gstreamer1.0-plugins-base \
            libusb-1.0-0-dev \
            libopenblas-dev
    if apt-cache search --names-only '^libpng12-dev'| grep -q libpng12; then
        sudo -E apt-get install -y libpng12-dev
    else
        sudo -E apt-get install -y libpng-dev
    fi
elif [ -f /etc/os-release ] && grep -q "raspbian" /etc/os-release; then
    # Raspbian
    sudo -E apt update
    sudo -E apt-get install -y \
            build-essential \
            curl \
            wget \
            libssl-dev \
            ca-certificates \
            git \
            libboost-regex-dev \
            libgtk2.0-dev \
            pkg-config \
            unzip \
            automake \
            libtool \
            autoconf \
            libcairo2-dev \
            libpango1.0-dev \
            libglib2.0-dev \
            libgtk2.0-dev \
            libswscale-dev \
            libavcodec-dev \
            libavformat-dev \
            libgstreamer1.0-0 \
            gstreamer1.0-plugins-base \
            libusb-1.0-0-dev \
            libopenblas-dev
    if apt-cache search --names-only '^libpng12-dev'| grep -q libpng12; then
        sudo -E apt-get install -y libpng12-dev
    else
        sudo -E apt-get install -y libpng-dev
    fi
else
    echo "Unknown OS, please install build dependencies manually"
fi

# cmake 3.19 or higher is required to build OpenVINO
current_cmake_version=$(cmake --version | sed -ne 's/[^0-9]*\(\([0-9]\.\)\{0,4\}[0-9][^.]\).*/\1/p')
required_cmake_ver=3.19
if [ ! "$(printf '%s\n' "$required_cmake_ver" "$current_cmake_version" | sort -V | head -n1)" = "$required_cmake_ver" ]; then
    wget "https://github.com/Kitware/CMake/releases/download/v3.19.0/cmake-3.19.0.tar.gz"
    tar xf cmake-3.19.0.tar.gz
    (cd cmake-3.19.0 && ./bootstrap --parallel="$(nproc --all)" && make --jobs="$(nproc --all)" && sudo make install)
    rm -rf cmake-3.19.0 cmake-3.19.0.tar.gz
fi