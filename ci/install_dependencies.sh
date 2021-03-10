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
        erlang \
        gcc-c++ \
        boost-devel \
        pugixml-devel \
        python-devel \
    
    yum clean all

    # 2) build uHAL
    cd /tmp/
    git clone --branch ${UHAL_VERSION} https://github.com/ipbus/ipbus-software.git
    cd ipbus-software/
    # 2b) patch uHAL to use _GLIBCXX_USE_CXX11_ABI macro
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/grammars/include/uhal/grammars/*.hpp
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/grammars/src/common/*.cpp
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/log/include/uhal/log/*.hpp
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/log/include/uhal/log/*.hxx
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/log/src/common/*.cpp
    # is it necessary to patch the tests? unsure, but just to be on the safe side..
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/tests/include/uhal/tests/*.hpp
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/tests/include/uhal/tests/*.hxx
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/tests/src/common/*.cxx
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/tests/src/common/*.cpp
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/uhal/include/_static/*.hpp
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/uhal/include/uhal/TemplateDefinitions/*.hxx
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/uhal/include/uhal/detail/*.hpp
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/uhal/include/uhal/utilities/*.hpp
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/uhal/include/uhal/*.hpp
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/uhal/src/common/detail/*.cpp
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/uhal/src/common/utilities/*.cpp
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' uhal/uhal/src/common/*.cpp
    # changed BUILD_UHAL_TESTS: 1 -> 0 BUILD_UHAL_PYCOHAL: 1 -> 0
    make -j$(nproc) Set=uhal BUILD_PUGIXML=0 BUILD_UHAL_TESTS=0 BUILD_UHAL_PYCOHAL=0
    make Set=uhal BUILD_PUGIXML=0 BUILD_UHAL_TESTS=0 BUILD_UHAL_PYCOHAL=0 install
    cd ..
    rm -rf ipbus-software/
    export CACTUS_ROOT=/opt/cactus

    # 3) building UIOuHAL
    git clone --branch ${UIOUHAL_VERSION} https://github.com/dgastler/UIOuHAL.git
    cd UIOuHAL/
    # 3b) patch UIOuHAL to use _GLIBCXX_USE_CXX11_ABI macro
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' include/*.hpp
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' src/*.cpp
    # build
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
    # 5b) patch BUTool itself to use _GLIBCXX_USE_CXX11_ABI macro
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' BUTool/include/BUException/*hh
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' BUTool/include/BUTool/helpers/StatusDisplay/*hh
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' BUTool/include/BUTool/helpers/*hh
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' BUTool/include/BUTool/*hh
    # is this necessary - tclap *h??
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' BUTool/include/tclap/*h
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' BUTool/src/BUException/*cc
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' BUTool/src/helpers/StatusDisplay/*cc
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' BUTool/src/helpers/*cc
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' BUTool/src/tool/*cc
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' BUTool/src/tool/*cxx
    # 5c) patch BUTool-IPBUS-Helpers to use _GLIBCXX_USE_CXX11_ABI macro
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/BUTool-IPBUS-Helpers/include/IPBusIO/*.hh
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/BUTool-IPBUS-Helpers/include/IPBusRegHelper/*.hh
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/BUTool-IPBUS-Helpers/include/IPBusStatus/*.hh
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/BUTool-IPBUS-Helpers/src/IPBusIO/*.cpp
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/BUTool-IPBUS-Helpers/src/IPBusStatus/*.cpp
    # 5d) patch GenericIPBus_plugin to use _GLIBCXX_USE_CXX11_ABI macro
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/GenericIPBus_plugin/include/GenericIPBus/*.hh
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/GenericIPBus_plugin/include/GenericIPBus_device/*.hh
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/GenericIPBus_plugin/src/GenericIPBus/*.cc
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/GenericIPBus_plugin/src/GenericIPBus_device/*.cc
    # 5e) patch ApolloSM_plugin to use _GLIBCXX_USE_CXX11_ABI macro
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/ApolloSM_plugin/include/ApolloSM/*.hh
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/ApolloSM_plugin/include/ApolloSM_device/*.hh
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/ApolloSM_plugin/include/standalone/*.hh
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/ApolloSM_plugin/src/ApolloSM/*.cc
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/ApolloSM_plugin/src/ApolloSM_device/*.cc
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/ApolloSM_plugin/src/standalone/*.cc
    sed -i '1 i\#define _GLIBCXX_USE_CXX11_ABI 0' plugins/ApolloSM_plugin/src/standalone/*.cxx
    # 5f) patch ApolloSM_plugin Makefile to deal with strange format-truncation error (append -Wno-format-trunctation to CXX_FLAGS variable)
    sed -i "s|-Wno-ignored-qualifiers|-Wno-ignored-qualifiers -Wno-format-truncation|g" plugins/ApolloSM_plugin/Makefile
    # time to build
    make local -j$(nproc) RUNTIME_LDPATH=/opt/BUTool COMPILETIME_ROOT=--sysroot=/
    make install RUNTIME_LDPATH=/opt/BUTool COMPILETIME_ROOT=--sysroot=/ INSTALL_PATH=/opt/BUTool
    cd ..
    rm -rf ApolloTool
fi