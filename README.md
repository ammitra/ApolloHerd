# Derived from the work of the CMS Phase-2 Tracker Online SW group:
Their work can be found [here](https://gitlab.cern.ch/cms-tracker-phase2-onlinesw)

# ApolloSM_device plugin for HERD library

This repository contains an ApolloSM_device-specific plugin with an `ApolloDevice` class that utilizes the [EvaluateCommand](https://github.com/dgastler/BUTool/blob/0e436628f55c17be3e43840e9006f1e75b787413/include/BUTool/CommandList.hh#L63) functionality from [BUTool](https://github.com/dgastler/BUTool/tree/0e436628f55c17be3e43840e9006f1e75b787413) to wrap several ApolloSM_device commands, including: 

 * `cmpwrdown`, `cmpwrup`, `read`, `svfplayer` and
 * `dev_cmd`, which allows for the execution of any ApolloSM_device command.
 
## Dependencies

The main dependency is the HERD library, which in turn requires the following:

 * Build utilities: make, CMake3
 * [boost](https://boost.org)
 * [log4cplus](https://github.com/log4cplus/log4cplus)
 * [yaml-cpp](https://github.com/jbeder/yaml-cpp)
 * [msgpack](https://github.com/msgpack/msgpack-c)
 * [cppzmq](https://github.com/zeromq/cppzmq)


## Build instructions

 1. Install the HERD library depdendencies listed above. For example, on CentOS7:
```
yum install gcc-c++ make cmake3 boost-devel log4cplus-devel yaml-cpp-devel msgpack-devel cppzmq-devel
```

 2. Build the [HERD library](https://github.com/ammitra/ApolloHerd/tree/master/herd-library).

 3. Build the ApolloSM_device plugin:
```
mkdir build
cd build
cmake3 -DBUILD_SHARED_LIBS=ON ../herd-library
make
cd ..
make
```


## Using this plugin with the control application

```
source env.sh
./build/control-fsm-server Apollo.yml
```

While this application is running, you can control the ApolloDevice instantiated by this server using `control-app-console.py`:
```
./herd-library/control-app-console.py
```
In this console you can:
 * list device, command and FSM information by typing `info`;
 * run commands by typing `run-command DEVICE_ID COMMAND_ID` (e.g. `run-command ApolloSM_device read`);

`control-fsm-server` by default starts a ZMQ TCP socket on port 12345. If you wish to use `control-app-console.py` to issue commands to the server from a remote computer, you must run:
```
control-app-console.py <ip_address> 12345
```
from that computer.
