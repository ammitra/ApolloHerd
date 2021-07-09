#!/usr/bin/env bash

# ---------------------------------------------------------------------
# Wrapper script for 'docker run'
#
#   Automatically adds all flags required to communicate with the 
#   hardware access software on the Apollo boards. All user-supplied 
#   flags and arguments are added on the command line and passed to 
#   this script after the mandatory flags included here
#
# Usage:
#
#   start_apolloherd.sh <flags> <apollo-herd image URL or ID>
#
# Example:
#
#   start_apolloherd.sh -it -v /local/path:/path/inside/container imageID_or_imageURL
#
# The above example will run the container interactively and mount some
# directory on the host to the specified path inside the container
#
# Credit: Tom Williams, RAL
#
# ---------------------------------------------------------------------

set -e

# gather all the UIO devices, use --device flag to make accessible in container
UIOs=$(ls -1 /dev/uio* | awk '{print " --device="$1":"$1}' | xargs)

# build array with command and flags
declare -a DOCKER_RUN_CMD=(docker run)
# pass all UIO devices to container
DOCKER_RUN_CMD+=($UIOs)
# mount address tables
DOCKER_RUN_CMD+=(-v /opt/address_table:/opt/address_table)
DOCKER_RUN_CMD+=(-v /opt/address_tables:/opt/address_tables)
# mount proc fs
DOCKER_RUN_CMD+=(-v /proc:/proc)
# run as privileged (for now)
DOCKER_RUN_CMD+=(--privileged)

"${DOCKER_RUN_CMD[@]}" $*