
#include <stdlib.h>
#include <stdio.h>

#include "../headers/vertex.h"
#include "../headers/shaders.h"
#include "../../resource.h"

GLFWwindow* window;

const Vertex Vertices[] = {
	{{1, -1, 0}, {1, 0, 0, 1}},
	{{1, 1, 0}, {0, 1, 0, 1}},
	{{-1, 1, 0}, {0, 0, 1, 1}},
	{{-1, -1, 0}, {0, 0, 0, 1}}
};

const GLubyte Indices[] = {
	0, 1, 2,
	2, 3, 0
};

static void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

int main(void) {

	GLint program_id, position_slot, color_slot;
	GLuint vertex_buffer;
	GLuint index_buffer;

	glfwSetErrorCallback(error_callback);

	// Initialize GLFW library
	if (!glfwInit())
		return -1;

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Create and open a window
	window = glfwCreateWindow(640,
		480,
		"Hello World",
		NULL,
		NULL);

	if (!window) {
		glfwTerminate();
		printf("glfwCreateWindow Error\n");
		exit(1);
	}

	glfwMakeContextCurrent(window);

	program_id = shader_create_program(RES_VERTEX_SHADER, RES_FRAGMENT_SHADER);

	glUseProgram(program_id);

	position_slot = glGetAttribLocation(program_id, "Position");
	color_slot = glGetAttribLocation(program_id, "SourceColor");
	glEnableVertexAttribArray(position_slot);
	glEnableVertexAttribArray(color_slot);

	// Create Buffer
	glGenBuffers(1, &vertex_buffer);

	// Map GL_ARRAY_BUFFER to this buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	// Send the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// Repeat
	while (!glfwWindowShouldClose(window)) {

		glClearColor(0, 104.0 / 255.0, 55.0 / 255.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, 640, 480);

		glVertexAttribPointer(position_slot,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			0);

		glVertexAttribPointer(color_slot,
			4,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(GLvoid*)(sizeof(float) * 3));

		glDrawElements(GL_TRIANGLES,
			sizeof(Indices) / sizeof(GLubyte),
			GL_UNSIGNED_BYTE, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}