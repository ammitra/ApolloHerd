# *****************************************************************************
# Run docker buildx build --platform linux/arm -t ammitra/apolloherd . --push
# inside of this directory. Currently fails while building BUTool:
# /usr/include/boost/program_options/detail/parsers.hpp:43: undefined reference to `boost::program_options::detail::cmdline::cmdline(std::vector<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > > > const&)'
# seems to be a compiler version issue? or maybe something else
# *****************************************************************************

FROM gitlab-registry.cern.ch/cms-tracker-phase2-onlinesw/herd-docker/herd-base-dev:master-8cd483a1 as base
# 1) install dependency packages, along with uHAL and UIOuHAL
RUN \
    yum -y install git readline-devel zlib-devel && \
    yum -y install rpm-build git-core erlang gcc-c++ boost-devel pugixml-devel python-devel && \
    # building uHAL
    cd /tmp/ && \
    git clone --branch v2.7.4 https://github.com/ipbus/ipbus-software.git && \
    cd ipbus-software && \
    # changed BUILD_UHAL_TESTS: 1 -> 0 BUILD_UHAL_PYCOHAL: 1 -> 0
    make -j$(nproc) Set=uhal BUILD_PUGIXML=0 BUILD_UHAL_TESTS=0 BUILD_UHAL_PYCOHAL=0 && \
    make Set=uhal BUILD_PUGIXML=0 BUILD_UHAL_TESTS=0 BUILD_UHAL_PYCOHAL=0 install && \
    cd .. && \
    rm -rf ipbus-software/ && \
    export CACTUS_ROOT=/opt/cactus && \
    # building UIOuHAL
    git clone --branch feature-zynqmp_issues https://github.com/dgastler/UIOuHAL.git && \
    cd UIOuHAL && \
    make && \
    mkdir -p /opt/UIOuHAL && \
    cp -r lib /opt/UIOuHAL && \
    cp -r include /opt/UIOuHAL && \
    cd .. && \
    rm -rf UIOuHAL && \
    # install needed (?) BUTool dependencies
    yum -y install boost-chrono boost-regex boost-thread boost-filesystem boost-system boost-devel boost-python boost-program-options && \
    # install BUTool from ApolloTool meta repo
    git clone --branch feature-USP_addrs https://github.com/apollo-lhc/ApolloTool.git && \
    cd ApolloTool && \
    make init && \
    rm -rf plugins/IPMC_plugin && \
    # Now we deal with the -Wno-format-truncation issue by appending the flag to the CXX_FLAGS variable
    sed -i "s|-Wno-ignored-qualifiers|-Wno-ignored-qualifiers -Wno-format-truncation|g" plugins/ApolloSM_plugin/Makefile && \
    make local -j$(nproc) RUNTIME_LDPATH=/opt/BUTool COMPILETIME_ROOT=--sysroot=/ && \
    make install RUNTIME_LDPATH=/opt/BUTool COMPILETIME_ROOT=--sysroot=/ INSTALL_PATH=/opt/BUTool && \
    cd .. && \
    rm -rf ApolloTool 

# 2) get only the needed software into a smaller image - this layer already has herd-control-app and HERD lib installed
FROM gitlab-registry.cern.ch/cms-tracker-phase2-onlinesw/herd-control-app/herd-control-app:master-c6a768dc as plugin
# copy over BUTool, cactus, UIOuHAL
COPY --from=base /opt/ /opt/
# build plugin
RUN \
    git clone --branch feature-cmake https://github.com/ammitra/ApolloHerd.git && \
    cd ApolloHerd && \
    mkdir build && \
    cd build && \
    cmake3 .. && \
    make

FROM plugin as app
USER root
ENTRYPOINT [ "/bin/bash", "-c" ]
CMD [ "bash" ]
