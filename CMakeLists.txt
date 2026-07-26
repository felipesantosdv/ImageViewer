cmake_minimum_required(VERSION 4.2)
project(ImageViewer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${stb_INCLUDE_DIR})
target_link_libraries(
    ${PROJECT_NAME} PRIVATE OpenGL::GL glfw glad::glad fmt::fmt
)
