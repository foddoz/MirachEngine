set (imgui_SOURCE_FILES
            "${MIRACH_EXTERN_DIR}/imgui/imgui.cpp"
            "${MIRACH_EXTERN_DIR}/imgui/imgui_demo.cpp"
            "${MIRACH_EXTERN_DIR}/imgui/imgui_draw.cpp"
            "${MIRACH_EXTERN_DIR}/imgui/imgui_tables.cpp"
            "${MIRACH_EXTERN_DIR}/imgui/imgui_widgets.cpp"
            "${MIRACH_EXTERN_DIR}/imgui/imgui_stdlib.cpp"
            "${MIRACH_EXTERN_DIR}/imgui/backends/imgui_impl_sdl3.cpp"
            "${MIRACH_EXTERN_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
)

target_sources(mirach PUBLIC ${imgui_SOURCE_FILES})

target_include_directories(mirach PUBLIC
        "${MIRACH_EXTERN_DIR}/imgui/"
        "${MIRACH_EXTERN_DIR}/imgui/backends"
)