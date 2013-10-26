cmake_minimum_required(VERSION 2.8)

project(ReadTag)

set(SOURCES ReadTag.cpp
            ${CMAKE_SOURCE_DIR}/omaha/extractor.cpp)

set(HEADERS ${CMAKE_SOURCE_DIR}/omaha/basictypes.h
            ${CMAKE_SOURCE_DIR}/omaha/extractor.h)

add_executable(ReadTag ${SOURCES} ${HEADERS})

include_directories("${CMAKE_SOURCE_DIR}/omaha")