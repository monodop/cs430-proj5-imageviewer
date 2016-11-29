
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

	char windowTitle[100];

	GLint program_id, position_slot, color_slot, trans_slot, rot_slot, scale_slot, shear_slot;
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
		"Image Viewer",
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
	trans_slot = glGetUniformLocation(program_id, "Translation");
	rot_slot = glGetUniformLocation(program_id, "Rotation");
	scale_slot = glGetUniformLocation(program_id, "Scale");
	shear_slot = glGetUniformLocation(program_id, "Shear");
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

	// Setup manipulation variables
	float trans[2] = { 0, 0 };
	float scale[2] = { 1, 1 };
	float shear[2] = { 0, 0 };
	float rotation = 0;

	// Repeat
	while (!glfwWindowShouldClose(window)) {

		// Horizontal Translation
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			trans[0] -= 0.01;
		}
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			trans[0] += 0.01;
		}

		// Vertical Translation
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			trans[1] += 0.01;
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			trans[1] -= 0.01;
		}

		// Rotation
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			rotation += 0.01;
		}
		else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			rotation -= 0.01;
		}

		// Horizontal Scale
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			scale[0] *= 1.005;
		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			scale[0] /= 1.005;
		}

		// Vertical Scale
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			scale[1] *= 1.005;
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			scale[1] /= 1.005;
		}

		// Horizontal Shear
		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
			shear[0] += 0.01;
		}
		else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
			shear[0] -= 0.01;
		}

		// Vertical Shear
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
			shear[1] -= 0.01;
		}
		else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
			shear[1] += 0.01;
		}

		sprintf_s(windowTitle, 100, "Image Viewer - (%.2f, %.2f)", shear[0], shear[1]);
		glfwSetWindowTitle(window, windowTitle);

		glUniform2f(trans_slot, trans[0], trans[1]);
		glUniform2f(scale_slot, scale[0], scale[1]);
		glUniform2f(shear_slot, shear[0], shear[1]);
		glUniform1f(rot_slot, rotation);

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