FROM cern/cc7-base:20170113

MAINTAINER "Tom Williams" <tom.williams@cern.ch>


RUN (for PACKAGE in \
    make cmake3 rpm-build git-core \
    doxygen \
    gcc-c++-4.8.5 \
    boost-devel \
    log4cplus-devel \
    cppzmq-devel yaml-cpp-devel; \
  do \
    yum -y install ${PACKAGE} || exit $?; \
  done) && \
  yum clean all


# Default command
CMD /bin/bash

