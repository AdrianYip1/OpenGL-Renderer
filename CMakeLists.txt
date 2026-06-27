cmake_minimum_required(VERSION 3.20)
project(glsl-shaders)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(libraries/enginemath)

include(FetchContent)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.4.3
)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(assimp)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.90.4
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC glfw)

add_executable(glsl-shaders
    libraries/include/shader/shader.cpp
    libraries/include/mesh/mesh.cpp
    libraries/include/model/model.cpp
    libraries/include/render/render.cpp
    libraries/include/camera/camera.cpp
    stb_image.cpp
    shaderShowcase.cpp
    glad.c
)

file(COPY textures DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shaders  DESTINATION ${CMAKE_BINARY_DIR})
file(COPY models DESTINATION ${CMAKE_BINARY_DIR})

target_include_directories(glsl-shaders PRIVATE
    ${CMAKE_SOURCE_DIR}/libraries/include
    ${CMAKE_SOURCE_DIR}
)

# Build the gl-profiler submodule. Its header includes <glad/glad.h>, so point it
# at our glad loader's include directory.
set(GLP_GL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/libraries/include CACHE STRING "" FORCE)
add_subdirectory(external/gl-profiler)

if (WIN32)
    target_link_libraries(glsl-shaders PRIVATE
        opengl32
        ${CMAKE_SOURCE_DIR}/libraries/lib/glfw3.lib
        enginemath::enginemath
        assimp::assimp
        imgui
        glp::profiler
    )
elseif(UNIX)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)

    target_link_libraries(glsl-shaders PRIVATE
        OpenGL::GL
        glfw
        enginemath::enginemath
        assimp::assimp
        imgui
        glp::profiler
    )
endif()