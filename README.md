# Apollo SWATCH plugin

This repository contains a SWATCH plugin library for the Apollo board. Two classes, `ApolloDevice` and `ApolloCMFPGA`, utilize [BUTool](https://github.com/BU-Tools/BUTool) and its Apollo Service Module control plugin [ApolloSM_plugin](https://github.com/apollo-lhc/ApolloSM_plugin) to interact with the Service Module and Command Module hardware, respectively. In addition, the `ApolloCMFPGA` class builds on top of the `EMPDevice` class located in the [EMP SWATCH](https://gitlab.cern.ch/p2-xware/software/emp-herd) plugin, which provides the CM control class with functionality for controlling the EMP (Extensible, Modular, data Processor) firmware framework running on the Command Module's two main FPGAs - the Kintex and Virtex. 

This plugin is derived from the work of the CMS Phase-2 Tracker Online SW group, whose repositories can be found [here](https://gitlab.cern.ch/cms-tracker-phase2-onlinesw).


## Dependencies

This plugin has several main dependencies:

* [EMP software](https://serenity.web.cern.ch/serenity/emp-fwk/software/)
* [EMP SWATCH plugin](https://gitlab.cern.ch/p2-xware/software/emp-herd/)
* [ApolloTool](https://github.com/apollo-lhc/ApolloTool)
* [SWATCH software](https://gitlab.cern.ch/cms-cactus/core/swatch)

Additional subdependencies may arise from these. Due to compatibility issues with the compiler on the rev.1 Apollo blades, this plugin is no longer meant to be built from source. Instead, the plugin is run in a docker container using the instructions below. 

## Using this plugin with the control application

The HERD control application (implemented in the herd-control-app repository) is an executable that loads SWATCH/HERD plugins, and provides a network interface that allows remote applications to run the commands and FSM transitions procedures declared in those plugins. If run with `Apollo.yml` as the configuration file, the control application will load the SWATCH plugin and create three devices:

* one for the Service Module (named `ApolloSM_device`), and 
* two for the Command Module FPGAs (named `kintex` and `virtex`).

At the moment, the control application can be run with the Apollo plugin only in a docker container:

* **Docker information will be added once a tag is created**