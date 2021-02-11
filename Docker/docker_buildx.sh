#!/bin/bash

# this script pulls a QEMU docker image which satisfies the emulator
# requirements without needing QEMU and binfmt-support packages on the host system
# run this script before trying to build multi-platform images on your host system

docker run --rm --privileged multiarch/qemu-user-static --reset -p yes