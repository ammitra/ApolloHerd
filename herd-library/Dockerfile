FROM cern/cc7-base

ARG BRANCH_OR_TAG=master
RUN env

# Install from repos
RUN (for PACKAGE in \
    wget bzip2 git yaml-cpp \
    gcc gcc-c++ boost-devel \
    make cmake3 cppzmq-devel \
    doxygen log4cplus-devel \
    rpm-build git-core; \
  do \
    yum -y install ${PACKAGE} || exit $?; \
  done) \
  && yum-config-manager --add-repo https://download.opensuse.org/repositories/network:/messaging:/zeromq:/release-stable/CentOS_7/network:messaging:zeromq:release-stable.repo \
  yum clean all

# Build and install latest:
# GCC
RUN cd / \
  && wget http://ftp.mirrorservice.org/sites/sourceware.org/pub/gcc/releases/gcc-9.1.0/gcc-9.1.0.tar.gz \
  && tar zxf gcc-9.1.0.tar.gz \
  && mv gcc-9.1.0 gcc \
  && cd /gcc \
  && ./contrib/download_prerequisites \
  && ./configure --disable-multilib --enable-languages=c,c++ \
  && make \
  && make install \
  && cd / \
  && rm -rf /gcc \
  && ln -s /bin/cmake3 /bin/cmake \
  && cd / \
# GoogleTest
  && git clone --depth=1 -b $BRANCH_OR_TAG -q https://github.com/google/googletest.git /googletest \
  && mkdir -p /googletest/build \
  && cd /googletest/build \
  && cmake .. && make && make install \
  && cd / \
  && rm -rf /googletest \
  && yum remove -y wget bzip2 git

#RUN yum remove -y gcc gcc-c++

# Set Library Path
ENV LD_LIBRARY_PATH=/usr/local/lib:/usr/lib:/usr/local/lib64:/usr/lib64

# Default command
CMD /bin/bash
