## ApolloSM plugin for HERD library

An ApolloSM-specific plugin using the [HERD Library](https://gitlab.cern.ch/cms-tracker-phase2-onlinesw/herd-library).

Derived from the work of the CMS Phase-2 Tracker Online SW group, whose work can be found [here](https://gitlab.cern.ch/cms-tracker-phase2-onlinesw).

## Overview

This repository contains an ApolloSM_device-specific plugin with an `ApolloDevice` class that utilizes the [EvaluateCommand](https://github.com/BU-Tools/BUTool/blob/a1e09e6b002829820006bb8e749ccb9541450c17/include/BUTool/CommandList.hh#L172) functionality from [BUTool](https://github.com/BU-Tools/BUTool) to wrap several ApolloSM_device commands, including:

* `cmpwrdown`, `cmpwrup`, `read`, `svfplayer` and 
* `dev_cmd`, which allows for the execution of any ApolloSM_device command by passing the command string and the appropriate arguments.

## Dependencies

The main dependency is `swatch`, which in turn requires:

* Build utilities: make, CMake3
* [boost](https://boost.org/)
* [log4cplus](https://github.com/log4cplus/log4cplus)

And, in the switch from ZMQ & msgpack to HTTP & JSON, the HERD control app now requires
* [jsoncpp-devel](https://github.com/open-source-parsers/jsoncpp)
for communication.

## Building on an Apollo

1. ssh into the SoC

2. Install the HERD control app and library, along with their dependencies, following the instructions in the [README](https://gitlab.cern.ch/cms-tracker-phase2-onlinesw/herd-control-app):

    * Install the dependencies: `yum install gcc-c++ make cmake3 boost-devel log4cplus-devel yaml-cpp-devel jsoncpp-devel gtest-devel`
    * Ensure `swatch` checked out, or run `git submodule update --init`
    * **NOTE:** due to the old compiler on the Apollo failing with certain aspects of the build (namely the test suite, which is not necessary for this), certain aspects of the build process for `swatch` and the control app must be changed:
        * Comment out [Line 92](https://gitlab.cern.ch/cms-tracker-phase2-onlinesw/herd-control-app/-/blob/master/CMakeLists.txt#L92) of the control app CMakeLists.txt file
        * Comment out [Line 44](https://gitlab.cern.ch/cms-cactus/core/swatch/-/blob/master/swatch/CMakeLists.txt#L44), [Line 53](https://gitlab.cern.ch/cms-cactus/core/swatch/-/blob/master/swatch/CMakeLists.txt#L53), and [Line 63](https://gitlab.cern.ch/cms-cactus/core/swatch/-/blob/master/swatch/CMakeLists.txt#L53) of the `swatch` CMakeLists.txt file
    * Build HERD library and control application with
        ```
        mkdir build && cd build
        cmake3 ..
        make
        ```
    * Finally, if you want to install the HERD library and control application, run 
    ```
    sudo make install
    ```



3. Checkout the ApolloHerd plugin and build by running:

```
mkdir build && cd build
cmake3 ..
make
```

## Using this plugin with the control application
**If built directly on SoC:**
After installing the HERD control app and `swatch` (see above) and building ApolloHerd, run:

```
source env.sh
herd-control-app Apollo.yml
```

NOTE: if `ERROR: locale::facet::_S_create_c_locale name not valid` error, then prepend command with `LC_ALL=C` (until workaround found)

```
source env.sh
LC_ALL=C herd-control-app Apollo.yml
```

## WIP - Running in a container

Eventually, the goal is to run this software in a container on the Apollo. In the `Docker/` directory there are several scripts designed to be run inside of a container. The main script (almost working) is `build.sh` and installs all of the software necessary to run the HERD control app and the Apollo plugin for HERD. The code is all compiled inside of the container:

```
gitlab-registry.cern.ch/cms-tracker-phase2-onlinesw/herd-docker/herd-base-dev:master-8cd483a1
```

**If building in a container: (WIP)**
To build the image targeting armv7 on x86 architecture:
1. Navigate to directory containing Dockerfile
2. Run `docker buildx build --platform linux/arm -t ammitra/apolloherd . --push`
	* login to the docker hub and substitute your own username as necessary
This should build and push the armv7 image to the desired registry. 

Useful links regarding `buildx`:
* https://frightanic.com/computers/docker-buildx-the-best-thing-since-sliced-bread/
* https://medium.com/@artur.klauser/building-multi-architecture-docker-images-with-buildx-27d80f7e2408
