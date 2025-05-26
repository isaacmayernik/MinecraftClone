#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "cglm/cglm.h"
#include "camera.h"
#include <stdbool.h>

// Global vars
Camera camera;
float lastX = 400, lastY = 300;
bool firstMouse = true;

// Mouse callback
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)(xpos - lastX);
    float yoffset = (float)(lastY - ypos); // Reversed since y-coordinates range from bottom to top
    lastX = (float)xpos;
    lastY = (float)ypos;

    processMouseMovement(&camera, xoffset, yoffset, GL_TRUE);
}

// Scroll callback
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    processMouseScroll(&camera, (float)yoffset);
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "ERROR: Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Initialize camera
    vec3 cameraPos = {0.0f, 0.0f, 3.0f};
    vec3 cameraUp = {0.0f, 1.0f, 0.0f};
    initCamera(&camera, cameraPos, cameraUp, -90.0f, 0.0f);

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
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
        processKeyboard(&camera, window, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // pressing esc closes
            glfwSetWindowShouldClose(window, true);

        // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create transformations
        mat4 projection, view, model;
        glm_perspective(glm_rad(camera.fov), 800.0f / 600.0f, 0.1f, 100.0f, projection);
        getViewMatrix(&camera, &view);
        glm_mat4_identity(model);

        // Get uniform locations and pass matrices (keep this the same)
        glUseProgram(shaderProgram);
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");

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