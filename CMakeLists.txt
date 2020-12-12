cmake_minimum_required(VERSION 3.0.0)
project(SR VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
include(CTest)
enable_testing()
link_libraries(pthread)
add_executable(sender sender.cpp
                protocol/proto_definition.h
                protocol/proto_SR.cc
                protocol/proto_SR.h
                protocol/timer.h
                socket/udp.cc
                socket/udp.h
                _SR.h
                )
add_executable(receiver receiver.cpp
                protocol/proto_definition.h
                protocol/proto_SR.cc
                protocol/proto_SR.h
                protocol/timer.h
                socket/udp.cc
                socket/udp.h
                _SR.h
                )
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
