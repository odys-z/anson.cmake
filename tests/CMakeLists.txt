
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(unit_tests
  semantier.h
  echoreq.h
  # t01_common.cpp
  # t02_entt_basic.cpp
  # t03_loadsettings.cpp
  # t04_entt_registry.cpp
  t05_deserialize.cpp
  # t06_java_enum.cpp
  # t06_java_enum.h
  # t05_ansonresp.cpp
)

add_custom_command(TARGET unit_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/t03-settings.json"
        "$<TARGET_FILE_DIR:unit_tests>/t03-settings.json"

    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/t03_echo.body.json"
        "$<TARGET_FILE_DIR:unit_tests>/t03_echo.body.json"

    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/ast"
        "$<TARGET_FILE_DIR:unit_tests>/ast"

    COMMENT "Copying JSON settings to build directory"
)

find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(unit_tests
    PRIVATE
        gtest_main
        Boost::url
        nlohmann_json::nlohmann_json
        anson
)

add_dependencies(unit_tests anson)

add_compile_definitions(ANSON_VERBOSE=5)
