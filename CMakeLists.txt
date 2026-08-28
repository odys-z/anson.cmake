cmake_minimum_required(VERSION 3.30)

add_compile_options(-std=gnu++20)

# vcpkg must be checked out as a sibling directory: ../vcpkg. See README.
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_INSTALLED_DIR "${CMAKE_SOURCE_DIR}/../vcpkg/installed" CACHE PATH "vcpkg install dir")

# Pick a sane default triplet per-platform. Override on the command line with
# -DVCPKG_TARGET_TRIPLET=... / -DVCPKG_HOST_TRIPLET=... if you need something else
# (e.g. cross-compiling Windows binaries from Linux via MinGW).
if(NOT DEFINED VCPKG_TARGET_TRIPLET)
    if(WIN32)
        set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic" CACHE STRING "")
    elseif(APPLE)
        set(VCPKG_TARGET_TRIPLET "x64-osx" CACHE STRING "")
    else()
        set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "")
    endif()
endif()

if(NOT DEFINED VCPKG_HOST_TRIPLET)
    set(VCPKG_HOST_TRIPLET "${VCPKG_TARGET_TRIPLET}" CACHE STRING "")
endif()

project(anson.cmake
    VERSION 0.1.3
    DESCRIPTION "The cpp peer of Anson"
    HOMEPAGE_URL "https://github.com/odys-z/anson.cmake"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if (MSVC)
    add_compile_options(/utf-8)
endif()

if(WIN32)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-Wa,-mbig-obj)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-Xclang -mbig-obj)
    endif()
endif()

# Generate compile_commands.json for clangd/IDE integration (single-config only)
if(NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# vcpkg install boost-url
find_package(boost_url CONFIG REQUIRED)

# vcpkg install openssl
find_package(OpenSSL REQUIRED)

# vcpkg install entt
find_package(EnTT CONFIG REQUIRED)

# vcpkg install nlohmann-json
find_package(nlohmann_json CONFIG REQUIRED)

add_subdirectory(src)

option(ANSON_BUILD_TESTS "Build tests for Anson.cmake" ON)
if (ANSON_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
