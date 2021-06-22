#!/bin/bash

set -e
set -x 

UHAL_VERSION=2.7.7
UIOUHAL_VERSION="develop"
APOLLOTOOL_VERSION="uhal_versioning_change"

if [ "$1" != "app" ]; then

    # 1) Install dependencies of uHAL and UIOuHAL
    yum -y install git \
        readline-devel \
        zlib-devel \
        rpm-build \
        git-core \
        gcc-c++ \
        boost-devel \
        pugixml-devel \
        python-devel \
    
    yum clean all

    # 2) build uHAL
    cp ci/*.repo /etc/yum.repos.d/
    yum -y install --exclude *debuginfo \
      cactuscore-uhal-*-${UHAL_VERSION}
    yum clean all
    export CACTUS_ROOT=/opt/cactus/

    # 3) build EMP software & toolbox
    cp emp-herd/ci/*.repo /etc/yum.repos.d/
    source emp-herd/ci/install_dependencies.sh

    # 4) building UIOuHAL
    git clone --branch ${UIOUHAL_VERSION} https://github.com/BU-Tools/UIOuHAL.git
    # 4b) patch UIOuHAL to use _GLIBCXX_USE_CXX11_ABI macro
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' $(find ./UIOuHAL -name '*.hpp') && \
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' $(find ./UIOuHAL -name '*.cpp') && \
    # build
    cd UIOuHAL/
    make
    mkdir -p /opt/UIOuHAL
    cp -r lib /opt/UIOuHAL
    cp -r include /opt/UIOuHAL
    cd ..
    rm -rf UIOuHAL

    # 5) install dependencies of BUTool
    yum -y install boost-chrono \
        boost-regex \
        boost-thread \
        boost-filesystem \
        boost-system \
        boost-devel \
        boost-python \
        boost-program-options \

    yum clean all

    # 6) build BUTool from ApolloTool meta repository
    git clone --branch ${APOLLOTOOL_VERSION} https://github.com/ammitra/ApolloTool.git
    cd ApolloTool
    make init
    # 6b) patch BUTool and plugins to use _GLIBCXX_USECXX11_ABI macro
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' $(find ./ -name '*.hh') && \
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' $(find ./ -name '*.h') && \
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' $(find ./ -name '*.cxx') && \
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' $(find ./ -name '*.cc') && \
    # 6c) patch ApolloSM_plugin Makefile to deal with strange format-truncation error (append -Wno-format-trunctation to CXX_FLAGS variable)
    sed -i "s|-Wno-ignored-qualifiers|-Wno-ignored-qualifiers -Wno-format-truncation|g" plugins/ApolloSM_plugin/Makefile
    # time to build
    make local -j$(nproc) RUNTIME_LDPATH=/opt/BUTool COMPILETIME_ROOT=--sysroot=/
    make install RUNTIME_LDPATH=/opt/BUTool COMPILETIME_ROOT=--sysroot=/ INSTALL_PATH=/opt/BUTool
    cd ..
    rm -rf ApolloTool

fi
