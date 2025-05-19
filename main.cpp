#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

#define WIDTH 1920
#define HEIGHT 1080

struct Triangle {
    glm::vec2 position;
    glm::vec3 color;
};

std::vector<GLuint> triangleVAOs;
std::vector<Triangle> triangles;

GLuint shaderProgram;
GLuint basicVAO, basicVBO;

const char* vertexShaderSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat4 model;
out vec3 vertColor;
uniform vec3 color;
void main() {
    gl_Position = model * vec4(aPos, 0.0, 1.0);
    vertColor = color;
}
)";

const char* fragmentShaderSrc = R"(
#version 330 core
in vec3 vertColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(vertColor, 1.0);
}
)";

GLuint compileShader(const char* src, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    return shader;
}

GLuint createShaderProgram() {
    GLuint vs = compileShader(vertexShaderSrc, GL_VERTEX_SHADER);
    GLuint fs = compileShader(fragmentShaderSrc, GL_FRAGMENT_SHADER);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2) {
    float vertices[] = {
        x0, y0,
        x1, y1,
        x2, y2
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return vao;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        Triangle t;
        t.position = glm::vec2((float)x, HEIGHT - (float)y);
        t.color = glm::vec3((float)(rand() % 100) / 100.0f,
                            (float)(rand() % 100) / 100.0f,
                            (float)(rand() % 100) / 100.0f);
        triangles.push_back(t);
    }
}

int main() {
    srand((unsigned int)time(NULL));
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Exercicio OpenGL - Triangulos", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();

    shaderProgram = createShaderProgram();
    glUseProgram(shaderProgram);

    triangleVAOs.push_back(createTriangle(-0.8f, -0.6f, -0.6f, -0.6f, -0.7f, -0.4f));
    triangleVAOs.push_back(createTriangle(-0.4f, -0.6f, -0.2f, -0.6f, -0.3f, -0.4f));
    triangleVAOs.push_back(createTriangle(0.0f, -0.6f, 0.2f, -0.6f, 0.1f, -0.4f));
    triangleVAOs.push_back(createTriangle(0.4f, -0.6f, 0.6f, -0.6f, 0.5f, -0.4f));
    triangleVAOs.push_back(createTriangle(-0.2f, 0.0f, 0.0f, 0.0f, -0.1f, 0.2f));

    float baseTriangle[] = {
        -0.1f, -0.1f,
         0.1f, -0.1f,
         0.0f,  0.1f
    };

    glGenVertexArrays(1, &basicVAO);
    glBindVertexArray(basicVAO);

    glGenBuffers(1, &basicVBO);
    glBindBuffer(GL_ARRAY_BUFFER, basicVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(baseTriangle), baseTriangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2, 0.2, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        for (size_t i = 0; i < triangleVAOs.size(); i++) {
            GLuint colorLoc = glGetUniformLocation(shaderProgram, "color");
            glUniform3f(colorLoc, 0.6f, 0.6f, 1.0f);
            GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
            glBindVertexArray(triangleVAOs[i]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glBindVertexArray(basicVAO);
        for (auto& tri : triangles) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(
                (tri.position.x / WIDTH) * 2.0f - 1.0f,
                (tri.position.y / HEIGHT) * 2.0f - 1.0f,
                0.0f));
            GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
            GLuint colorLoc = glGetUniformLocation(shaderProgram, "color");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(colorLoc, tri.color.r, tri.color.g, tri.color.b);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
