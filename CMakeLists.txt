cmake_minimum_required(VERSION 3.25)

project(EngineGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENGINE_BUILD_APPS "Build example applications" ON)
option(ENGINE_ENABLE_BGFX "Enable bgfx renderer bootstrap in sandbox" ON)

add_subdirectory(engine)

if(ENGINE_BUILD_APPS)
  add_subdirectory(apps)
endif()
