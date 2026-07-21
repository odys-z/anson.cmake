
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(unit_tests
  # echoreq.expect.h # Keep this as a testing stepstone. (All codes can be generated)
  test_common.cpp

  # t01_aeshelper.cpp
  # t01_common.cpp
  # t01_typs_java_vs.cpp
  # t02_type_java_tests.h
  # t02_type_java_tests.cpp
  # t02_entt_basic.h
  # t02_entt_basic.cpp
  # t02_entt_basic_serialize.cpp
  # t03_loadsettings.cpp
  # t04_entt_registry.cpp
  # t05_deserialize.cpp
  # t06_java_enum.h
  # t06_java_enum.cpp
  # t07_ansonresp.cpp
  # t08_resultset.cpp
  # t08_resultset.h
  # t09_expect_semantier.cpp
  # test.common.h
  # expect/t_01_echomsg.hpp
  # expect/t_02_semantier.hpp
  # expect/t_10_doctier.hpp
  # expect/wsport_tobe_generated.hpp
  # t10_design_patterns.cpp
  t11_docstier.cpp
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

target_link_libraries(unit_tests
    PRIVATE
        gtest_main
        Boost::url
        EnTT::EnTT
        nlohmann_json::nlohmann_json
        OpenSSL::Crypto OpenSSL::SSL
        anson
)

add_dependencies(unit_tests anson)

add_compile_definitions(ANSON_VERBOSE=4)
