cmake_minimum_required(VERSION 3.24)

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
set_target_properties(tracy-interface PROPERTIES PUBLIC_HEADER "src/include/tracy_interface.hpp")


# Build the Tracy profiler if we are told to do so
if(tracy-interface.with_tracy_bin)
    include(ExternalProject)

    set(TRACY_ARCH "x86_64")

    find_program(MAKE_EXECUTABLE
        NAMES gmake mingw32-make make
        NAMES_PER_DIR
        DOC "GNU Make")
    cmake_host_system_information(RESULT Ncpu QUERY NUMBER_OF_PHYSICAL_CORES)

    ExternalProject_Add(tracy-profiler
        SOURCE_DIR "${TRACY_PATH}"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ${MAKE_EXECUTABLE} ARCH="${TRACY_ARCH}" -j${Ncpu} -C "${TRACY_PATH}/profiler/build/unix/" -f release.mk all
        INSTALL_COMMAND install "${TRACY_PATH}/profiler/build/unix/Tracy-release" "<INSTALL_DIR>/Tracy"
        BUILD_BYPRODUCTS "<INSTALL_DIR>/Tracy"  # Ninja needs this
    )
    ExternalProject_Get_Property(tracy-profiler INSTALL_DIR)
    install(PROGRAMS "${INSTALL_DIR}/Tracy" DESTINATION bin)
endif()

install(TARGETS tracy-interface)

