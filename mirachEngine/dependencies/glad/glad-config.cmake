# Add Glad v2.0.8.
# OpenGL v3.3 Core.
# See: https://github.com/Dav1dde/glad.

add_library(glad STATIC "${CMAKE_CURRENT_LIST_DIR}/src/gl.c")
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")