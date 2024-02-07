#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>

#include <GLM/glm.hpp>  // vec3, vec4, mat4 etc
#include <GLM/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <GLM/gtc/type_ptr.hpp>

#include "CommonValues.h"

#include "Mesh.h"
#include "Shader.h"

class Skybox
{

    public:
        Skybox();

        Skybox(std::vector<std::string> faceLocations);

        void DrawSkyBox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

        ~Skybox();

    private:

        // Skybox Shader
        const char* vSkyShader = "Shaders/skybox.vert";
        const char* vSkyFrag = "Shaders/skybox.frag";

        Mesh* skyMesh;
        Shader* skyShader;

        GLuint textureId;
        GLuint uniformProjection, uniformView;
};

