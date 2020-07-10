# HERD library

The HERD library has been developed to support the development of on-board software applications for controlling and monitoring ATCA boards in the phase-2 upgrade of the CMS tracker. Specifically, it provides a framework for the registration of arbitrary system-/board-specific control/configuration procedures and monitoring data, which can then be accessed from generic system-/board-independent applications using the library's abstract interfaces. It is based on the SWATCH framework that was developed for the phase-1 upgrade of the CMS level-1 trigger system.


## Dependencies

The main dependencies of the HERD library are:

 * Build utilities: make, CMake3
 * [boost](https://boost.org)
 * [log4cplus](https://github.com/log4cplus/log4cplus)
 * [yaml-cpp](https://github.com/jbeder/yaml-cpp)
 * [msgpack](https://github.com/msgpack/msgpack-c)
 * [cppzmq](https://github.com/zeromq/cppzmq)


## Build instructions

 1. Install the dependencies listed above. For example, on CentOS7:
```
yum install gcc-c++ make cmake3 boost-devel log4cplus-devel yaml-cpp-devel msgpack-devel cppzmq-devel
```

 2. Build the HERD library
```
mkdir build
cd build
cmake3 -DBUILD_SHARED_LIBS=ON ..
make
```


## Using a HERD plugin with the prototype control application

The `herd-library` repository contains a minimal skeleton of the control application, `control-fsm-server`, which opens a ZMQ socket to allow remote applications to run HERD commands and FSM transitions. It must be run with a YAML configuration file which lists the plugin libraries to load (`libraries` field) and the devices to instantiate (`devices` field); for an example of this file, see `dummy.yaml` in the `herd-dummy` repository. If run with `dummy.yaml` as the configuration file, the control application will load the dummy plugin and create two devices (of type `swatch::dummy::DummyDevice`, named `x0` and `x1`):
```
source env.sh
./build/control-fsm-server dummy.yaml
```

While this application is running, you can control the dummy devices instantiated by this server (i.e. run their commands and FSM transitions) using `control-app-console.py`:
```
./control-app-console.py
```
In this console you can:
 * list device, command and FSM information by typing `info`;
 * run commands by typing `run-command DEVICE_ID COMMAND_ID` (e.g. `run-command x0 reset`);
 * engage FSMs by typing `engage-fsm DEVICE_ID FSM_ID` (e.g. `engage-fsm x0 myFSM`); and
 * reset FSMs by typing `reset-fsm DEVICE_ID FSM_ID` (e.g. `reset-fsm x0 myFSM`); and
 * disengage FSMs by typing `disengage-fsm DEVICE_ID FSM_ID` (e.g. `disengage-fsm x0 myFSM`); and
 * run FSM transitions by typing `run-transition DEVICE_ID COMMAND_ID` (e.g. `run-transition x0 configure`)


## Test suite

The test suite is used to validate the functionality of the library's core classes and functions. It can be run as follows:
```
source tests/env.sh
./build/herd_test_runner --log_level=test_suite
```
