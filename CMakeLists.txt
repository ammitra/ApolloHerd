# with help from CMS online SW group documentation and
# https://cliutils.gitlab.io/modern-cmake/chapters/basics.html

cmake_minimum_required(VERSION 3.14.6 FATAL_ERROR)
project(ApolloHerd VERSION 0.0.1
                   DESCRIPTION "ApolloSM plugin for HERD lib"
                   LANGUAGES CXX)

# herd library
find_package(swatch 0.3.0 REQUIRED)

# paths to Apollo and Cactus
SET(APOLLO_PATH "/opt/BUTool")
SET(CACTUS_PATH "/opt/cactus")

#######################################################
# Main target: Declare - source files & dependencies
#######################################################

# adds library ApolloHerd to be be built from the source files in target_sources()
add_library(ApolloHerd SHARED)

# specifies sources to use when building ApolloHerd
target_sources(ApolloHerd PRIVATE
    src/common/ApolloDeviceController.cpp
    src/common/ApolloDevice.cpp
    src/common/commands/ApolloAccess.cpp
    src/common/commands/CMPwrDown.cpp
    src/common/commands/CMPwrUp.cpp
    src/common/commands/Read.cpp
    src/common/commands/SVFPlayer.cpp
    )

target_include_directories(ApolloHerd
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${APOLLO_PATH}/include
        ${CACTUS_PATH}/include
    )

target_link_directories(ApolloHerd PUBLIC
    ${APOLLO_PATH}/lib
    ${CACTUS_PATH}/lib
    )

target_link_libraries(ApolloHerd
    SWATCH::swatch_action
    BUTool_ApolloSM
    BUTool_ApolloSMDevice
    BUTool_GenericIPBus
    BUTool_GenericIPBusDevice
    BUTool_Helpers
    BUTool_IPBusIO
    BUTool_IPBusStatus
    BUTool
    ToolException
    )

#######################################################
# Main target: Build options
#######################################################

target_compile_features(ApolloHerd PUBLIC cxx_std_11)
target_compile_options(ApolloHerd PRIVATE -g -fPIC -O3 -Werror=return-type -Wall)

# Use pre-CXX11 ABI to resolve linker errors with gcc 8.3.0 on arm/v7
# See https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_dual_abi.html
add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)

#######################################################
# Install rules: Headers & binaries
#######################################################

include(GNUInstallDirs)

install(TARGETS ApolloHerd
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
