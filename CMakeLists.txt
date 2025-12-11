cmake_minimum_required(VERSION 3.16)

project(chroma VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    sdl3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-3.2.24
)

FetchContent_Declare(
    sdl_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-3.2.4
)

FetchContent_Declare(
    sdl_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG release-3.2.2
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.4-docking
)

FetchContent_Declare(
    lua
    GIT_REPOSITORY https://github.com/lubgr/lua-cmake.git
    GIT_BRANCH master
)

find_package(Freetype QUIET)
if(NOT Freetype_FOUND)
    message(STATUS "Fetching FreeType from source...")
    include(FetchContent)
    FetchContent_Declare(
        freetype
        GIT_REPOSITORY https://github.com/freetype/freetype.git
        GIT_TAG VER-2-14-1
    )
    FetchContent_MakeAvailable(freetype)
    set(FREETYPE_INCLUDE_DIRS ${Freetype_INCLUDE_DIRS} CACHE PATH "")
    set(FREETYPE_LIBRARY ${Freetype_LIBRARIES} CACHE FILEPATH "")
    # set(SDLTTF_FREETYPE_VENDORED ON CACHE BOOL "")
endif()

FetchContent_MakeAvailable(
    sdl3
    sdl_image
    sdl_ttf
    imgui
    lua
)

file(GLOB CPP_SOURCES
    src/*.cpp
    src/window/*.cpp
    src/lua/*.cpp
)

file(GLOB IMGUI_SOURCES "${imgui_SOURCE_DIR}/*cpp")

list(APPEND IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

add_executable(${PROJECT_NAME} ${CPP_SOURCES} ${IMGUI_SOURCES})

find_package(Vulkan QUIET)
if(NOT Vulkan_FOUND)
    message(STATUS "Fetching Vulkan headers...")
    include(FetchContent)
    FetchContent_Declare(
        vulkan_headers
        GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
        GIT_BRANCH main
    )
    FetchContent_MakeAvailable(vulkan_headers)
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${vulkan_headers_SOURCE_DIR}/include
    )
endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
    -g
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${sdl3_SOURCE_DIR}/include
    ${sdl_image_SOURCE_DIR}/include
    ${sdl_ttf_SOURCE_DIR}/include
    ./include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    lua::lib
)
