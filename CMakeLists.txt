cmake_minimum_required(VERSION 3.17.0)

project(tracy-interface-dev LANGUAGES CXX)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
else()
    message(ERROR "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()

project(tracy-interface VERSION 0.1.0 LANGUAGES CXX)

# Set the version and current build date
set(tracy-interface_VERSION "${PROJECT_VERSION}")
set(tracy-interface_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
string(TIMESTAMP tracy-interface_BUILD_DATE "%Y-%m-%d")
string(TIMESTAMP tracy-interface_BUILD_YEAR "%Y")

# Library options
option(tracy-interface.with_tracy_bin "Also build the Tracy profiler executable" OFF)

message(STATUS "Library ver      : ${tracy-interface_VERSION}")
message(STATUS "Build date       : ${tracy-interface_BUILD_DATE}")
message(STATUS "Build year       : ${tracy-interface_BUILD_YEAR}")
message(STATUS "With Tracy bin   : ${tracy-interface.with_tracy_bin}")
message(STATUS)

find_path(TRACY_PATH "Tracy.hpp" PATH "external/tracy/")
if(NOT EXISTS "${TRACY_PATH}")
    message(FATAL_ERROR "Cannot find Tracy folder; is the external folder properly set up?")
endif()
message(STATUS "Tracy path   : ${TRACY_PATH}")
message(STATUS)


# The source files for the tracy-interface library
set(tracy-interface_sourceFiles
    "src/lib/tracy_interface.cpp"
    "${TRACY_PATH}/TracyClient.cpp"
)

# The tracy-interface library target
add_library(tracy-interface ${tracy-interface_sourceFiles})
set_target_properties(tracy-interface PROPERTIES
                      VERSION "${tracy-interface_VERSION}"
                      SOVERSION "${tracy-interface_VERSION_MAJOR}")
target_include_directories(tracy-interface PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include/>
  $<INSTALL_INTERFACE:include>
)
target_compile_features(tracy-interface PUBLIC cxx_std_17)
target_include_directories(tracy-interface PRIVATE "${TRACY_PATH}")
target_compile_definitions(tracy-interface PUBLIC TRACY_INTERFACE_ENABLE_PROFILING=1)
target_compile_definitions(tracy-interface PRIVATE TRACY_ENABLE=1)


set(TARGETS_TO_INSTALL tracy-interface)

add_library(tracy-interface::tracy-interface ALIAS tracy-interface)



# Build the Tracy profiler if we are told to do so
if(tracy-interface.with_tracy_bin)
    include(ExternalProject)

    find_program(MAKE_EXECUTABLE
        NAMES gmake mingw32-make make
        NAMES_PER_DIR
        DOC "GNU Make")
    cmake_host_system_information(RESULT Ncpu QUERY NUMBER_OF_PHYSICAL_CORES)

    ExternalProject_Add(tracy-profiler
        SOURCE_DIR "${TRACY_PATH}"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ${MAKE_EXECUTABLE} -j${Ncpu} -C "${TRACY_PATH}/profiler/build/unix/" -f release.mk all
        INSTALL_COMMAND install "${TRACY_PATH}/profiler/build/unix/Tracy-release" "<INSTALL_DIR>/Tracy"
        BUILD_BYPRODUCTS "<INSTALL_DIR>/Tracy"  # Ninja needs this
    )
    ExternalProject_Get_Property(tracy-profiler INSTALL_DIR)
    install(PROGRAMS "${INSTALL_DIR}/Tracy" DESTINATION bin)
endif()


# Define installation settings for tracy-interface
install(TARGETS ${TARGETS_TO_INSTALL} EXPORT tracy-interfaceTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)
install(EXPORT tracy-interfaceTargets
        FILE tracy-interfaceTargets.cmake
        NAMESPACE tracy-interface::
        DESTINATION lib/cmake/tracy-interface
)

# Ensure that all the include files are installed
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/include/
        DESTINATION include
        USE_SOURCE_PERMISSIONS
        FILES_MATCHING PATTERN "*.hpp"
)

# Write the tracy-interfaceConfig.cmake and tracy-interfaceConfigVersion.cmake files
include(CMakePackageConfigHelpers)
write_basic_package_version_file("tracy-interfaceConfigVersion.cmake"
        VERSION "${tracy-interface_VERSION}"
        COMPATIBILITY SameMajorVersion
)
configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/tracy-interfaceConfig.cmake
        INSTALL_DESTINATION lib/cmake/tracy-interface
)
install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/tracy-interfaceConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/tracy-interfaceConfigVersion.cmake"
        DESTINATION lib/cmake/tracy-interface
)

# Basic CPack support
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VENDOR "Lucian Radu Teodorescu")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/lucteo/tracy-interface")
include(CPack)
