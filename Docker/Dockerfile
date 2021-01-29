# just for testing, don't use. 
# instead, pull the below image and run either BUTool.sh or ApolloTool.sh (not working yet)

# base image for plugin repos
FROM gitlab-registry.cern.ch/cms-tracker-phase2-onlinesw/herd-docker/herd-base-dev as base
RUN \
  yum -y install git && \
  cd home && \
  git clone --recursive https://gitlab.cern.ch/cms-tracker-phase2-onlinesw/herd-control-app.git \
FROM base as app

# Default user & command
USER root
ENTRYPOINT ["/bin/bash", "-c"]
CMD ["bash"]
