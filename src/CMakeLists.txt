
add_subdirectory(lox)

add_executable(cpplox main.cpp)

target_link_libraries(cpplox PRIVATE lox::Lox)

target_include_directories(cpplox PRIVATE ./)