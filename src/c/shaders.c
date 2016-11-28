
#include "../headers/shaders.h"
#include "../headers/res_manager.h"
#include <string.h>
#include <stdio.h>

GLint shader_create_shader(GLint shader_type, char* shader_src) {

	GLint compile_success = 0;

	int shader_id = glCreateShader(shader_type);

	glShaderSource(shader_id, 1, &shader_src, 0);

	glCompileShader(shader_id);

	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_success);

	if (compile_success == GL_FALSE) {
		GLchar message[256];
		glGetShaderInfoLog(shader_id, sizeof(message), 0, &message[0]);
		printf("glCompileShader Error: %s\n", message);
		exit(1);
	}

	return shader_id;
}


GLint shader_create_program(int vertex_shader_res, int fragment_shader_res) {

	char* vertex_shader_src = load_resource(vertex_shader_res, "SHADER");
	char* fragment_shader_src = load_resource(fragment_shader_res, "SHADER");

	GLint link_success = 0;


	GLint program_id = glCreateProgram();
	GLint vertex_shader = shader_create_shader(GL_VERTEX_SHADER, vertex_shader_src);
	GLint fragment_shader = shader_create_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

	glAttachShader(program_id, vertex_shader);
	glAttachShader(program_id, fragment_shader);

	glLinkProgram(program_id);

	glGetProgramiv(program_id, GL_LINK_STATUS, &link_success);

	if (link_success == GL_FALSE) {
		GLchar message[256];
		glGetProgramInfoLog(program_id, sizeof(message), 0, &message[0]);
		printf("glLinkProgram Error: %s\n", message);
		exit(1);
	}

	return program_id;
}