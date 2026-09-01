cmake_minimum_required(VERSION 4.4)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

project(Test
	VERSION 1.0
	LANGUAGES CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "..")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.h")

add_executable(out ${SOURCES} ${HEADERS})

target_include_directories(out PRIVATE src)