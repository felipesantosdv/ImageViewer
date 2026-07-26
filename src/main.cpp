#include <cstdint>
#include <cstdlib>

#include <string>

#include <fmt/base.h>

#include "glad/glad.h"

#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

constexpr std::int32_t WIDTH{800};
constexpr std::int32_t HEIGHT{800};
const std::string TITLE{"Viewer"};

void framebuffer_size_callback(GLFWwindow *window, GLsizei width, GLsizei height) {
    glViewport(0, 0, width, height);
}

void proccess_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fmt::report_error("Number of arguments not supported");
        return EXIT_FAILURE;
    }

    std::int32_t img_width{}, img_height{}, img_channels{};
    std::uint8_t *data{stbi_load(argv[1], &img_width, &img_height, &img_channels, 4)};
    const char *failure_reason{stbi_failure_reason()};
    if (!failure_reason) {
        fmt::report_error(failure_reason);
        return EXIT_FAILURE;
    }

    if (!glfwInit()) {
        fmt::report_error("Failed to initialize GLFW\n");
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window{glfwCreateWindow(WIDTH, HEIGHT, TITLE.c_str(), nullptr, nullptr)};
    if (window == nullptr) {
        fmt::report_error("Could not open GLFW Window\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fmt::report_error("Could not initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    GLuint texture{};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    while (!glfwWindowShouldClose(window)) {
        proccess_input(window);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
