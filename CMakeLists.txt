cmake_minimum_required(VERSION 3.22.1)

project(perlin_noise)

file(GLOB_RECURSE SOURCES src/*.cpp include/*.h)
file(GLOB_RECURSE RESOURCES resources/fonts/*)

set(SFML_STATIC_LIBRARIES TRUE)

#Define path to your SFML root here (for example, C:/libs/SFML-2.5.0)
set(SFML_ROOT "path/to/your/SFML/root/folder")

set(SFML_DIR "${SFML_ROOT}/lib/cmake/SFML")
set(EXECUTABLE_NAME "perlin_noise")
add_executable(${EXECUTABLE_NAME} ${SOURCES} ${RESOURCES})

target_compile_options(${EXECUTABLE_NAME} PUBLIC -std=c++14 -Wall -g -O3)

file(COPY ${RESOURCES} DESTINATION resources/fonts)

include_directories(${PROJECT_SOURCE_DIR}/include)

find_package(SFML 2.6.1 REQUIRED network audio graphics window system)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} sfml-graphics sfml-window sfml-system)
endif()

