ROOT_DIR = $(shell pwd)

BUILD_HOME = $(ROOT_DIR)
include ${ROOT_DIR}/config/mfCommonDefs.mk

Project = swatch
Package = swatch

PackagePath = $(ROOT_DIR)
PackageName = cactuscore-swatch

Packager = Tom Williams

PACKAGE_VER_MAJOR = 1
PACKAGE_VER_MINOR = 2
PACKAGE_VER_PATCH = 0
PACKAGE_RELEASE = 1

Library = cactus_swatch

Includes = \
	include

LibraryPaths = \
	lib

Libraries = \
	dl \
	log4cplus \
	boost_regex \
	pthread \
	boost_thread \
	boost_chrono \
	boost_filesystem \
	yaml-cpp \
	zmq \
	msgpackc

ExecutableLibraries = \
	cactus_swatch \
	${Libraries}

include ${ROOT_DIR}/config/mfRules.mk
include ${ROOT_DIR}/config/mfRPMRules.mk
