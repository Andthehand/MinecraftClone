cmake_minimum_required(VERSION 3.10)
project(ImGui)

# ImGui source files
set(IMGUI_SRC
    imconfig.h
    imgui.h
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_internal.h
    imgui_tables.cpp
    imgui_widgets.cpp
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
)

# Add backend files for GLFW + OpenGL3
set(IMGUI_BACKEND_SRC
    backends/imgui_impl_glfw.cpp
    backends/imgui_impl_glfw.h
    backends/imgui_impl_opengl3.cpp
    backends/imgui_impl_opengl3.h
    backends/imgui_impl_opengl3_loader.h
)

# Create ImGui static library
add_library(imgui STATIC ${IMGUI_SRC} ${IMGUI_BACKEND_SRC})

target_link_libraries(imgui PRIVATE glfw)

# Include directories
target_include_directories(imgui PUBLIC . ./backends)
