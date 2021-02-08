#!/bin/bash

# install packages
yum -y install git readline-devel zlib-devel

# install uHAL packages: https://ipbus.web.cern.ch/doc/user/html/software/install/compile.html
yum -y install rpm-build git-core erlang gcc-c++ boost-devel pugixml-devel python-devel

# build ipbus-software
cd /tmp/
git clone --branch v2.7.4 https://github.com/ipbus/ipbus-software.git
cd ipbus-software/
make -j32 Set=uhal BUILD_PUGIXML=0 BUILD_UHAL_TESTS=1 BUILD_UHAL_PYCOHAL=1
make Set=uhal BUILD_PUGIXML=0 BUILD_UHAL_TESTS=1 BUILD_UHAL_PYCOHAL=1 install
cd ..
rm -rf ipbus-software/

export CACTUS_ROOT=/opt/cactus

# build UIOuHAL
git clone --branch feature-zynqmp_issues https://github.com/dgastler/UIOuHAL.git
cd UIOuHAL
make
mkdir -p /opt/UIOuHAL
cp -r lib /opt/UIOuHAL
cp -r include /opt/UIOuHAL
cd ..
rm -rf UIOuHAL

# build BUTool from ApolloTool meta repo
git clone --branch feature-USP_addrs https://github.com/apollo-lhc/ApolloTool.git
cd ApolloTool
make init
# do not need IPMC plugin for ApolloHerd
rm -rf plugins/IPMC_plugin
# there is a format-truncation error -> need to add -Wno-format-truncation to CXX_FLAGS in ApolloSM_plugin Makefile somehow
# do that here, somehow
make local -j32 RUNTIME_LDPATH=/opt/BUTool COMPILETIME_ROOT=--sysroot=/
make install RUNTIME_LDPATH=/opt/BUTool COMPILETIME_ROOT=--sysroot=/ INSTALL_PATH=/opt/BUTool
cd ..
rm -rf ApolloTool

# build and install HERD library and control app - jsoncpp-devel needed for latest HERD w/ HTTP&JSON
yum install -y gcc-c++ make cmake3 boost-devel log4cplus-devel yaml-cpp-devel jsoncpp-devel gtest-devel
git clone --recursive https://gitlab.cern.ch/cms-tracker-phase2-onlinesw/herd-control-app.git
cd herd-control-app
mkdir build
cd build
cmake3 ..
make
make install
cd ..
#rm -rf herd-control-app

# by now, we should be able to build the Apollo plugin for HERD
git clone --branch feature-cmake https://github.com/ammitra/ApolloHerd.git
cd ApolloHerd
mkdir build 
cd build
cmake3 ..
make

