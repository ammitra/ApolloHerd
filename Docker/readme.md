# Testing Containers

This directory contains some shell scripts used for testing the operation of ApolloHerd in a container

1) First, pull the base image for plugin repos:

```
docker pull gitlab-registry.cern.ch/cms-tracker-phase2-onlinesw/herd-docker/herd-base-dev:master-8cd483a1
```

2) Get the container ID and run `docker run -ti <CONTAINER ID>`. This will allow an interactive session in the container as root. 

3) **WIP:** for now, just copy either `BUTool.sh` or `ApolloTool.sh` into `/home/` in the container and run it. This will automatically build BUTool and necessary ApolloSM plugins, as well as the `herd-control-app`, `herd-library`, and `ApolloHerd` plugin for the ApolloSM


## Notes

The ApolloSM_device plugin has make errors when building `src/standalone/*UIO.cxx` due to `snprintf`:

```
src/standalone/peekUIO.cxx: In function 'int main(int, char**)':
src/standalone/peekUIO.cxx:49:5: error: '%d' directive output may be truncated writing between 1 and 10 bytes into a region of size 8 [-Werror=format-truncation=]
     "/dev/uio%d",uio);
     ^~~~~~~~~~~~
src/standalone/peekUIO.cxx:49:5: note: directive argument in the range [0, 2147483647]
src/standalone/peekUIO.cxx:48:11: note: 'snprintf' output between 10 and 19 bytes into a destination of size 16
   snprintf(UIOFilename,strlen(UIOFilename),
   ~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     "/dev/uio%d",uio);
```

This can either be dealt with in the ApolloSM_device plugin `Makefile` by adding `-Wno-format-truncation` to the `CXX_FLAGS`

**To do:** Find a way to append the compiler flag `-Wno-format-truncation` to the make only for building the ApolloSM_device plugin. Adding a global `-k` flag to the make for the whole thing is dangerous. 