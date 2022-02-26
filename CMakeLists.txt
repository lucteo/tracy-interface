cmake_minimum_required(VERSION 3.17.0)

project(tracy-interface-dev LANGUAGES CXX)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
else()
    message(ERROR "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

# Set the version of the project
project(tracy-interface-dev VERSION "${CONAN_PACKAGE_VERSION}")

# Print CMake configuration
message(STATUS "System           : ${CMAKE_SYSTEM}")
message(STATUS "System name      : ${CMAKE_SYSTEM_NAME}")
message(STATUS "System ver       : ${CMAKE_SYSTEM_VERSION}")
message(STATUS)

# The main tracy-interface library
add_subdirectory(src)

# Basic CPack support
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VENDOR "Lucian Radu Teodorescu")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/lucteo/tracy-interface")
include(CPack)
