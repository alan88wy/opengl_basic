#define _USE_MATH_DEFINES

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>  // vec3, vec4, mat4 etc
#include <GLM/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <GLM/gtc/type_ptr.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

const float toRadians = 3.14159265f / 180.0f; // M_PI

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0002f;

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Window mainWindow;

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

static void CreateObjects() 
{
    unsigned int indices[] = {
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    Mesh *obj1 = new Mesh();

    obj1->CreateMesh(vertices, indices, 12, 12);

    meshList.push_back(obj1);

    Mesh *obj2 = new Mesh();

    obj2->CreateMesh(vertices, indices, 12, 12);

    meshList.push_back(obj2);
}

static void CreateShaders()
{
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

int main() 
{
    Window mainWindow = Window(800, 600);
    // mainWindow.Initialise();

    CreateObjects();
    CreateShaders();
    
    Camera camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.1f, 1.0f);

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0;

    // Create projection variable
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f); // (y, aspect, near, far

    // Loop until window close
    while (!mainWindow.getShouldClose()) 
    {
        GLfloat now = glfwGetTime();  // for SDL, SDL_GetPerformnceCounter();
        deltaTime = now - lastTime;   // for SDL, (now - lasttime)*1000/SDL_GetPerformanceFrequency() 


        // Get & Handle user input events
        glfwPollEvents();

        // Managing key press
        camera.keyControl(mainWindow.getKeys(), deltaTime);

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

        // clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // no divide by 256. RGB Color (r, g, b) plus transparancy
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear all colors;

        shaderList[0].UseShader();

        uniformModel = shaderList[0].GetModelLocation();
        uniformProjection = shaderList[0].GetProjectionLocation();
        uniformView = shaderList[0].GetViewLocation();

        // glm::mat4 model(1.0f);  Or alternatively : glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 model = glm::mat4(1.0f);
        
        // order is important
        // model = glm::translate(model, glm::vec3(0.0f, triOffset, -2.5f));  // We only translate x value with triOffset
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));  // We only translate x value with triOffset

        // model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate along y axis
        // scaling
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f)); // currently, this will go beyond window

        // Specify the value of a uniform variable for the current program object
        // void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

        meshList[0]->RenderMesh();

        // Obj2
        model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(triOffset, 0.0f, -2.5f));  // We only translate x value with triOffset
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));  // We only translate x value with triOffset
       // model = glm::rotate(model, curAngle * toRadians, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate along y axis
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f)); // currently, this will go beyond window
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
  
        meshList[1]->RenderMesh();
        
        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}
