cmake_minimum_required(VERSION 4.2)
project(ImageViewer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(fmt CONFIG REQUIRED)

file(
        COPY ${CMAKE_CURRENT_SOURCE_DIR}/img
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)
file(
        COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/
)

add_executable(${PROJECT_NAME} src/main.cpp)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror)

target_include_directories(${PROJECT_NAME} PRIVATE ${stb_INCLUDE_DIR})
target_link_libraries(
    ${PROJECT_NAME} PRIVATE OpenGL::GL glfw glad::glad fmt::fmt
)
