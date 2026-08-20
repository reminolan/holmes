# -- CMakeSetup ----------------------------------------------------------------
cmake_minimum_required(VERSION 4.3.3)

# -- ProjectSetup --------------------------------------------------------------
project(holmes)

set(BUILD_NAME "dev"
    CACHE STRING
    "The name of the build (i.e. 'Steam', 'dev-remi')")

find_package(Git QUIET)
if(GIT_FOUND)
   execute_process(COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h
                   OUTPUT_VARIABLE BUILD_VERSION
                   OUTPUT_STRIP_TRAILING_WHITESPACE
                   ERROR_QUIET)
else()
   set(BUILD_VERISON "invalid version")
endif()

if((${CMAKE_BUILD_TYPE} STREQUAL "Debug") OR
   (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebugInfo"))
  set(BUILD_INTERNAL ON)
else()
  set(BUILD_INTERNAL OFF)
endif()

# -- Libraries -----------------------------------------------------------------
set(VENDOR_PATH ${PROJECT_SOURCE_DIR}/vendor)

if(EXISTS ${VENDOR_PATH}/SDL/CMakeLists.txt)
   add_subdirectory(${VENDOR_PATH}/SDL EXCLUDE_FROM_ALL)
else()
   find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

# -- C Options -----------------------------------------------------------------
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# -- Modules -------------------------------------------------------------------
set(SOURCE_PATH ${PROJECT_SOURCE_DIR}/source)
set(HEADER_PATH ${PROJECT_SOURCE_DIR}/header)

configure_file(${HEADER_PATH}/cmake.h.in ${HEADER_PATH}/cmake.h
               NEWLINE_STYLE UNIX)

set(SOURCES
  ${SOURCE_PATH}/draw.c

  ${SOURCE_PATH}/testbed.h
  ${SOURCE_PATH}/testbed.c

  ${SOURCE_PATH}/main.c)

set(HEADERS
  ${HEADER_PATH}/draw.h
  ${HEADER_PATH}/holmes.h)

add_executable(holmes WIN32 ${SOURCES} ${HEADERS})

target_include_directories(holmes PRIVATE ${HEADER_PATH})
target_link_libraries(holmes PRIVATE SDL3::SDL3)

# -- Warnings as Errors & SSE4.1 -----------------------------------------------
if(MSVC)
  target_compile_options(holmes PRIVATE /W4 /arch:SSE4.1)
else()
  target_compile_options(holmes PRIVATE -Wall -Werror -Wpedantic -msse4.1)
endif()

