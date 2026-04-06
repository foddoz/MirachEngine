#Add mirach library target.

# Set mirach directories.
set (MIRACH_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set (MIRACH_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set (MIRACH_EXTERN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies")
set (MIRACH_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Add mirach sources.
add_library(mirach
        "${MIRACH_SOURCE_DIR}/assets.cpp"
        "${MIRACH_SOURCE_DIR}/configuration.cpp")

# Include directories.
target_include_directories(mirach
        PUBLIC
            "${MIRACH_INCLUDE_DIR}"
            "${MIRACH_GENERATED_DIR}"
            "${MIRACH_SOURCE_DIR}"
        PRIVATE
            #"${MIRACH_SOURCE_DIR}"
)

# Set up external libraries/dependencies.
include("${MIRACH_EXTERN_DIR}/cpm/cpm-config.cmake") # Install CPM.
include("${MIRACH_EXTERN_DIR}/lua/lua-config.cmake") # Target: lua.
include("${MIRACH_EXTERN_DIR}/sol/sol-config.cmake") # Target: sol2.
include("${MIRACH_EXTERN_DIR}/glad/glad-config.cmake") # Target: glad.
include("${MIRACH_EXTERN_DIR}/sdl/sdl-config.cmake") # Target: SDL3::SDL3-static.
include("${MIRACH_EXTERN_DIR}/imgui/imgui-config.cmake") # Adds to target mirach directly.

# Link libraries to mirach_lib.
target_link_libraries(mirach PUBLIC
        lua
        sol2
        glad
        SDL3::SDL3-static
)