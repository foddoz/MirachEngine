# Define options and cache variables.

# Build options.
option(MIRACH_BUILD_GAME "Build the game in MIRACH_GAME_DIR." ON)
option(MIRACH_BUILD_INTERPRETER "Build the Mirach engine interpreter. NOTE: NOT IMPLEMENTED." OFF)
option(MIRACH_BUILD_COMPILER "Build the Mirach engine compiler. NOTE: NOT IMPLEMENTED." OFF)
set(MIRACH_BUILD_DIR "${CMAKE_BINARY_DIR}" CACHE STRING "The directory to output executables.")

# Game build options.
set(MIRACH_GAME_DIR "${CMAKE_SOURCE_DIR}/gameFiles" CACHE STRING "Directory of the game files.")
set(MIRACH_GAME_EXTERN_FOLDER "." CACHE STRING "Name of the folder to put external game files, relative to the executable.")

option(MIRACH_GAME_EMBED_DEFAULT "Embed un-tagged assets by default." OFF)

option(MIRACH_GAME_EMBED_ALL "Embed all game files into the game executable. Overwrites import tags." OFF)
option(MIRACH_GAME_EXTERN_ALL "Copy all game files into the executable directory. Overwrites import tags." OFF)

# Turn off EMBED_ALL and COPY_ALL if game is not built.
if($<NOT:${MIRACH_BUILD_GAME}>)
    set(MIRACH_GAME_EMBED_ALL OFF)
    set(MIRACH_GAME_EXTERN_ALL OFF)

    # Default to EMBED_ALL if both are ON.
elseif(${MIRACH_GAME_EMBED_ALL})
    set(MIRACH_GAME_EXTERN_ALL OFF)

endif()