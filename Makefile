ROOT_DIR= $(shell pwd)

#===============================================================
#					added ApolloSM path and libs
#===============================================================

APOLLO_PATH = /opt/BUTool
CACTUS_ROOT = /opt/cactus

BUILD_HOME = $(ROOT_DIR)
include ${ROOT_DIR}/herd-library/deprecated/mfCommonDefs.mk

Project = swatch
Package = dummy

PackagePath = $(ROOT_DIR)
PackageName = cactuscore-herd-dummy

Packager = Alessandro Thea

PACKAGE_VER_MAJOR = 1
PACKAGE_VER_MINOR = 2
PACKAGE_VER_PATCH = 2
PACKAGE_RELEASE = 1

Library = herd_dummy

Includes = \
	include  \
	${ROOT_DIR}/herd-library/include \
	${APOLLO_PATH}/include \
	${CACTUS_ROOT}/include

LibraryPaths = \
	${ROOT_DIR}/build \
	${ROOT_DIR}/lib \
	${APOLLO_PATH}/lib

Libraries = \
	herd \
	BUTool_ApolloSM \
	BUTool_ApolloSMDevice \
	BUTool_GenericIPBus \
	BUTool_GenericIPBusDevice \
	BUTool_Helpers \
	BUTool_IPBusIO \
	BUTool_IPBusStatus \
	BUTool \
	ToolException

ExecutableLibraries = \
	log4cplus \
	boost_system \
	boost_filesystem \
	boost_regex \
	boost_thread \
	herd \
	dl

include ${ROOT_DIR}/herd-library/deprecated/mfRules.mk
include ${ROOT_DIR}/herd-library/deprecated/mfRPMRules.mk