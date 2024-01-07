#pragma once
#include "ShadowMap.h"

class OmniShadowMap :
    public ShadowMap
{
    public:
        OmniShadowMap();

        bool Init(unsigned int width, unsigned int height);

        void write();

        void Read(GLenum textureUnit);

        ~OmniShadowMap();

};

