cmake_minimum_required(VERSION 3.5)

project(Flight)
set(EXECUTABLE_NAME ${PROJECT_NAME})
set(PROJECT_SOURCE_DIR "../src/")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

set(LIBNOISE_DIR "../deps/libnoise")
set(SFML_DIR "../deps/SFML-2.4.1")

set(CMAKE_BUILD_TYPE Release)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 3)
configure_file(
  "${PROJECT_BINARY_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}/config.h"
  )

if(WIN32)
  link_directories(${SFML_DIR}/lib/Release)
endif()

add_executable(${EXECUTABLE_NAME} ${SOURCES})

if(APPLE)
  add_definitions(-DFLIGHT_MAC)
  target_link_libraries(${EXECUTABLE_NAME} "-framework sfml-window -framework sfml-audio -framework sfml-graphics -framework sfml-system -framework sfml-audio -framework OpenGL ${LIBNOISE_DIR}/src/libnoise.a ${LIBNOISE_DIR}/noiseutils/libnoiseutils-static.a")
elseif(WIN32)
  add_definitions(-DFLIGHT_WINDOWS)
  # FIXME: how to get path to show up in visual studio's "additional library directories"?
  target_link_libraries(${EXECUTABLE_NAME} sfml-graphics sfml-window sfml-system sfml-audio)
elseif(UNIX)
  # TODO: not enough to just assume non-mac will be linux, there's BSD, etc...
  add_definitions(-DFLIGHT_LINUX)
  find_package(Threads)
  target_link_libraries(${EXECUTABLE_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()
