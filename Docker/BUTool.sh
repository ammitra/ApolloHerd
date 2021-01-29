#!/bin/bash

# Builds BUTool with necessary Apollo SW libraries and builds Apollo plugin for HERD
# USE: run this shell script inside the CERN gitlab docker container

# install necessary packages
yum -y install git readline-devel zlib-devel

# UIOuHAL build
#yum groupremove uhal

# UIOuHAL build
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

# build BUTool and plugins 
cd /opt/
git clone https://github.com/BU-Tools/BUTool.git
cd BUTool/plugins/
git clone https://github.com/apollo-lhc/ApolloSM_plugin.git
git clone https://github.com/BU-Tools/BUTool-IPBUS-Helpers.git
git clone https://github.com/BU-Tools/GenericIPBus_plugin.git
cd ../
cat > plugins/Makefile.plugins << EOF
ApolloSM_plugin: BUTool-IPBUS-Helpers
         ${MAKE} ${FLAGS} -C $@
GenericIPBus_plugin: BUTool-IPBUS-Helpers
         ${MAKE} ${FLAGS} -C $@
EOF
cp make/Makefile.x86 ./Makefile
# -k flag to ignore ApolloSM_device peek/pokeUIO.cxx from format error on build
# this is not good - find a way to add the compiler flag `-Wno-format-truncation` when building ApolloSM_device plugin
make -k 

