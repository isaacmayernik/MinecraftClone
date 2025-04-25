#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "cglm/cglm.h"

// Camera structure
typedef struct
{
    vec3 pos;
    vec3 front;
    vec3 up;
    float yaw;
    float pitch;
    float moveSpeed;
} Camera;

Camera camera = {
    .pos = {0.0f, 0.0f, 3.0f},
    .front = {0.0f, 0.0f, -1.0f},
    .up = {0.0f, 1.0, 0.0f},
    .yaw = -90.0f,
    .pitch = 0.0f,
    .moveSpeed = 2.5f};

void updateCameraVectors()
{
    vec3 front;
    front[0] = cos(glm_rad(camera.yaw)) * cos(glm_rad(camera.pitch));
    front[1] = sin(glm_rad(camera.pitch));
    front[2] = sin(glm_rad(camera.yaw)) * cos(glm_rad(camera.pitch));
    glm_normalize_to(front, camera.front);
}

// Function to read shader file
char *readShaderFile(const char *filePath)
{
    FILE *file = fopen(filePath, "rb");

    if (!file)
    {
        fprintf(stderr, "ERROR: Failed to open shader file: %s\n", filePath);
        return NULL;
    }
    fseek(file, 0, SEEK_END);

    long fileSize = ftell(file);
    if (fileSize == -1L)
    {
        fprintf(stderr, "ERROR: Could not determine file size: %s\n", filePath);
        fclose(file);
        return NULL;
    }
    fseek(file, 0, SEEK_SET);

    if (fileSize > SIZE_MAX - 1)
    {
        fprintf(stderr, "ERROR: File too large: %s\n", filePath);
        fclose(file);
        return NULL;
    }
    size_t bufferSize = (size_t)fileSize + 1;
    char *buffer = (char *)malloc(bufferSize);

    if (!buffer)
    {
        fprintf(stderr, "ERROR: Memory allocation failed for shader: %s\n", filePath);
        fclose(file);
        return NULL;
    }

    size_t bytesRead = fread(buffer, 1, (size_t)fileSize, file);
    if (bytesRead != (size_t)fileSize)
    {
        fprintf(stderr, "ERROR: Failed to read complete file: %s\n", filePath);
        free(buffer);
        fclose(file);
        return NULL;
    }
    buffer[fileSize] = '\0';
    fclose(file);
    return buffer;
}

void processInput(GLFWwindow *window, float deltaTime)
{
    float velocity = camera.moveSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        glm_vec3_muladds(camera.front, velocity, camera.pos);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        glm_vec3_muladds(camera.front, -velocity, camera.pos);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        vec3 right;
        glm_cross(camera.front, camera.up, right);
        glm_normalize(right);
        glm_vec3_muladds(right, -velocity, camera.pos);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        vec3 right;
        glm_cross(camera.front, camera.up, right);
        glm_normalize(right);
        glm_vec3_muladds(right, velocity, camera.pos);
    }
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "ERROR: Failed to initialize GLFW\n");
        return -1;
    }

    // Configure OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow *window = glfwCreateWindow(800, 600, "Isaac is doing things", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "ERROR: Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "ERROR: Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Load shaders
    char *vertexShaderSource = readShaderFile("../shaders/shader.vert");
    char *fragmentShaderSource = readShaderFile("../shaders/shader.frag");
    if (!vertexShaderSource || !fragmentShaderSource)
    {
        if (vertexShaderSource)
            free(vertexShaderSource);
        if (fragmentShaderSource)
            free(fragmentShaderSource);
        glfwTerminate();
        return -1;
    }

    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check vertex shader
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR: Vertex shader compilation failed:\n%s\n", infoLog);
        free(vertexShaderSource);
        free(fragmentShaderSource);
        glfwTerminate();
        return -1;
    }

    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Check fragment shader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR: Fragment shader compilation failed:\n%s\n", infoLog);
        free(vertexShaderSource);
        free(fragmentShaderSource);
        glfwTerminate();
        return -1;
    }

    // Link shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check linking
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "ERROR: Shader program linking failed:\n%s\n", infoLog);
        free(vertexShaderSource);
        free(fragmentShaderSource);
        glfwTerminate();
        return -1;
    }

    // Cleanup shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    free(vertexShaderSource);
    free(fragmentShaderSource);

    // Triangle data
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // Left
        0.5f, -0.5f, 0.0f,  // Right
        0.0f, 0.5f, 0.0f    // Top
    };

    // Create buffers
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        processInput(window, deltaTime);
        updateCameraVectors();

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create transformations
        mat4 projection, view, model;
        glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, projection);

        vec3 center;
        glm_vec3_add(camera.pos, camera.front, center);
        glm_lookat(camera.pos, center, camera.up, view);

        glm_mat4_identity(model);

        // Get uniform locations
        glUseProgram(shaderProgram);
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");

        // Pass matrices to shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float *)model);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float *)view);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float *)projection);

        // Render triangle
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}