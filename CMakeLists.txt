cmake_minimum_required(VERSION 3.16)

project(chroma VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Disable SDL subsystems we don't need
set(SDL_AUDIO OFF CACHE BOOL "")
# set(SDL_RENDER OFF CACHE BOOL "")
set(SDL_CAMERA OFF CACHE BOOL "")
# set(SDL_JOYSTICK OFF CACHE BOOL "")
set(SDL_HAPTIC OFF CACHE BOOL "")
set(SDL_HIDAPI OFF CACHE BOOL "")
set(SDL_POWER OFF CACHE BOOL "")
set(SDL_SENSOR OFF CACHE BOOL "")
set(SDL_DIALOG OFF CACHE BOOL "")
set(SDL_TRAY OFF CACHE BOOL "")

set(SDL_TEST_LIBRARY OFF CACHE BOOL "")
set(SDL_TESTS OFF CACHE BOOL "")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(SDL_ASAN ON CACHE BOOL "")
endif()

add_subdirectory(external/SDL)
add_subdirectory(external/SDL_image)

file(GLOB CPP_SOURCES
    src/*.cpp
    src/menu/*.cpp
    src/window/*.cpp
    # src/lua/*.cpp
    # src/lua/imgui/*.cpp
    src/canvas/*.cpp
    src/canvas/cmd/*.cpp
    src/canvas/command/*.cpp
    src/canvas/command/select/*.cpp
)

set(imgui_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/imgui)

file(GLOB IMGUI_SOURCES "${imgui_SOURCE_DIR}/*cpp")

list(APPEND IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
    # ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlgpu3.cpp
    # ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
)

add_executable(${PROJECT_NAME} ${CPP_SOURCES} ${IMGUI_SOURCES})

# find_package(Vulkan REQUIRED)

# if(NOT Vulkan_FOUND)
#     message(STATUS "Fetching Vulkan headers...")

#     include(FetchContent)
#     FetchContent_Declare(
#         vulkan_headers
#         GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
#         GIT_BRANCH main
#     )
#     FetchContent_MakeAvailable(vulkan_headers)
#     target_include_directories(${PROJECT_NAME} PRIVATE
#         ${vulkan_headers_SOURCE_DIR}/include
#     )

#     target_link_libraries(${PROJECT_NAME} PRIVATE
#         Vulkan::Headers
#     )
# endif()

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wextra -Wpedantic
    -g
    -Ofast
)

# Add sanitizer for debug build
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Enabling AddressSanitizer for Debug build")
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-fsanitize=address -fno-omit-frame-pointer>
        $<$<CXX_COMPILER_ID:Clang>:-fsanitize=address -fno-omit-frame-pointer>
        $<$<CXX_COMPILER_ID:MSVC>:/fsanitize=address>
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-fsanitize=address>
        $<$<CXX_COMPILER_ID:Clang>:-fsanitize=address>
        $<$<CXX_COMPILER_ID:MSVC>:/fsanitize=address>
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${sdl3_SOURCE_DIR}/include
    ${sdl_image_SOURCE_DIR}/include
    # ${sdl_ttf_SOURCE_DIR}/include
    # ${lua_SOURCE_DIR}/src
    ./include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    # Vulkan::Vulkan
    SDL3::SDL3
    SDL3_image::SDL3_image
    # SDL3_ttf::SDL3_ttf
    # lua::lib
)
