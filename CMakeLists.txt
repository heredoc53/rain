cmake_minimum_required(VERSION 3.10)
project(Rain)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)
file(GLOB SOURCES src/*.cpp)

find_package(fmt CONFIG REQUIRED)
find_package(argparse CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(boost_process CONFIG REQUIRED) #Boost REQUIRED COMPONENTS process

add_executable(rain ${SOURCES})

target_compile_options(rain PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror>
)
target_link_libraries(rain PRIVATE 
			Boost::process
			fmt::fmt-header-only
			argparse::argparse
			tomlplusplus::tomlplusplus )
