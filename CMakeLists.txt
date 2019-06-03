cmake_minimum_required (VERSION 3.10)
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_BUILD_TYPE Debug)
set (EXECUTABLE_OUTPUT_PATH "..")

# Project name
project (sb-2019)


set (SOURCE_DIR src)
set (INCLUDE_DIR include)

file (GLOB_RECURSE HEADER_FILES ${INCLUDE_DIR}/*.hpp)
file (GLOB_RECURSE SOURCE_FILES ${SOURCE_DIR}/*.cpp)
# Executable name
add_executable (sb-2019 ${DOMAINS_SOURCE_FILES}
                            ${SOURCE_FILES}
)
target_include_directories(sb-2019 PUBLIC ${INCLUDE_DIR})
