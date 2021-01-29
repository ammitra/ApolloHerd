#!/bin/bash

# builds ApolloTool with necessary Apollo SW libraries for use in container
# Use: source ApolloTool.sh
# to be performed in container, as root (no need for sudo)

# ******************** NOTE ****************************
# Using ApolloTool over BUTool is not recommended, and 
# currently does not work. Instead, use BUTool.sh
# *****************************************************

# install necessary packages
yum -y install git readline-devel zlib-devel

# UIOuHAL build
#yum groupremove uhal

# add appropriate repo to yum.repos.d
cat > /etc/yum.repos.d/ipbus-sw.repo << EOF
[ipbus-sw-base]
name=IPbus software repository
baseurl=http://www.cern.ch/ipbus/sw/release/2.7/repos/centos7_gcc8/x86_64/base/RPMS
enabled=1
gpgcheck=0

[ipbus-sw-updates]
name=IPbus software repository updates
baseurl=http://www.cern.ch/ipbus/sw/release/2.7/repos/centos7_gcc8/x86_64/updates/RPMS
enabled=1
gpgcheck=0
EOF

# install ipbus
yum clean all
yum -y groupinstall uhal

# adding proper path to cactus
export CACTUS_ROOT="/opt/cactus"

# build UIOuHAL
cd /opt/
git clone https://github.com/BU-Tools/UIOuHAL/
cd UIOuHAL 
make 

# building ApolloTool - at this point dependencies should be satisfied
cd /home/
git clone --recursive https://github.com/apollo-lhc/ApolloTool.git
cd ApolloTool/
# -k, --keep-going: tells compiler to keep going as much as possible
# used because in ApolloSM_device/src/standalone/peekUIO.cxx
# the -k workaround is dangerous, need to find a way to add `-Wno-format-truncation` compiler flag when building ApolloSM_dvice plugin
make -k local

# build herd-control-app and herd-library & install headers and libs
cd /home/
git clone --recursive https://gitlab.cern.ch/cms-tracker-phase2-onlinesw/herd-control-app.git
cd herd-control-app/
mkdir build
cd build/
cmake3 ..
make 
make install

# build ApolloHerd plugin
cd /home/
git clone -b feature-cmake http://github.com/ammitra/ApolloHerd.git
cd ApolloHerd/
mkdir build/
cd build/
cmake3 ..
# *** Here is where the issue is, due to how ApolloTool builds BUTool ***
# on Apollo, BUTool (and APOLLO_PATH) is /opt/BUTool/
# /opt/BUTool/include contains all the plugins as well
# this is why I think it is better to write a BUTool.sh script and build everything that way
make 




