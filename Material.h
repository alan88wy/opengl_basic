#pragma once
#include <GL/glew.h>

class Material
{

    public:
        Material();
        Material(GLfloat sIntensity, GLfloat shine);

        void UseMaterials(GLuint specularIntensityLocation, GLuint shininessLocation);

        ~Material();

    private:
        GLfloat specularIntensity;
        GLfloat shininess;

};

