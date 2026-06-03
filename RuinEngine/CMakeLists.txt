cmake_minimum_required(VERSION 3.16)

project(Ruin C CXX)

set_source_files_properties(src/ri_renderer/vma.cpp PROPERTIES LANGUAGE CXX)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(SDL3   REQUIRED)

# Collect source files
file(GLOB_RECURSE RUINENGINE_SOURCES CONFIGURE_DEPENDS
        src/*.c
        src/*.cpp
)

# Create library (private implementation)
add_library(Ruin ${RUINENGINE_SOURCES}
        src/ruin__internal.h
        external/cvec.h
)

# Public include directory
target_include_directories(Ruin
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/external
)

# ---- Test executable ----
add_executable(ruin
        test/main.c
)

# Link library to test executable
target_link_libraries(ruin
        PRIVATE
        Ruin
        SDL3::SDL3
        Vulkan::Vulkan
)


target_compile_options(ruin
        PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer>
)

target_link_options(ruin
        PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address>
)


target_link_options(ruin PRIVATE -static)

