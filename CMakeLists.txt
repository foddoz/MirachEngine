# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
set (CMAKE_CXX_STANDARD 20)
project (ICT397_Project)

# Build options.
set (MIRACH_BUILD_GAME ON)
add_subdirectory ("mirachEngine")

