#include "Camera.h"

Camera::Camera()
{
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    worldUp = glm::vec3(0.0f, 0.0f, 0.0f);
    yaw = 0.0f;
    pitch = 0.0f;
    front = glm::vec3(0.0f, 0.0f, 0.0f);;

    moveSpeed = 0.0f;
    turnSpeed = 0.0f;

    update();
}

Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch, GLfloat moveSpeed, GLfloat turnSpeed)
{
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    this->front = glm::vec3(0.0f, 0.0f, -1.0f);

    this->moveSpeed = moveSpeed;
    this->turnSpeed = turnSpeed;

    update();
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{

    GLfloat velocity = moveSpeed * deltaTime;

    if (keys[GLFW_KEY_W])
    {
        position += front * velocity; // Forward
    }

    if (keys[GLFW_KEY_S])  
    {
        position -= front * velocity;  // Backward
    }

    if (keys[GLFW_KEY_A])
    {
        position -= right * velocity;  // Left
    }

    if (keys[GLFW_KEY_D])
    {
        position += right * velocity;  // Right
    }
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
    xChange *= turnSpeed;
    yChange *= turnSpeed;

    yaw += xChange;
    pitch += yChange;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }

    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    update();

}

glm::mat4 Camera::calculateViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

void Camera::update()
{
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));

}

Camera::~Camera()
{

}
