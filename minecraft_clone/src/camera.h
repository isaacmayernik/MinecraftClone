#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include "cglm/cglm.h"

typedef struct
{
    vec3 pos;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;

    float fov;
} Camera;

void initCamera(Camera *camera, vec3 position, vec3 up, float yaw, float pitch);
void updateCameraVectors(Camera *camera);
void processKeyboard(Camera *camera, GLFWwindow *window, float deltaTime);
void processMouseMovement(Camera *camera, float xoffset, float yoffset, GLboolean constrainPitch);
void processMouseScroll(Camera *camera, float yoffset);
mat4 *getViewMatrix(Camera *camera, mat4 *view);

#endif