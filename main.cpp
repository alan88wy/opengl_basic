#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>  // vec3, vec4, mat4 etc
#include <GLM/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <GLM/gtc/type_ptr.hpp>

#include "Common.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f; // M_PI

Window mainWindow;
Camera camera;

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Material shinnyMaterial;
Material dullMaterial;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

static void calcAverageNormals(unsigned int* indices, unsigned int indiceCount,
                               GLfloat* vertices, unsigned int verticeCount, 
                               unsigned int vLength, unsigned int normalOffset)
{
    for (size_t i = 0; i < indiceCount; i += 3)
    {
        unsigned int in0 = indices[i] * vLength;
        unsigned int in1 = indices[i + 1] * vLength;
        unsigned int in2 = indices[i + 2] * vLength;

        glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
        glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
        
        glm::vec3 normal = glm::cross(v1, v2);

        normal = glm::normalize(normal);

        in0 += normalOffset;
        in1 += normalOffset;
        in2 += normalOffset;

        vertices[in0] += normal.x;
        vertices[in0 + 1] += normal.y;
        vertices[in0 + 2] += normal.z;

        vertices[in1] += normal.x;
        vertices[in1 + 1] += normal.y;
        vertices[in1 + 2] += normal.z;

        vertices[in2] += normal.x;
        vertices[in2 + 1] += normal.y;
        vertices[in2 + 2] += normal.z;
    }

    for (size_t i = 0; i < verticeCount / vLength; i++)
    {
        unsigned int nOffset = i * vLength + normalOffset;

        glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);

        vec = glm::normalize(vec);

        vertices[nOffset] = vec.x;
        vertices[nOffset + 1] = vec.y;
        vertices[nOffset + 2] = vec.z;

    }
}

static void CreateObjects() 
{
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    GLfloat vertices[] = {
       // x      y     x     u     v    normal
        -1.0f, -1.0f, -0.6f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f,  -1.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  -1.0f, 0.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f,  1.0f,  0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f
    };

    unsigned int floorIndices[] = {
        0, 2, 1,
        1, 2, 3
    };

    GLfloat floorVertices[] = {
        -10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
        -10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
    };

    calcAverageNormals(indices, 12, vertices, 32, 8, 5);

    Mesh *obj1 = new Mesh();

    obj1->CreateMesh(vertices, indices, 32, 12);

    meshList.push_back(obj1);

    Mesh *obj2 = new Mesh();

    obj2->CreateMesh(vertices, indices, 32, 12);

    meshList.push_back(obj2);

    Mesh* obj3 = new Mesh();

    obj3->CreateMesh(floorVertices, floorIndices, 32, 6);

    meshList.push_back(obj3);
}

static void CreateShaders()
{
    Shader *shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

int main() 
{
    mainWindow = Window(2560, 1440);
    mainWindow.Initialise();

    CreateObjects();
    CreateShaders();
    
    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 1.0f, 0.3f);

    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTexture();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTexture();
    plainTexture = Texture("Textures/plain.png");
    plainTexture.LoadTexture();

    shinnyMaterial = Material(4.0f, 256);
    dullMaterial = Material(0.3f, 4);

    mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
                                 0.0f, 0.0f, 
                                 0.0f, 0.0f, -1.0f);

    unsigned int pointLightCount = 0;

    pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
                                0.0f, 1.0f,
                                0.0f, 0.0f, 0.0f,
                                0.3f, 0.2f, 0.1f
                               );
    pointLightCount += 1;
    
    pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
        0.0f, 1.0f,
        -4.0f, 2.0f, 0.0f,
        0.3f, 0.1f, 0.1f);

    pointLightCount++;

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
           uniformSpecularIntensity = 0, uniformShininess = 0;

    // Create projection variable
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat) (mainWindow.getBufferWidth() / mainWindow.getBufferHeight()), 0.1f, 100.0f); // (y, aspect, near, far

    // Loop until window close
    while (!mainWindow.getShouldClose()) 
    {
        GLfloat now = glfwGetTime();  // for SDL, SDL_GetPerformnceCounter();
        deltaTime = now - lastTime;   // for SDL, (now - lasttime)*1000/SDL_GetPerformanceFrequency() 
        lastTime = now;

        // Get & Handle user input events
        glfwPollEvents();

        // Managing key press
        camera.keyControl(mainWindow.getKeys(), deltaTime);

        // Managing Mouse Movement
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
                
        // clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // no divide by 256. RGB Color (r, g, b) plus transparancy
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear all colors;

        shaderList[0].UseShader();

        uniformModel = shaderList[0].GetModelLocation();
        uniformProjection = shaderList[0].GetProjectionLocation();
        uniformView = shaderList[0].GetViewLocation();
        uniformEyePosition = shaderList[0].GetEyePositionLocation();
        uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
        uniformShininess = shaderList[0].GetShininessLocation();

        shaderList[0].SetDirectionalLight(&mainLight);
        shaderList[0].SetPointLights(pointLights, pointLightCount);

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


        // glm::mat4 model(1.0f);  Or alternatively : glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 model = glm::mat4(1.0f);
        
        // order is important
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));  // We only translate x value with triOffset

        // model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate along y axis
        // scaling
        // model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f)); // currently, this will go beyond window

        // Specify the value of a uniform variable for the current program object
        // void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
     
        brickTexture.UseTexture();
        
        shinnyMaterial.UseMaterials(uniformSpecularIntensity, uniformShininess);

        meshList[0]->RenderMesh();

        // Obj2
        model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(triOffset, 0.0f, -2.5f));  // We only translate x value with triOffset
        model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));  // We only translate x value with triOffset
       // model = glm::rotate(model, curAngle * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate along y axis
       // model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f)); // currently, this will go beyond window
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
  
        dirtTexture.UseTexture();
        dullMaterial.UseMaterials(uniformSpecularIntensity, uniformShininess);

        meshList[1]->RenderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        plainTexture.UseTexture();
        shinnyMaterial.UseMaterials(uniformSpecularIntensity, uniformShininess);
        meshList[2]->RenderMesh();
        
        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}
