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

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"
#include "SpotLight.h"
#include "Model.h"

const float toRadians = 3.14159265f / 180.0f; // M_PI

GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0,
uniformDirectionalLightTransform = 0;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 15.0f;
float triIncrement = 0.002f;

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

Window mainWindow;
Camera camera;

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Shader directionalShadowShader;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Material shinnyMaterial;
Material dullMaterial;

Model xwing;
Model blackhawk;
Model spaceship1;
Model spaceship2;
Model spaceship3;

GLfloat blackhawkAngle = 0.0f;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

// Directional Shadow shader vertex
static const char* vShadowShader = "Shaders/directional_shadow_map.vert";

// Directional Shadow shader fragment
static const char* vShadowFrag = "Shaders/directional_shadow_map.frag";

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
       // x      y     x     u     v    nx     ny    nz
       -1.0f,  -1.0f, -0.6f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f,  -1.0f,  1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  -1.0f, -0.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f,   1.0f,  0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f
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

    directionalShadowShader.CreateFromFiles(vShadowShader, vShadowFrag);
}

static void RenderScene()
{

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

    shinnyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);

    meshList[0]->RenderMesh();

    // Obj2
    model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(triOffset, 0.0f, -2.5f));  // We only translate x value with triOffset
    model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));  // We only translate x value with triOffset
    // model = glm::rotate(model, curAngle * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate along y axis
    // model = glm::scale(model, glm::vec3(0.006f, 0.006f, 0.006f)); // currently, this will go beyond window
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    dirtTexture.UseTexture();
    dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);

    meshList[1]->RenderMesh();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
    //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    dirtTexture.UseTexture();
    // plainTexture.UseTexture();
    shinnyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    meshList[2]->RenderMesh();

    model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 10.0f));
    model = glm::translate(model, glm::vec3(-7.0f, 0.0f, triOffset));
    // model = glm::rotate(model, curAngle * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate along y axis
    model = glm::scale(model, glm::vec3(0.006f, 0.006f, 0.006f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    shinnyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    xwing.RenderModel();

    blackhawkAngle += 0.1f;

    if (blackhawkAngle > 360.0f)
    {
        blackhawkAngle = 0.1f;
    }

    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(-3.0f, 1.5f, 0.0f));
    // model = glm::translate(model, glm::vec3(triOffset, 1.5f, 0.0f));
    //model = glm::rotate(model, -blackhawkAngle * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate along y axis
    // model = glm::rotate(model, curAngle * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate along y axis
    //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
    //glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    //shinnyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    //spaceship1.RenderModel();

    model = glm::mat4(1.0f);
    model = glm::rotate(model, -blackhawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-8.0f, 2.0f, 0.0f));
    model = glm::rotate(model, -20.0f * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, -90.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    shinnyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
    blackhawk.RenderModel();

}

