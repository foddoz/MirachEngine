# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.28.1)

# Reject any attempt to use a toolchain file. We must not use one because
# we could be downloading it here. If the CMAKE_TOOLCHAIN_FILE environment
# variable is set, the cache variable will have been initialized from it.
unset(CMAKE_TOOLCHAIN_FILE CACHE)
unset(ENV{CMAKE_TOOLCHAIN_FILE})

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(assimp-populate NONE)



include(ExternalProject)
ExternalProject_Add(assimp-populate
                     "UPDATE_DISCONNECTED" "False" "DOWNLOAD_EXTRACT_TIMESTAMP" "FALSE" "EXTERNALPROJECT_INTERNAL_ARGUMENT_SEPARATOR" "URL" "https://github.com/assimp/assimp/archive/refs/tags/v5.4.3.tar.gz" "GIT_SHALLOW" "TRUE"
                    SOURCE_DIR          "/download/assimp/248954a90c949291c541ba7ee69b327d01e5f212/assimp"
                    BINARY_DIR          "C:/Users/Liam/Documents/ICT397/Project/ICT397-Project/cmake-build-debug/_deps/assimp-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
                    USES_TERMINAL_PATCH     YES
)


