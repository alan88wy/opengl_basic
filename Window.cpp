#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;

	xChange = 0.0f;
	yChange = 0.0f;

	// mouseFirstMoved = true;

	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;  // false;
	}

}

Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	xChange = 0.0f;
	yChange = 0.0f;

	// mouseFirstMoved = true;

	for (size_t i = 0; i < 1024; i++) 
	{
		keys[i] = 0;  // false;
	}

}

int Window::Initialise()
{
	if (!glfwInit())
	{
		printf("Error Initialising GLFW");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW Windows Properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	// Core Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatiblity
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create the window
	mainWindow = glfwCreateWindow(width, height, "Test Window", NULL, NULL);

	if (!mainWindow)
	{
		printf("Error creating GLFW window!");
		glfwTerminate();
		return 1;
	}

	// Get buffer size information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set the current context
	glfwMakeContextCurrent(mainWindow);

	// Hanfle Key + Mouse Input
	createCallbacks();

	// Lock cursor to screen. With this we cannote prexx the top right X button to close. 
	// We need to press ESCAPE key.

	// disable mouse
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

	// Allow modern extension access
	glewExperimental = GL_TRUE;

	GLenum error = glewInit();

	if (error != GLEW_OK)
	{
		printf("Error: %s", glewGetErrorString(error));
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	// Create Viewport
	// SetViewPort(0, 0, bufferWidth, bufferHeight);
	glViewport(0, 0, bufferWidth, bufferHeight);

	glfwSetWindowUserPointer(mainWindow, this);

	return 0;
}

void Window::createCallbacks() 
{
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat Window::getXChange() 
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange() 
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{

	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window)); // Get the user pointer which is "this" above

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			theWindow->keys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			theWindow->keys[key] = false;
		}
	}
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{

	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window)); // Get the user pointer which is "this" above

	if (theWindow->mouseFirstMoved) {
		theWindow->mouseFirstMoved = false;
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;  // can do the other way round but the coordinate system will be upside down

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

void Window::SetViewPort(GLuint x, GLuint y, GLuint width, GLuint height)
{
	glViewport(x, y, width, height);
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
