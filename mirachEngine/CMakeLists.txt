# CMakeLists.txt : Compiles MirachEngine as a library.
# An internet connection is required when running for the first time.
# This project downloads all the necessary dependencies as defined in the dependencies folder.
cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)

project(mirach VERSION 0.0.0 DESCRIPTION "A Lua game engine.")

# Help CMake to find the installed library on Windows
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}\\dependencies\\ReactPhysics3D")

# Import the ReactPhysics3D library 
find_package(ReactPhysics3D REQUIRED)

# Define mirach options and cache variables.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/mirach-options.cmake")

# Create mirach library.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/mirach-config.cmake") # Target: mirach

# Build executables.
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/mirach-build.cmake")

#Copy shaders folder to the executable path
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders/ DESTINATION ${CMAKE_BINARY_DIR})
#Copy mesh files to the executable path
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../gameFiles/ DESTINATION ${CMAKE_BINARY_DIR})

include(FetchContent)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8 # or latest version
)
FetchContent_MakeAvailable(glm)
# Link glm to target:
target_link_libraries(mirach PUBLIC glm)

FetchContent_Declare(
	assimp
	GIT_REPOSITORY https://github.com/assimp/assimp.git
	GIT_TAG master
)
FetchContent_MakeAvailable(assimp)
target_link_libraries(mirach PUBLIC assimp)

# Link with the ReactPhysics3D library
target_link_libraries(mirach PUBLIC ReactPhysics3D::ReactPhysics3D)


