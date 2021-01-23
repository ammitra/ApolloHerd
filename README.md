## ApolloSM plugin for HERD library

An ApolloSM-specific plugin using the [HERD Library](https://gitlab.cern.ch/cms-tracker-phase2-onlinesw/herd-library).

Derived from the work of the CMS Phase-2 Tracker Online SW group, whose work can be found [here](https://gitlab.cern.ch/cms-tracker-phase2-onlinesw).

## Overview

This repository contains an ApolloSM_device-specific plugin with an `ApolloDevice` class that utilizes the [EvaluateCommand]() functionality from [BUTool]() to wrap several ApolloSM_device commands, including:

* `cmpwrdown`, `cmpwrup`, `read`, `svfplayer` and 
* `dev_cmd`, which allows for the execution of any ApolloSM_device command by passing the command string and the appropriate arguments.

## Dependencies

The main dependency is the HERD library, which in turn requires:

* Build utilities: make, CMake3
* [boost]()
* [log4cplus]()

## Building on an Apollo

1. ssh into the SoC

2. Install the HERD control app and library, along with their dependencies, following the instructions in the [README]().

3. Build the plugin:

```
mkdir build && cd build
cmake3 ..
make
```

## Using this plugin with the control application

todo


