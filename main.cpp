#define _USE_MATH_DEFINES

#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <cmath>
//#include <GLM/vec3.hpp> // glm::vec3
//#include <GLM/vec4.hpp> // glm::vec4
//#include <GLM/mat4x4.hpp> // glm::mat4
#include <GLM/glm.hpp>  // vec3, vec4, mat4 etc
#include <GLM/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <GLM/gtc/type_ptr.hpp>

// Window Dimentions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = M_PI / 180.0f; // convert to radian 3.14159265f

GLuint VAO, VBO, shader, uniformModel;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0002f;

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

// Create Shader. Normally, you create in external file

// Vertex Shader
static const char* vShader = "                                       \n\
#version 410                                                         \n\
                                                                     \n\
layout (location = 0) in vec3 pos;                                   \n\
                                                                     \n\
uniform mat4 model;                                                  \n\
                                                                     \n\
void main()                                                          \n\
{                                                                    \n\
     gl_Position = model * vec4(pos, 1.0);                           \n\
}";


// Fragment Shader
static const char* fShader = "                                       \n\
#version 410                                                         \n\
                                                                     \n\
out vec4 colour;                                                     \n\
                                                                     \n\
void main()                                                          \n\
{                                                                    \n\
     colour = vec4(1.0, 0.0, 0.0, 1.0);                              \n\
}";


void CreateTriangle() 
{
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };

    // VAO - Vertex array object
    //       A vertex array object (also known as VAO) can be bound just like a vertex buffer 
    //       object and any subsequent 
    //       vertex attribute calls from that point on will be stored inside the VAO.
    // VBO - Vertex Buffer Object (VBO) to store vertex data in GPU memory

    glGenVertexArrays(1, &VAO);  // glGenVertexArrays(GLsizei n, GLuint* arrays); generate vertex array object names
    glBindVertexArray(VAO);      // bind a vertex array object

    glGenBuffers(1, &VBO);       // glGenBuffers(GLsizei n, GLuint* buffers); generate buffer object names
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // glBindBuffer(GLenum target, GLuint buffer); bind a named buffer object

    // glBufferData, glNamedBufferData — creates and initializes a buffer object's data store 
    // void glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
    //                            const void* pointer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // define an array of generic vertex attribute data
    glEnableVertexAttribArray(0); // void glEnableVertexAttribArray(GLuint index); Enable or disable generic vertex attribute array

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the buffer because we set it to 0

    glBindVertexArray(0);  // Remove after use. Unbind the buffer because we set it to 0
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) 
{
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1] = {};

    theCode[0] = shaderCode;

    GLint codeLength[1] = {};

    codeLength[0] = strlen(shaderCode);

    glShaderSource(theShader, 1, theCode, codeLength);

    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

    if (!result) {
        glGetProgramInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("Error compiling the %d shader : '%s'\n", shaderType, eLog);
        return;
    }


    glAttachShader(theProgram, theShader);

    return;
}

void CompileShaders() 
{
    shader = glCreateProgram();

    if (!shader) {
        printf("error creating shader program !");
        return;
    }

    AddShader(shader, vShader, GL_VERTEX_SHADER);
    AddShader(shader, fShader, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &result);

    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error linking program : '%s'\n", eLog);
        return;
    }

    glValidateProgram(shader);

    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error validating program : '%s'\n", eLog);
        return;
    }

    uniformModel = glGetUniformLocation(shader, "model");

}

void init_opengl() {

}

int main() 
{

    // Initialize GLFW

    if (!glfwInit()) {
        printf("GLFW Initialization failed!");
        glfwTerminate();
        return 1;
    }

    // Setup GLFW Window Properties

    // Setting OPENGL Version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Setting Profile - Core Profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Allow forward compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create Main Window
    GLFWwindow* mainwindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);

    if (!mainwindow) {
        printf("GLFW Window creation failed!");
        glfwTerminate();
        return 1;
    }

    // Get Buffer size information
    int bufferWidth, bufferHeight;

    glfwGetFramebufferSize(mainwindow, &bufferWidth, &bufferHeight);

    // Set context for GLEW to use
    glfwMakeContextCurrent(mainwindow);  // Use window created
    // glfwSetFramebufferSizeCallback(mainwindow, framebuffer_size_callback);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        printf("GLEW Initialization failed!");
        glfwDestroyWindow(mainwindow);
        glfwTerminate();
        return 1;
    }

    // Setup Viewport Size
    glViewport(0, 0, bufferWidth, bufferHeight);

    CreateTriangle();
    CompileShaders();

    // Loop until window close
    while (!glfwWindowShouldClose(mainwindow)) {

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

        // clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // no divide by 256. RGB Color (r, g, b) plus transparancy
        glClear(GL_COLOR_BUFFER_BIT);  // Clear all colors;

        glUseProgram(shader);

        // glm::mat4 model(1.0f);
        // Or alternatively :
        // glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 model = glm::mat4(1.0f);
        
        // order is important
        // model = glm::scale(model, glm::vec3(curSize, curSize, 1.0f)); // currently, this will go beyond window
        model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));  // We only translate x value with triOffset
        // model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate along y axis

        // scalling
        model = glm::scale(model, glm::vec3(curSize, 0.4f, 1.0f)); // currently, this will go beyond window
        // if we put translate here, it will not do that.

        // void glUniform1f(GLint location, GLfloat v0); v0 = triOffset
        glUniform1f(uniformModel, triOffset);  // Specify the value of a uniform variable for the current program object

        // Specify the value of a uniform variable for the current program object
        // void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);
        
        glUseProgram(0);

        glfwSwapBuffers(mainwindow);
    }


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    return 0;
}
