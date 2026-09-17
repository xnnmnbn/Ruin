cmake_minimum_required(VERSION 3.16)

project(Ruin C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Bağımlılıkları Bul
find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED COMPONENTS SDL3-static)

# 2. Kaynak Dosyaları Tara
file(GLOB_RECURSE RUINENGINE_SOURCES CONFIGURE_DEPENDS
    src/*.c
    external/*.c
    external/*.cpp
)

# 3. Ruin DLL Hedefini Oluştur (Eksik Olan Kısım)
add_library(Ruin SHARED ${RUINENGINE_SOURCES})

set_source_files_properties(external/ext_impls.cpp PROPERTIES LANGUAGE CXX)

# DLL Export Tanımı
target_compile_definitions(Ruin PRIVATE RUIN_BUILD_DLL)

# Include Dizinleri
target_include_directories(Ruin
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external
)

# 4. SDL3 Statik Kütüphanesini DLL İçine Paketleme
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_link_libraries(Ruin
        PRIVATE
            -Wl,--whole-archive
            SDL3::SDL3-static
            -Wl,--no-whole-archive
            Vulkan::Vulkan
    )
else()
    # MSVC (Visual Studio) Kullanılıyorsa
    target_link_libraries(Ruin
        PRIVATE
            SDL3::SDL3-static
            Vulkan::Vulkan
    )
endif()

# 5. Test Executable Hedefi
add_executable(ruin_test
    test/main.c
)

target_link_libraries(ruin_test
    PRIVATE
        Ruin
)
