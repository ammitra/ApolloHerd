# Design overview

This file outlines the structure of this repository, the roles of the most important classes, and the relationships between them.


## User interface

The main user-facing classes are:

 1. `Command`: An abstract base class representing an action, e.g: reset FPGA; reboot FPGA; configure firmware component X.

   * The functionality for specific actions (such as those from the list above), is implemented in classes that derive from the `Command` class.

   * Parameters for the commands (and constraints on the parameters' values) are registered in the constructor of the derived class. The parameters are stored in instances of the `ParameterSet` class (essentially, a wrapper class for a map of strings to values of any type).

   * The derived class must implement the following method, which will be called whenever the command is executed:
     ```
     Comand::State code(const ParameterSet& parameters);
     ```
   
     The associated resource (i.e. `Device` object) can be accessed from within this method using the template method `T& getActionable<T>()`.

 2. `Device`: A generic base class that represents a single FPGA

   * Commands for specific hardware/applications can be registered by creating a class that inherits from the `Device` class, and calling the `registerCommand` method in the constructor of the user-defined derived class.

 3. `Board`: A generic class that represents a board hosting multiple FPGAs. It contains a map of strings to `Device` pointers.


### Examples and intended usage

The `herd-dummy` repository contains a few classes that illustrate how to create a hardware-/application-specific HERD plugin library. Specifically, it contains an example device class, `DummyDevice` (see `DummyDevice.{hpp,cpp}`), that:

 * registers 5 commands, implemented by the `AlignLinks`, `Reset` and `Reboot` and `Reset` classes; and
 * registers an example Finite State Machine (FSM), whose transitions consist of one or more of the commands, run in sequence

The source code for these classes demonstrate how the HERD-SWATCH API can be used for creating and registering "plugin" classes that control and monitor firmware.

This repository contains a minimal skeleton of the control application, `control-fsm-server`, which loads user-created HERD plugin libraries and opens a ZMQ socket to allow remote applications to run HERD commands and FSM transitions. It is expected that user-created HERD device classes (and the associated commands & transitions) will be intially used via `control-fsm-server`, and eventually with a less minimal centrally-developed control application that supercedes it. Instructions for running `control-fsm-server` can be found in this repository's README (and an example configuration file can be found in the `herd-dummy` repository).



## Internal and optional interfaces

`core` directories - most important classes:

 * `AbstractFactory`: A factory class, used to create instances of classes that are developed by board/application developers without needing to link against the 'plugin' libraries that they developed (or explicitely refer to user-developed classes) in generic framework applications.

   * Device classes are registered to the factory by adding the `SWATCH_REGISTER_CLASS` macro to the class' `.cpp` file (e.g. see `DummyDevice.cpp` in `herd-dummy`)

 * `ParameterSet`: A container of parameters of arbitrary type, each named with an ID string. I.e. a wrapper for map of strings to values of any type.

 * `PSetConstraint`, `Rule` and `Match`

   * `Rule` and `PSetConstraint` represent restrictions on the values of parameters (individual or multiple parameters respectively). For example: integer must be in range 0 to 10; string can only have values X, Y and Z.
   * `Match` represents the result of applying these rules/constraints to specific values. It's a plain struct, containing a bool (indicating whether the rule is satisfied or violated) and a string (user-friendly message summarising the cause for rule violation)
   * These classes are used by the `Command` class to allow board/application developers to explicitly declare such restrictions to the framework (e.g. see the `clockSource` parameter of the `Reset` command in `herd-dummy`)

 * `Object`: Base class that handles ownership of child objects associated with ID strings - e.g. devices and commands (the latter being the child object)

   * Child objects must inherit from `Object` or `LeafObject` and be heap-allocated. They are owned by their parent object, and deleted by the `Object` class' destructor.

`action` directories - most important classes:

 * `ActionableObject`: Base class representing the 'resource' that a command is acting upon. `Device` inherits from `ActionableObject`, and `ActionableObject` inherits from `Object`

   * Contains a map of named commands, and provides the `registerCommand` method allowing commands to be registered in derived classes.

 * `Command`: An abstract base class representing an action, e.g: reset; reboot; configure firmware component X. (See above, first section)

 * `CommandSnapshot`: Represents current snapshot of a command's state. E.g: whether command is still running, or has finished execution; duration of last/current invocation; latest message from command; parameters used for the command.

   * Retrieved through the `Command::getStatus()` method

 * `Board`: A generic class that represents a board hosting multiple FPGAs. Contains a map of strings to `Device` pointers.

 * `Device`: A generic base class that represents a single FPGA. (See above, first section)

 * `DeviceStub` Simple struct that contains the information required to construct classes that inherit from `Device` (i.e. an ID string, a class name, info for establishing communication with FPGA, path to address table)
