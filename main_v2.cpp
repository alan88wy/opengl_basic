#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <cmath>

// Window Dimentions
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader, uniformXMove;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0005f;

// Create Shader. Normally, you create in external file

// Vertex Shader
static const char* vShader = "                                               \n\
#version 410                                                                 \n\
                                                                             \n\
layout (location = 0) in vec3 pos;                                           \n\
                                                                             \n\
uniform float xMove;                                                         \n\
                                                                             \n\
void main()                                                                  \n\
{                                                                            \n\
     gl_Position = vec4(0.6 * pos.x + xMove, 0.6 * pos.y, pos.z, 1.0);       \n\
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

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);  // Remove after use
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

    uniformXMove = glGetUniformLocation(shader, "xMove");

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

        // clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // no divide by 256. RGB Color (r, g, b) plus transparancy
        glClear(GL_COLOR_BUFFER_BIT);  // Clear all colors;

        glUseProgram(shader);

        glUniform1f(uniformXMove, triOffset);

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
