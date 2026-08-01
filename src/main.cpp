#include <cstdint>
#include <cstdlib>

#include <array>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <fmt/color.h>
#include <fmt/core.h>

#include "glad/glad.h"

#include "GLFW/glfw3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, GLsizei width, GLsizei height) {
    glViewport(0, 0, width, height);
}

void proccess_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

std::filesystem::path exe_dir(char *src_file) {
    return std::filesystem::absolute(src_file).parent_path();
}

GLuint compile_shader(GLenum type, const char *source) {
    std::ifstream file;
    std::stringstream buffered_lines;
    std::string line;

    file.open(source);
    if (!file.is_open()) {
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "ERROR: ");
        fmt::println("Could not open file: {}\n", source);
        return 0;
    }
    while (std::getline(file, line)) {
        buffered_lines << line << '\n';
    }

    std::string str_shader_source{buffered_lines.str()};
    const char *shader_source{str_shader_source.c_str()};
    buffered_lines.str("");
    file.close();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shader_source, nullptr);
    glCompileShader(shader);

    GLint success{};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        std::array<char, 512> info_log{};
        glGetShaderInfoLog(shader, 512, nullptr, info_log.data());
        fmt::print(fg(fmt::color::gold), "WARN: ");
        fmt::println("{}", info_log.data());
    }

    return shader;
}

GLuint create_shader_program(const char *vertexSrc, const char *fragmentSrc) {
    GLuint vertexShader = compile_shader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fragmentShader = compile_shader(GL_FRAGMENT_SHADER, fragmentSrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success{};
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        std::array<char, 512> info_log{};
        glGetProgramInfoLog(program, 512, nullptr, info_log.data());
        fmt::print(fg(fmt::color::gold), "WARN: ");
        fmt::println("{}", info_log.data());
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

int main(int argc, char *argv[]) {

    constexpr std::int32_t WIDTH{800};
    constexpr std::int32_t HEIGHT{600};
    constexpr std::string_view TITLE{"Image Viewer"};

    if (argc != 2) {
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "ERROR: ");
        fmt::println("Number of arguments not supported");
        return EXIT_FAILURE;
    }

    stbi_set_flip_vertically_on_load(true);

    std::int32_t img_width{}, img_height{}, img_channels{};
    std::uint8_t *data{stbi_load(argv[1], &img_width, &img_height, &img_channels, 4)};

    if (!data) {
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "ERROR: ");
        fmt::println(stbi_failure_reason());
        return EXIT_FAILURE;
    }

    if (!glfwInit()) {
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "ERROR: ");
        fmt::println("Failed to initialize GLFW");
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window{glfwCreateWindow(WIDTH, HEIGHT, TITLE.data(), nullptr, nullptr)};
    if (window == nullptr) {
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "ERROR: ");
        fmt::println("Could not open GLFW Window");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "ERROR: ");
        fmt::println("Could not initialize GLAD");
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    constexpr std::array<float, 16> VERTICES{
        // pos          // texcoord
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 1.0f};
    constexpr std::array<std::uint32_t, 6> INDICES{0, 1, 2, 2, 3, 0};

    std::filesystem::path base_path{exe_dir(argv[0])};
    GLuint shader_program{create_shader_program(
        (base_path / "shaders" / "vertex.glsl").string().c_str(),
        (base_path / "shaders" / "fragment.glsl").string().c_str())};

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, VERTICES.size() * sizeof(float), VERTICES.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDICES.size() * sizeof(std::uint32_t), INDICES.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

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
    glBindTexture(GL_TEXTURE_2D, 0);

    glfwSwapInterval(1);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        proccess_input(window);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shader_program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteTextures(1, &texture);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
