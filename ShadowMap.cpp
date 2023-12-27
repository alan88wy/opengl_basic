#include "ShadowMap.h"

ShadowMap::ShadowMap()
{
    FBO = 0;
    shadowMap = 0;
}

bool ShadowMap::Init(unsigned int width, unsigned int height)
{
    shadowWidth = width;
    shadowHeight = height;

    glGenFramebuffers(1, &FBO); // Create Framebuffer

    glGenTextures(1, &shadowMap);   // Generate Shadow Map

    glBindTexture(GL_TEXTURE_2D, shadowMap);  // Bind Shadowmap to texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColour[] = {10.f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColour); // Bi

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);  // Bind FBO to Draw Framebuffer

    // Connect Framebuffer to texture
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0); 

    glDrawBuffer(GL_NONE);  // None. We only need GL_DEPTH_ATTACHMENT
    glReadBuffer(GL_NONE);  //

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Frame buffer Error : %i\n", status);
        return false;
    }

    // glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind to default frame buffer

    return true;
}

void ShadowMap::write()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO); // Bind to defined frame buffer
}

void ShadowMap::Read(GLenum textureUnit)
{
    glActiveTexture(textureUnit);                // to use the textureUnit
    glBindTexture(GL_TEXTURE_2D, shadowMap);     // Attach the textureUnit id to shadowMap
}

ShadowMap::~ShadowMap()
{
    if (FBO)
    {
        glDeleteFramebuffers(1, &FBO);

        if (shadowMap)
        {
            glDeleteTextures(1, &shadowMap);
        }
    }
}