static void DirectionalShadowMapPass(DirectionalLight* light)
{
    directionalShadowShader.UseShader();

    mainWindow.SetViewPort(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

    light->GetShadowMap()->write();

    glClear(GL_DEPTH_BUFFER_BIT);

    uniformModel = directionalShadowShader.GetModelLocation();
    directionalShadowShader.SetDirectionalLightTransform(&(light->CalculateLightTransform()));

    RenderScene();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

static void RenderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
    shaderList[0].UseShader();

    uniformModel = shaderList[0].GetModelLocation();
    uniformProjection = shaderList[0].GetProjectionLocation();
    uniformView = shaderList[0].GetViewLocation();
    uniformEyePosition = shaderList[0].GetEyePositionLocation();
    uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
    uniformShininess = shaderList[0].GetShininessLocation();

    mainWindow.SetViewPort(0, 0, 2560, 2048);

    // clear window
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // no divide by 256. RGB Color (r, g, b) plus transparancy
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear all colors;

    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

    shaderList[0].SetDirectionalLight(&mainLight);
    shaderList[0].SetPointLights(pointLights, pointLightCount);
    shaderList[0].SetSpotLights(spotLights, spotLightCount);
    shaderList[0].SetDirectionalLightTransform(&(mainLight.CalculateLightTransform()));

    mainLight.GetShadowMap()->Read(GL_TEXTURE1);

    shaderList[0].SetTecture(0);  // Default is 0. So, we really do not need to do this
    shaderList[0].SetDirectionalShadowMap(1);
 
    glm::vec3 lowerLight = camera.getCameraPosition();

    lowerLight.y -= 0.3f;

    spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

    RenderScene();

}

int main() 
{
    //mainWindow = Window(2560, 1440);
    mainWindow = Window(2560, 2048);
    mainWindow.Initialise();

    CreateObjects();
    CreateShaders();
    
    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTextureA();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTextureA();
    plainTexture = Texture("Textures/plain.png");
    plainTexture.LoadTextureA();

    shinnyMaterial = Material(4.0f, 256);
    dullMaterial = Material(0.3f, 4);

    xwing = Model();
    xwing.LoadModel("Models/x-wing.obj");

    blackhawk = Model();
    blackhawk.LoadModel("Models/uh60.obj");

    spaceship1 = Model();
    spaceship1.LoadModel("Models/E-45-Aircraft/E 45 Aircraft_obj.obj");

    /*mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
                                 0.1f, 0.3f, 
                                 0.0f, 0.0f, -1.0f);*/

    mainLight = DirectionalLight(2560, 2048,
                                 1.0f, 1.0f, 1.0f,
                                 0.1f, 0.3f,
                                 0.0f, -15.0f, -10.0f);

    

    pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
                                0.1f, 0.1f,
                                0.0f, 0.0f, 0.0f,
                                0.3f, 0.2f, 0.1f
                               );
    pointLightCount++;
    
    pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
                                0.0f, 0.1f,
                               -4.0f, 2.0f, 0.0f,
                                0.3f, 0.1f, 0.1f);

    pointLightCount++;
    
    

    /*spotLights[0] = SpotLight(0.0f, 0.0f, 1.0f,
                              0.0f, 1.0f,
                              0.0f, 0.0f, 0.0f,
                              0.0f, -1.0f, 0.0f,
                              0.3f, 0.2f, 0.1f,
                              20.0f);*/

    spotLights[0] = SpotLight(1.0f,  1.0f, 1.0f,
                              0.0f,  2.0f,
                              0.0f,  0.0f, 0.0f,
                              0.0f, -1.0f, 0.0f,
                              1.0f,  0.0f, 0.0f,
                              20.0f);
    spotLightCount++;

    spotLights[1] = SpotLight(1.0f,   1.0f, 1.0f,
                               0.0f,  1.0f,
                               0.0f, -1.5f, 0.0f,
                            -100.0f, -1.0f, 0.0f,
                               1.0f,  0.0f, 0.0f,
                              20.0f);

    spotLightCount++;

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

        if (direction) {
            triOffset += triIncrement;
        }
        else {
            triOffset -= triIncrement;
        }

        if (abs(triOffset) >= triMaxOffset) {
            direction = !direction;
        }

        curAngle += 0.005f;

        // Not necessay but good to do to ensure it is not to large after running for a while
        if (curAngle >= 360) {
            curAngle -= 360;
        }

        // if (sizeDirection) {
        if (sizeDirection) {
            curSize += 0.0001f;
        }
        else {
            curSize -= 0.0001f;
        }

        if (curSize >= maxSize || curSize <= minSize) {
            sizeDirection = !sizeDirection;
        }


        // Managing key press
        camera.keyControl(mainWindow.getKeys(), deltaTime);

        // Managing Mouse Movement
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
                
        DirectionalShadowMapPass(&mainLight);

        RenderPass(camera.calculateViewMatrix(), projection);
        
        // glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}
