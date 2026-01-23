cmake_minimum_required(VERSION 3.21)
project(raylib_imgui_hybrid LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# -----------------------------
# Dependencies
# -----------------------------

# raylib
add_subdirectory(external/raylib)

# ImGui
add_library(imgui
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    external/imgui
    external/imgui/backends
    external/raylib/src/external/glfw/include
)

add_executable(raylib_imgui_hybrid WIN32
    src/main.cpp
    src/Core/Application.cpp
)

target_include_directories(raylib_imgui_hybrid PRIVATE
    src
    external/raylib/src
)

target_include_directories(raylib_imgui_hybrid PRIVATE
    external/raylib/src/external/glfw/include
    external/glad/include
)

target_link_libraries(raylib_imgui_hybrid PRIVATE
    raylib
    imgui
)

add_compile_definitions(NOMINMAX)


# -----------------------------
# Copy Assets
# -----------------------------
add_custom_command(TARGET raylib_imgui_hybrid POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/src/Core/Roboto-Regular.ttf"
    "$<TARGET_FILE_DIR:raylib_imgui_hybrid>/Roboto-Regular.ttf"
)

# -----------------------------
# Platform-specific
# -----------------------------
if (WIN32)
    target_link_libraries(raylib_imgui_hybrid PRIVATE opengl32)
elseif (UNIX AND NOT APPLE)
    target_link_libraries(raylib_imgui_hybrid PRIVATE GL X11 pthread dl)
elseif (APPLE)
    target_link_libraries(raylib_imgui_hybrid PRIVATE
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
endif()
