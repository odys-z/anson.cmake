
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
  t01_common.cpp
  t02_enttmeta.cpp
  t03_deserialize.cpp
  t04_java_enum.cpp
  t04_java_enum.h
  t05_ansonresp.cpp
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

add_compile_definitions(ANSON_VERBOSE=3)
