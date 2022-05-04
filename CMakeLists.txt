cmake_minimum_required(VERSION 3.13.0)
project(CWave VERSION 0.1.0)

include(CTest)
enable_testing()

add_subdirectory(src)
include_directories(headers)

add_executable(CWave main.c)

target_include_directories(CWave PUBLIC headers)
link_directories(CWave PRIVATE src)
target_link_libraries(CWave player)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
