## ApolloSM plugin for HERD library

An ApolloSM-specific plugin using the [HERD Library](https://gitlab.cern.ch/cms-tracker-phase2-onlinesw/herd-library).

Derived from the work of the CMS Phase-2 Tracker Online SW group, whose work can be found [here](https://gitlab.cern.ch/cms-tracker-phase2-onlinesw).

## Overview

This repository contains an ApolloSM_device-specific plugin with an `ApolloDevice` class that utilizes the [EvaluateCommand](https://github.com/BU-Tools/BUTool/blob/a1e09e6b002829820006bb8e749ccb9541450c17/include/BUTool/CommandList.hh#L172) functionality from [BUTool](https://github.com/BU-Tools/BUTool) to wrap several ApolloSM_device commands, including:

* `cmpwrdown`, `cmpwrup`, `read`, `svfplayer` and 
* `dev_cmd`, which allows for the execution of any ApolloSM_device command by passing the command string and the appropriate arguments.

## Dependencies

The main dependency is the HERD library, which in turn requires:

* Build utilities: make, CMake3
* [boost](https://boost.org/)
* [log4cplus](https://github.com/log4cplus/log4cplus)

And, in the switch from ZMQ & msgpack to HTTP & JSON, the HERD control app now requires
* [jsoncpp-devel](https://github.com/open-source-parsers/jsoncpp)
for communication.

## Building on an Apollo

1. ssh into the SoC

2. Install the HERD control app and library, along with their dependencies, following the instructions in the [README](https://gitlab.cern.ch/cms-tracker-phase2-onlinesw/herd-control-app).

3. Build the plugin:

```
mkdir build && cd build
cmake3 ..
make
```

## Using this plugin with the control application

todo


