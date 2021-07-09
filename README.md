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

* The image should always be run using the ```start_apolloherd.sh``` shell script. This script wraps the `docker run` command, makes all UIO devices accessible inside the container, mounts several volumes, and sets permissions.

* The Docker image URL will differ based on the architecture on which the container will be run (`arm/v7` on the rev.1 blades and `arm64` on the rev.2 blades):

  * **Tags:**

    * `gitlab-registry.cern.ch/ammitra/apollo-herd/linux-arm-v7/herd-app:vX.Y.Z`
    * `gitlab-registry.cern.ch/ammitra/apollo-herd/linux-arm64/herd-app:vX.Y.Z`

  * **Branches:**

    * `gitlab-registry.cern.ch/ammitra/apollo-herd/linux-arm-v7/herd-app:BRANCHNAME-COMMITSHA`
    * `gitlab-registry.cern.ch/ammitra/apollo-herd/linux-arm64/herd-app:BRANCHNAME-COMMITSHA`

    Where `COMMITSHA` is the first 8 characters of the git commit's SHA

An example of running the plugin in a container using the non-default config file located in the `/path/to/apollo-config/` directory on the host machine, mounted as `/config/` in the container:

```
./path/to/start_apolloherd.sh -d -p 3000:3000 -v /path/to/apollo-config:/config gitlab-registry.cern.ch/ammitra/apollo-herd/linux-arm-v7/herd-app:vX.Y.Z /config/Apollo.yml 
```

The above command
  
1. runs the container in the background with the `-d` flag
2. maps TCP port `3000` in the container to `3000` on the Docker host
3. mounts the directory `/path/to/apollo-config` on the host to `/config` in the container. In this example, and in practice, the directory should contain one config file, named `Apollo.yml`
4. specifies the appropriate apollo-herd image to run, and, 
5. starts the HERD control app with the appropriate Apollo-specific config file, `Apollo.yml`