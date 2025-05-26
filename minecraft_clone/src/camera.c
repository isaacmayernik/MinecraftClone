#include "camera.h"
#include <math.h>

void initCamera(Camera *camera, vec3 position, vec3 up, float yaw, float pitch)
{
    glm_vec3_copy(position, camera->pos);
    glm_vec3_copy(up, camera->worldUp);
    camera->yaw = yaw;
    camera->pitch = pitch;
    camera->movementSpeed = 2.5f;
    camera->mouseSensitivity = 0.1f;
    camera->fov = 45.0f;

    updateCameraVectors(camera);
}

void updateCameraVectors(Camera *camera)
{
    vec3 front;
    front[0] = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    front[1] = sin(glm_rad(camera->pitch));
    front[2] = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    glm_normalize_to(front, camera->front);

    glm_cross(camera->front, camera->worldUp, camera->right);
    glm_normalize(camera->right);

    glm_cross(camera->right, camera->front, camera->up);
    glm_normalize(camera->up);
}

void processKeyboard(Camera *camera, GLFWwindow *window, float deltaTime)
{
    float velocity = camera->movementSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        glm_vec3_muladds(camera->front, velocity, camera->pos);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        glm_vec3_muladds(camera->front, -velocity, camera->pos);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        glm_vec3_muladds(camera->right, -velocity, camera->pos);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        glm_vec3_muladds(camera->right, velocity, camera->pos);
}

void processMouseMovement(Camera *camera, float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= camera->mouseSensitivity;
    yoffset *= camera->mouseSensitivity;

    camera->yaw += xoffset;
    camera->pitch += yoffset;

    if (constrainPitch)
    {
        if (camera->pitch > 89.0f)
            camera->pitch = 89.0f;
        if (camera->pitch < -89.0f)
            camera->pitch = -89.0f;
    }

    updateCameraVectors(camera);
}

void processMouseScroll(Camera *camera, float yoffset)
{
    camera->fov -= yoffset;
    if (camera->fov < 1.0f)
        camera->fov = 1.0f;
    if (camera->fov > 45.0f)
        camera->fov = 45.0f;
}

mat4 *getViewMatrix(Camera *camera, mat4 *view)
{
    vec3 center;
    glm_vec3_add(camera->pos, camera->front, center);
    glm_lookat(camera->pos, center, camera->up, view);
    return view;
}