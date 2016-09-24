#include "game.h"
#include "GL/glew.h"
#include "GLFW\glfw3.h"
#include <stdio.h>

int Game::initWindow()
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Open a window and create its OpenGL context	
	m_window = glfwCreateWindow(WIDTH, HEIGHT, "TableFlip", NULL, NULL);
	if (m_window == NULL) {
		fprintf(stderr, "Failed to open GLFW window");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(m_window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}


	glfwSetInputMode(m_window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glViewport(0, 0, WIDTH, HEIGHT);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glfwSetCursorPos(m_window, WIDTH / 2, HEIGHT / 2);

	return 1;
}

