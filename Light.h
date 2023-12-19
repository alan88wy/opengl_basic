#pragma once

#include <GL/glew.h>
#include <GLM/glm.hpp>

class Light
{

    public:
        Light();
        Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity,
            GLfloat xDirection, GLfloat yDirection, GLfloat zDirection, GLfloat dIntensity);

        void useLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
            GLuint diffuseIntensityLocation, GLuint directionLocation);

        ~Light();

    public:
        glm::vec3 colour;
        GLfloat ambientIntensity;

        glm::vec3 direction;
        GLfloat diffuseIntensity;
};

