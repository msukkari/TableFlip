#ifndef GAME_H
#define GAME_H

#include "GL/glew.h"
#include "GLFW\glfw3.h"
#include <stdio.h>


#include <vector>

class Game
{

public:
	int initWindow();
	GLFWwindow* getWindow() { return m_window; };


private:
	GLFWwindow* m_window;
	const GLuint WIDTH = 1280, HEIGHT = 720;

};

#endif