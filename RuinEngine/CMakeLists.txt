cmake_minimum_required(VERSION 3.16)

project(Ruin C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Enable C++17 for VMA and modern C++ files
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------------------
# 1. Dependencies
# ------------------------------------------------------------------------------
find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED COMPONENTS SDL3-static)

# ------------------------------------------------------------------------------
# 2. Source Discovery & File Properties
# ------------------------------------------------------------------------------
file(GLOB_RECURSE RUINENGINE_SOURCES CONFIGURE_DEPENDS
    src/*.c
    src/*.cpp
)

# Tell CMake explicitly to handle VMA as C++
set_source_files_properties(src/ri_renderer/vma.cpp PROPERTIES LANGUAGE CXX)

# ------------------------------------------------------------------------------
# 3. Engine Shared Library Target (DLL)
# ------------------------------------------------------------------------------
add_library(Ruin SHARED 
    ${RUINENGINE_SOURCES}
    src/ruin__internal.h
    external/cvec.h
)

# Macro used by ruin.h to trigger __declspec(dllexport) when building the DLL
target_compile_definitions(Ruin PRIVATE RUIN_BUILD_DLL)

target_include_directories(Ruin
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external
)

# Link dependencies directly into the Ruin library
target_link_libraries(Ruin
    PRIVATE
        SDL3::SDL3-static   # Statically bundles SDL3 into Ruin.dll
        Vulkan::Vulkan      # Vulkan runtime loader
)

# Enable AddressSanitizer on Debug builds for the engine
target_compile_options(Ruin
    PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer>
)

target_link_options(Ruin
    PRIVATE
        $<$<CONFIG:Debug>:-fsanitize=address>
)

# ------------------------------------------------------------------------------
# 4. Test Executable Target
# ------------------------------------------------------------------------------
add_executable(ruin_test
    test/main.c
)

# Executable links ONLY against the Ruin engine library
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