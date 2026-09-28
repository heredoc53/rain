cmake_minimum_required(VERSION 3.10)
project(Foo)

add_library(foo src/foo.cpp)
target_include_directories(foo PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS foo
    LIBRARY DESTINATION install/lib
    ARCHIVE DESTINATION install/lib
    RUNTIME DESTINATION install/bin
)

install(DIRECTORY include/ DESTINATION install/include)
