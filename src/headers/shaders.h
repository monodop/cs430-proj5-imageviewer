#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>

GLint shader_create_shader(GLint shader_type, char* shader_src);
GLint shader_create_program(int vertex_shader_res, int fragment_shader_res);