cmake_minimum_required(VERSION 3.5)

project(Audio-modelling)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

option(ENABLE_LOG "Enable modelling logging" OFF)
option(ENABLE_PYTHON "Enable Python support" OFF)
option(ENABLE_SHARED_LIBRARIES "Enable shared libraries generation" ON)
option(ENABLE_STATIC_LIBRARIES "Enable static libraries generation" OFF)
option(ENABLE_CLANG_SUPPORT "Enable clang output and static generation" ON)

SET(CMAKE_MODULE_PATH ${ATK_DIR}/share/CMake)

find_package(ATK)
find_package(Boost REQUIRED log filesystem unit_test_framework system)

include_directories(SYSTEM ${Boost_INCLUDE_DIR})
include_directories(${ATK_INCLUDE_DIRS})
add_definitions(-DBOOST_NO_AUTO_PTR)

include(Utilities)

link_directories(${ATK_DIR}/lib)

if(ENABLE_PYTHON)
  add_subdirectory(${ATK_ORIGINAL_DIR}/3rdParty/pybind11 ${CMAKE_BINARY_DIR}/3rdParty/pybind11 )

  LIST(APPEND CMAKE_MODULE_PATH ${ATK_ORIGINAL_DIR}/3rdParty/pybind11/tools)
  FIND_PACKAGE(PythonLibsNew REQUIRED)

  IF(NOT DEFINED PYTHON_INSTALL_FOLDER)
    set(PYTHON_INSTALL_FOLDER "${CMAKE_INSTALL_PREFIX}/lib/")
  ENDIF(NOT DEFINED PYTHON_INSTALL_FOLDER)
endif(ENABLE_PYTHON)

if(ENABLE_CLANG_SUPPORT)
  find_package(LLVM REQUIRED CONFIG)

  add_definitions(-DENABLE_CLANG_SUPPORT)
  include_directories(${LLVM_DIR}/../../../include)
  link_directories(${LLVM_DIR}/../../../lib)
  add_definitions(${LLVM_DEFINITIONS})
endif(ENABLE_CLANG_SUPPORT)

add_subdirectory(ATK/Modelling)
if(ENABLE_PYTHON)
  add_subdirectory(Python/ATK/Modelling)
  add_subdirectory(test/Python/Modelling)
endif()
add_subdirectory(test)

FILE(GLOB_RECURSE CMAKE_OTHER_SRC Python/proto/*.py)

add_custom_target(Prototype SOURCES ${CMAKE_OTHER_SRC})
