cmake_minimum_required(VERSION 3.30)

add_compile_options(-std=gnu++20)

# in my/github/vcpkg install ...
# See README
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_INSTALLED_DIR "${CMAKE_SOURCE_DIR}/../vcpkg/installed" CACHE PATH "vcpkg install dir")

set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic" CACHE STRING "")
set(VCPKG_HOST_TRIPLET "x64-mingw-dynamic" CACHE STRING "")

project(anson.cmake
    VERSION 0.1.0
    DESCRIPTION "The cpp peer of Anson"
    HOMEPAGE_URL "https://github.com/odys-z/anson.cmake"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if (MSVC)
    # deprecated
    add_compile_options(/utf-8)
endif()

if(WIN32)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-Wa,-mbig-obj)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-Xclang -mbig-obj)
    endif()
endif()

# I can't solve the MSVC mojibake problem. You might be better remove any MSVC kit's language packs other than English.
# add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
# if(MSVC)
# This forces MSVC to report errors in a way that avoids some encoding issues
# add_compile_options(/source-charset:utf-8 /execution-charset:utf-8)
# endif()

# Generate compile_commands.json for clangd/IDE integration (single-config only)
if(NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# vcpkg install boost-url
# find_package(Boost CONFIG REQUIRED COMPONENTS url)
find_package(boost_url CONFIG REQUIRED)

 # vcpkg install openssl:x64-mingw-dynamic
find_package(OpenSSL REQUIRED)

find_package(EnTT CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_subdirectory(src)

option(ANSON_BUILD_TESTS "Build tests for Anson.cmake" ON)
if (ANSON_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
