cmake_minimum_required(VERSION 2.8)

project(ReadTagDLL)

set(SOURCES pluginapi.h
            ReadTagDLL.h
            nsis_tchar.h
            ${CMAKE_SOURCE_DIR}/omaha/basictypes.h
            ${CMAKE_SOURCE_DIR}/omaha/extractor.h
            )

set(HEADERS pluginapi.c
            ReadTagDLL.cpp
            ${CMAKE_SOURCE_DIR}/omaha/extractor.cpp
            )

add_library(ReadTagDLL SHARED ${SOURCES} ${HEADERS})

include_directories("${CMAKE_SOURCE_DIR}/omaha")

if (MSVC)
  set ( CMAKE_CXX_FLAGS_MINSIZEREL "/O1 /Os /MT")
endif ()