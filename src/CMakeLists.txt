cmake_minimum_required(VERSION 3.0.0)
project(CWave VERSION 0.1.0)

include(CTest)
enable_testing()


add_library(player channel.c dynarrExperiment.c mixer.c player.c wavedata.c wavStream.c)
target_link_libraries(player libasound.so pthread)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
