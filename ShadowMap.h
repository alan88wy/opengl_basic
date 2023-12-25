#pragma once

#include <stdio.h>

#include <GL/glew.h>

class ShadowMap
{
    public:
        ShadowMap();

        virtual bool Init(unsigned int width, unsigned int height);

        virtual void write();

        virtual void Read(GLenum textureUnit);

        GLuint GetShadowWidth() const { return shadowWidth; }

        GLuint GetShadowHeight() const { return shadowHeight; }

        ~ShadowMap();

    private:

        GLuint FBO, shadowMap;
        GLuint shadowWidth, shadowHeight;


};

