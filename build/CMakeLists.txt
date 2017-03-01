cmake_minimum_required(VERSION 3.5)

project(FLIGHT)
set(EXECUTABLE_NAME ${PROJECT_NAME})
set(PROJECT_SOURCE_DIR "../src/")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

set(LIBNOISE_DIR "../deps/libnoise")
set(SFML_DIR "../deps/SFML-2.4.1")
set(YAML_DIR "../deps/yaml-cpp")

set(CMAKE_BUILD_TYPE Release)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 3)
configure_file(
  "${PROJECT_BINARY_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}/config.h")

include_directories("${SFML_DIR}/include ${YAML_DIR}/include")

if(APPLE)
  add_definitions(-DFLIGHT_MAC)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(${EXECUTABLE_NAME} ${SOURCES})
    target_link_libraries(${EXECUTABLE_NAME} "-framework sfml-window -framework sfml-audio -framework sfml-graphics -framework sfml-system -framework sfml-audio -framework OpenGL ${LIBNOISE_DIR}/src/libnoise.a ${LIBNOISE_DIR}/noiseutils/libnoiseutils-static.a ${YAML_DIR}/libyaml-cpp.a")
  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(${EXECUTABLE_NAME} MACOSX_BUNDLE ${SOURCES})
    target_link_libraries(${EXECUTABLE_NAME} "-framework sfml-window -framework sfml-graphics -framework sfml-system -framework sfml-audio -framework OpenGL -Xlinker -rpath -Xlinker @loader_path/../Frameworks ${LIBNOISE_DIR}/src/libnoise.a ${LIBNOISE_DIR}/noiseutils/libnoiseutils-static.a ${YAML_DIR}/libyaml-cpp.a")
    set(MACOSX_BUNDLE_BUNDLE_NAME ${EXECUTABLE_NAME})
    set(MACOSX_BUNDLE_ICON_FILE icon.icns)
    add_custom_target(pkg ALL mkdir -p ${EXECUTABLE_NAME}.app/Contents/Resources
      COMMAND mkdir -p ${EXECUTABLE_NAME}.app/Contents/Frameworks
      COMMAND cp -r ../res/* ${EXECUTABLE_NAME}.app/Contents/Resources
      COMMAND cp macOS/icon.icns ${EXECUTABLE_NAME}.app/Contents/Resources
      COMMAND cp -r ${SFML_DIR}/lib/* ${EXECUTABLE_NAME}.app/Contents/Frameworks
      COMMAND cp -r ${SFML_DIR}/extlibs/libs-osx/Frameworks/* ${EXECUTABLE_NAME}.app/Contents/Frameworks)
  endif()
elseif(WIN32)
  add_definitions(-DFLIGHT_WINDOWS)
  add_executable(${EXECUTABLE_NAME} WIN32 ${SOURCES})
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    link_directories(${SFML_DIR}/lib/Release)
  elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_directories(${SFML_DIR}/lib/Debug)
  endif()
  # FIXME: how to get path to show up in visual studio's "additional library directories"?
  target_link_libraries(${EXECUTABLE_NAME} sfml-graphics sfml-window sfml-system sfml-audio)
elseif(UNIX)
  # TODO: not enough to just assume non-mac will be linux, there's BSD, etc...
  add_definitions(-DFLIGHT_LINUX)
  find_package(Threads)
  target_link_libraries(${EXECUTABLE_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()
