#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialize(char input2);
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }
	bool* getKeys() { return keys; }
	void SwapBuffers() { glfwSwapBuffers(mainWindow); }
	~Window();
private:
	GLFWwindow* mainWindow;
	GLint width, height;
	GLint bufferWidth, bufferHeight;
	bool keys[1024];		//tablica klawiszy - aktualizowana po nacisnieciach, wystarczy potem tylko ja odczytywac regularnie
	void createCallbacks();
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
};

