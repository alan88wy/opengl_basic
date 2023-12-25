#pragma once

#include "stdio.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>

class Window
{
	public:
		Window();

		Window(GLint windowWidth, GLint windowHeight);

		int Initialise();

		GLint getBufferWidth() const { return bufferWidth; }
		GLint getBufferHeight() const { return bufferHeight; }

		bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

		bool* getKeys() { return keys; };

		GLfloat getXChange();
		GLfloat getYChange();

		void swapBuffers() { glfwSwapBuffers(mainWindow); }
		void SetViewPort(GLuint x, GLuint y, GLuint width, GLuint height);

		~Window();

	private:
		GLFWwindow* mainWindow;

		GLint width, height;
		GLint bufferWidth, bufferHeight;

		bool keys[1024]; // 1024 to cover the range of ASCII character. Set true when key is press.

		GLfloat lastX;
		GLfloat lastY;
		GLfloat xChange;
		GLfloat yChange;
		bool mouseFirstMoved; // for first mouse move to set lastX and lastY

		void createCallbacks();

		static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
		static void handleMouse(GLFWwindow* window, double xPos, double yPos);

};