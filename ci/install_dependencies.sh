#!/bin/bash

set -e
set -x 

UHAL_VERSION="v2.7.4"
UIOUHAL_VERSION="feature-zynqmp_issues"
APOLLOTOOL_VERSION="feature-USP_addrs"

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
    cd /tmp/
    git clone --branch ${UHAL_VERSION} https://github.com/ipbus/ipbus-software.git
    # patch uHAL libs to use _GLIBCXX_USE_CXX11_ABI macro
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' $(find ./ipbus-software -name '*.hpp') && \
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' $(find ./ipbus-software -name '*.hxx') && \
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' $(find ./ipbus-software -name '*.cpp') && \
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' $(find ./ipbus-software -name '*.cxx') && \
    # changed BUILD_UHAL_TESTS: 1 -> 0 BUILD_UHAL_PYCOHAL: 1 -> 0
    cd ipbus-software/
    make -j$(nproc) Set=uhal BUILD_PUGIXML=0 BUILD_UHAL_TESTS=0 BUILD_UHAL_PYCOHAL=0
    make Set=uhal BUILD_PUGIXML=0 BUILD_UHAL_TESTS=0 BUILD_UHAL_PYCOHAL=0 install
    cd ..
    rm -rf ipbus-software/
    export CACTUS_ROOT=/opt/cactus

    # 3) building UIOuHAL
    git clone --branch ${UIOUHAL_VERSION} https://github.com/dgastler/UIOuHAL.git
    # 3b) patch UIOuHAL to use _GLIBCXX_USE_CXX11_ABI macro
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

    # 4) install dependencies of BUTool
    yum -y install boost-chrono \
        boost-regex \
        boost-thread \
        boost-filesystem \
        boost-system \
        boost-devel \
        boost-python \
        boost-program-options \

    yum clean all

    # 5) build BUTool from ApolloTool meta repository
    git clone --branch ${APOLLOTOOL_VERSION} https://github.com/apollo-lhc/ApolloTool.git
    cd ApolloTool
    make init
    rm -rf plugins/IPMC_plugin
    # 5b) patch BUTool and plugins to use _GLIBCXX_USECXX11_ABI macro
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' $(find ./ -name '*.hh') && \
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' $(find ./ -name '*.h') && \
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' $(find ./ -name '*.cxx') && \
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' $(find ./ -name '*.cc') && \
    # 5c) patch ApolloSM_plugin Makefile to deal with strange format-truncation error (append -Wno-format-trunctation to CXX_FLAGS variable)
    sed -i "s|-Wno-ignored-qualifiers|-Wno-ignored-qualifiers -Wno-format-truncation|g" plugins/ApolloSM_plugin/Makefile
    # time to build
    make local -j$(nproc) RUNTIME_LDPATH=/opt/BUTool COMPILETIME_ROOT=--sysroot=/
    make install RUNTIME_LDPATH=/opt/BUTool COMPILETIME_ROOT=--sysroot=/ INSTALL_PATH=/opt/BUTool
    cd ..
    rm -rf ApolloTool

    # 5) remove compiler & unecessary libs - reduce image size
    rm -rf /usr/local/libexec
    rm -rf /usr/libexec/gcc
    rm -rf /usr/lib/python3.6
    rm -rf /usr/lib/python2.7
    rm -rf /usr/local/lib/gcc
    rm -rf /usr/local/lib/cmake
    rm -rf /lib/gcc
    #rm -rf /lib/perl5
    #rm -rf /lib/rpm

fi
