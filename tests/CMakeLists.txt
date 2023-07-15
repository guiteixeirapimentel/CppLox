
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

function (add_test name_test)

    add_executable(
        ${name_test}
        ${name_test}.cpp
    )
    target_link_libraries(
        ${name_test}
        PRIVATE
        GTest::gtest_main
        lox::Lox
    )
    target_include_directories(
        ${name_test}
        PRIVATE
        ../src/
    )

    gtest_discover_tests(${name_test})
    add_compile_definitions(${name_test} PRIVATE TEST_MODE=1)
endfunction()

add_test(AstVisitor_t)
add_test(IntegrationTests_t)