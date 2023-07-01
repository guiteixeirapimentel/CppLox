
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


add_executable(
    AstVisitor_t
    AstVisitor_t.cpp
)
target_link_libraries(
    AstVisitor_t
    PRIVATE
    GTest::gtest_main
    lox::Lox
)
target_include_directories(
    AstVisitor_t
    PRIVATE
    ../src/
)

gtest_discover_tests(AstVisitor_t)
add_compile_definitions(AstVisitor_t PRIVATE TEST_MODE=1)
