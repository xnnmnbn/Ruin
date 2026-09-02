cmake_minimum_required(VERSION 3.16)

project(Ruin C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED COMPONENTS SDL3-static)

file(GLOB_RECURSE RUINENGINE_SOURCES CONFIGURE_DEPENDS
    src/*.c
    src/*.cpp
)


set_source_files_properties(src/ri_renderer/vma.cpp PROPERTIES LANGUAGE CXX)

add_library(Ruin SHARED 
    ${RUINENGINE_SOURCES}
    src/ruin__internal.h
    external/cvec.h
)

target_compile_definitions(Ruin PRIVATE RUIN_BUILD_DLL)

target_include_directories(Ruin
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external
)

target_link_libraries(Ruin
    PRIVATE
        SDL3::SDL3-static   # Statically bundles SDL3 into Ruin.dll
        Vulkan::Vulkan      # Vulkan runtime loader
)

target_compile_options(Ruin
    PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer>
)

target_link_options(Ruin
    PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address>
)

add_executable(ruin_test
    test/main.c
)

target_link_libraries(ruin_test
    PRIVATE
        Ruin
)

target_compile_options(ruin_test
    PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer>
)

target_link_options(ruin_test
    PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address>
)