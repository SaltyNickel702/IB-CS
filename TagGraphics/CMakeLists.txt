cmake_minimum_required(VERSION 4.4)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

set(EXE_NAME "out")

project(
	GLFW_APP
	VERSION 1.0
	LANGUAGES C CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "..")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

include_directories(include)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE RUNTIME_COMP CONFIGURE_DEPENDS "libraries/compile/*.cpp" "libraries/compile/*.c")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.h")

add_executable(${EXE_NAME} ${SOURCES} ${RUNTIME_COMP} ${HEADERS})

target_include_directories(${EXE_NAME} PRIVATE "include")

if(WIN32)
    target_link_libraries(${EXE_NAME} PRIVATE glfw opengl32 gdi32 user32 shell32)
	target_link_directories(${EXE_NAME} PRIVATE glm::glm)
endif()