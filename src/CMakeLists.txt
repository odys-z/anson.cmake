# Anson.cmake source, pure header files

cmake_minimum_required(VERSION 3.30)
# project(anson LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(anson INTERFACE
    io/odysz/anson.h
    io/odysz/jprotocol.h
    io/odysz/json.h
    io/odysz/common.h
    # io/odysz/anserializer.h
)

add_library(anson::anson ALIAS anson)

# 1. Register the Header Set
target_sources(anson INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        io/odysz/anson.h
        io/odysz/jprotocol.h
        io/odysz/json.h
        # io/odysz/json.hpp
        io/odysz/common.h
        # io/odysz/serializer.h
)

target_include_directories(anson INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<INSTALL_INTERFACE:include>
)


target_link_libraries(anson INTERFACE
    Boost::url
    EnTT::EnTT
    nlohmann_json::nlohmann_json
)

# In the user project:
# set(ANSON_ENABLE_LOGGING ON)
# set(ANSON_LOG_LEVEL 5) # Set to VERBOSE
# default 2 (warn)
option(ANSON_ENABLE_LOGGING "Enable Anson library verbose logging" ON)

if(ANSON_ENABLE_LOGGING)
    set(ANSON_LOG_LEVEL 2 CACHE STRING "Set Anson log verbosity level (1-5)")
    target_compile_definitions(anson INTERFACE ANSON_VERBOSE=${ANSON_LOG_LEVEL})
endif()

# clangd helper
if(ANSON_BUILD_TESTS)
    add_executable(anson_clangd_helper dev/ping.cpp)
    target_link_libraries(anson_clangd_helper PRIVATE anson)
endif()
