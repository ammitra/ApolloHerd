# Set minimums
cmake_minimum_required(VERSION 3.14.6)
project(herd_lib VERSION 0.01)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Locations on all platforms
#TODO change to CERN CENTOS
include(GNUInstallDirs)

# Set build flags according to if building for release or debug
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Werror=return-type")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -g -shared -fPIC -Wall -O3")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -g -Wall -O3")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -ggdb -Wall -Werror=return-type -MMD -MP -fPIC")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} -g -ggdb -shared -fPIC -Wall")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -g -ggdb -Wall")

# Add library sources
add_library(herd "")
target_sources(herd PRIVATE
        src/common/action/ActionableObject.cpp
        src/common/action/ActionableStatus.cpp
        src/common/action/Board.cpp
        src/common/action/BusyGuard.cpp
        src/common/action/Command.cpp
        src/common/action/CommandSequence.cpp
        src/common/action/CommandVec.cpp
        src/common/action/Device.cpp
        src/common/action/DeviceStub.cpp
        src/common/action/Functionoid.cpp
        src/common/action/GateKeeper.cpp
        src/common/action/StateMachine.cpp
        src/common/action/ThreadPool.cpp

        src/common/core/exception.cpp
        src/common/core/Factory.cpp
        src/common/core/LeafObject.cpp
        src/common/core/Match.cpp
        src/common/core/Object.cpp
        src/common/core/ParameterSet.cpp
        src/common/core/PSetConstraint.cpp
        src/common/core/Rule.cpp
        src/common/core/utilities.cpp

        src/common/core/rules/FiniteNumber.cpp
        src/common/core/rules/FiniteVector.cpp
        src/common/core/rules/GreaterThan.cpp
        src/common/core/rules/IsAmong.cpp
        src/common/core/rules/InRange.cpp
        src/common/core/rules/LesserThan.cpp
        src/common/core/rules/NonEmptyString.cpp
        src/common/core/rules/OfSize.cpp
        src/common/core/rules/OutOfRange.cpp
        src/common/core/rules/PathExists.cpp

        src/common/examples/utilities.cpp

        src/common/logger/Logger.cpp
        src/common/logger/SwatchPatternLayout.cpp

        src/common/server/Server.cpp
        src/common/server/utilities.cpp
        )

# Add included directories
target_include_directories(herd PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)

# Add dependencies
find_package(Boost REQUIRED)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
#add_library(boost_regex)

target_link_libraries(herd dl log4cplus boost_system boost_regex pthread boost_thread boost_chrono boost_filesystem msgpackc zmq yaml-cpp)


# Add executable sources
set( EXE_SOURCES src/common/server/control-fsm-server.cxx )
foreach( EXE_SOURCE_FILE ${EXE_SOURCES} )
  get_filename_component( EXE_NAME ${EXE_SOURCE_FILE} NAME_WE )
  add_executable( ${EXE_NAME} ${EXE_SOURCE_FILE} )
  target_link_libraries( ${EXE_NAME} herd )
endforeach( EXE_SOURCE_FILE ${EXE_SOURCES} )

target_link_libraries( control-fsm-server )

add_library(herd_tests "")
target_sources(herd_tests PRIVATE
       tests/src/common/core/TestConstraints.cpp
       tests/src/common/core/TestFactory.cpp
       tests/src/common/core/TestObject.cpp
       tests/src/common/core/TestParameterSet.cpp
       tests/src/common/core/TestRules.cpp

       tests/src/common/action/DummyActionableObject.cpp
       tests/src/common/action/DummyCommand.cpp
       tests/src/common/action/DummyDevice.cpp
       tests/src/common/action/DummyGateKeeper.cpp
       tests/src/common/action/DummySleepCommand.cpp
       tests/src/common/action/TestActionableObject.cpp
       tests/src/common/action/TestCommand.cpp
       tests/src/common/action/TestGateKeeper.cpp
       tests/src/common/action/TestStateMachine.cpp

       tests/src/common/server/TestUtilities.cpp
       )

target_link_libraries(herd_tests herd boost_unit_test_framework )

target_include_directories(herd_tests PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests/include>
        PRIVATE tests/src)

add_executable( herd_test_runner tests/src/common/runTests.cxx )
target_link_libraries( herd_test_runner herd_tests )


#TODO get this working in standalone her_lib repo
# 'make install' to the correct locations //Currently based on GNUInstallDirs
#install(TARGETS herd EXPORT herdConfig
#        ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR})
#install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Make herd importable from the install directory by placing config file in per-project dir
#install(EXPORT herdConfig DESTINATION share/herd/cmake)

# Makes herd importable from the build directory
#export(TARGETS herd FILE herd.cmake)

#Test
#TODO
#add_executable(testHerd tests/testlib.cpp)
