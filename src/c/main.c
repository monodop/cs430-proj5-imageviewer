
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>

#include "../headers/vertex.h"
#include "../headers/shaders.h"
#include "../../resource.h"
#include "../headers/ppm.h"
#include "../headers/scene.h"
#include "../headers/raycast.h"

#define MODE_IMAGE 1
#define MODE_RAYCAST 2
#define WORKER_THREADS 8

GLFWwindow* window;

const Vertex Vertices[] = {
	{{1, -1, 0}, {1, 1, 1, 1}, {1, 1}},
	{{1, 1, 0}, {1, 1, 1, 1}, {1, 0}},
	{{-1, 1, 0}, {1, 1, 1, 1}, {0, 0}},
	{{-1, -1, 0}, {1, 1, 1, 1}, {0, 1}}
};

const GLubyte Indices[] = {
	0, 1, 2,
	2, 3, 0
};

static void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

int displayUsage() {
	fprintf(stderr, "Usage:\nezview image.ppm\n");
	fprintf(stderr, " - image.ppm: The name of the ppm file to be displayed.\n");
	fprintf(stderr, " OR \n");
	fprintf(stderr, "ezview width height input.json output.ppm\n");
	fprintf(stderr, " - width: The width of the image to generate.\n");
	fprintf(stderr, " - height: The height of the image to generate.\n");
	fprintf(stderr, " - input.json: The name of the json file that contains the scene information.\n");
	fprintf(stderr, " - output.ppm: The name of the ppm file that will be created with the output image.\n");
	return 1;
}

int main(int argc, char* argv[]) {

	char mode;
	FILE* filePointer;
	PpmImage image;
	LowpColor* lowpImage;
	char* imageFilename;
	char* jsonFilename;
	char* frameFilename;
	int width, height, frameCount, i, padding, dotIndex;
	Scene scene;
	JsonElement rootElement;

	Worker workers[WORKER_THREADS];

	// Validate number of arguments
	if (argc == 2) {
		mode = MODE_IMAGE;

		// Read input image file
		imageFilename = argv[1];
		ppm_read(imageFilename, &image);
	}
	else if (argc == 5) {
		mode = MODE_RAYCAST;
		
		// Validate width parameter
		width = atoi(argv[1]);
		if (width <= 0) {
			fprintf(stderr, "Error: Invalid parameter width = '%s', must be greater than 0.\n", argv[1]);
			return displayUsage();
		}

		// Validate height parameter
		height = atoi(argv[2]);
		if (width <= 0) {
			fprintf(stderr, "Error: Invalid parameter height = '%s', must be greater than 0.\n", argv[2]);
			return displayUsage();
		}

		jsonFilename = argv[3];
		imageFilename = argv[4];

		// Open input file
		printf("Processing input json file.\n");
		filePointer = fopen(jsonFilename, "rb");
		if (filePointer == NULL) {
			fprintf(stderr, "Error: File '%s' does not exist or cannot be opened. Error number %d.\n", jsonFilename, errno);
			return displayUsage();
		}

		// Parse scene data
		if (!json_parse(filePointer, &rootElement)) {
			fprintf(stderr, "Error: Unable to parse JSON input file. Render cancelled.\n");
			return displayUsage();
		}

		printf("Input json file processed.\n");
		// Close input file
		fclose(filePointer);

		// Build scene
		if (!scene_build(&rootElement, &scene)) {
			fprintf(stderr, "Error: Unable to build object scene. Render cancelled.\n");
			return displayUsage();
		}

		// Unload json data - its no longer needed
		if (!json_dispose(&rootElement)) {
			fprintf(stderr, "Error: Error disposing json structure. Render cancelled.\n");
			return displayUsage();
		}
		printf("Json data unloaded.\n");

		// Setup image file
		if (!image_create(&image, (unsigned int)width, (unsigned int)height, (Color) { .r = 0, .g = 0, .b = 0 })) {
			fprintf(stderr, "Error: Could not create image buffer. Render cancelled.\n");
			return displayUsage();
		}
		printf("Image buffer created.\n");

		// Calculate number of frames
		if (scene.camera->data.camera.animated) {
			frameCount = (int)ceil((scene.camera->data.camera.endTime - scene.camera->data.camera.startTime) * scene.camera->data.camera.frameRate) + 1;
			if (frameCount < 0) {
				fprintf(stderr, "Error: startTime must be greater than endTime. Render cancelled.\n");
				return 0;
			}

			// Calculate frame filename template
			padding = (int)floor(log10(abs(frameCount))) + 1;
			frameFilename = malloc(sizeof(char) * (strlen(imageFilename) + padding + 2));
			dotIndex = (int)strcspn(imageFilename, ".");
			memcpy(frameFilename, imageFilename, dotIndex);
			strcpy(frameFilename + dotIndex + padding + 1, imageFilename + dotIndex);
			frameFilename[dotIndex] = '-';
		}
		else {
			frameCount = 1;
			frameFilename = imageFilename;
		}

		// Allocate worker threads
		if (!raycast_create_workers(workers, &image, &scene, WORKER_THREADS)) {
			fprintf(stderr, "Error: Could not allocate worker threads. Render cancelled.\n");
			return displayUsage();
		}
		printf("Ray casting workers allocated.\n");

		// Begin render loop
		for (i = 1; i <= frameCount; i++) {

			printf("Rendering frame %d of %d.\n", i, frameCount);

			// Calculate filename for frame
			if (scene.camera->data.camera.animated) {
				sprintf(frameFilename + dotIndex + 1, "%0*d", padding, i);
				frameFilename[dotIndex + padding + 1] = '.';
			}

			// Prep frame
			double t = scene.camera->data.camera.startTime + (i - 1) * (scene.camera->data.camera.endTime - scene.camera->data.camera.startTime) / (frameCount - 1);
			if (!isnormal(t))
				t = 0;
			if (!scene_prep_frame(&scene, t)) {
				fprintf(stderr, "Error: Unable to prepare the frame. Render cancelled.\n");
				return displayUsage();
			}

			// Clear image buffer
			image_fill(&image, (Color) { .r = 0, .g = 0, .b = 0 });

			// Perform raycasting
			long workload = raycast_image(workers, &image, &scene, WORKER_THREADS);

			double progress;
			do {
				progress = raycast_get_progress(workers, WORKER_THREADS, workload);
				printf("Casted %lf%%..\n", progress * 100);
				Sleep(100);
			} while (isfinite(progress));
			

			// Write image to file
			if (!ppm_write(frameFilename, &image)) {
				fprintf(stderr, "Error: Unable to write the output file. Render cancelled.\n");
				return displayUsage();
			}
		}

		// Terminate worker threads
		if (!raycast_terminate_workers(workers, WORKER_THREADS)) {
			fprintf(stderr, "Error: Could not terminate worker threads. Render cancelled.\n");
			return displayUsage();
		}
		printf("Ray casting workers terminated.\n");
	}
	else {
		return displayUsage();
	}

	lowpImage = malloc(sizeof(LowpColor) * image.header.imageHeight * image.header.imageWidth);
	image_copy_lowp(&image, lowpImage);

	char windowTitle[1000];

	GLint program_id, position_slot, color_slot, trans_slot, rot_slot, scale_slot, shear_slot, tex_slot, tex_coord_slot;
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
	window = glfwCreateWindow(image.header.imageWidth,
		image.header.imageHeight,
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
	tex_coord_slot = glGetAttribLocation(program_id, "SourceTextureCoordinate");
	trans_slot = glGetUniformLocation(program_id, "Translation");
	rot_slot = glGetUniformLocation(program_id, "Rotation");
	scale_slot = glGetUniformLocation(program_id, "Scale");
	shear_slot = glGetUniformLocation(program_id, "Shear");
	glEnableVertexAttribArray(position_slot);
	glEnableVertexAttribArray(color_slot);
	glEnableVertexAttribArray(tex_coord_slot);

	// Create Buffer
	glGenBuffers(1, &vertex_buffer);

	// Map GL_ARRAY_BUFFER to this buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	// Send the data
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// Textures
	glGenTextures(1, &tex_slot);
	glBindTexture(GL_TEXTURE_2D, tex_slot);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.header.imageWidth, image.header.imageHeight, 0, GL_RGBA, GL_FLOAT, lowpImage);

	// Setup manipulation variables
	float trans[2] = { 0, 0 };
	float scale[2] = { 1, 1 };
	float shear[2] = { 0, 0 };
	float rotation = 0;

	// Repeat
	while (!glfwWindowShouldClose(window)) {

		// Horizontal Translation
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			trans[0] -= 0.01f;
		}
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			trans[0] += 0.01f;
		}

		// Vertical Translation
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			trans[1] += 0.01f;
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			trans[1] -= 0.01f;
		}

		// Rotation
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			rotation += 0.01f;
		}
		else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			rotation -= 0.01f;
		}

		// Horizontal Scale
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			scale[0] *= 1.005f;
		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			scale[0] /= 1.005f;
		}

		// Vertical Scale
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			scale[1] *= 1.005f;
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			scale[1] /= 1.005f;
		}

		// Horizontal Shear
		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
			shear[0] += 0.01f;
		}
		else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
			shear[0] -= 0.01f;
		}

		// Vertical Shear
		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
			shear[1] -= 0.01f;
		}
		else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
			shear[1] += 0.01f;
		}

		sprintf_s(windowTitle, 1000, "Image Viewer - %s", imageFilename);
		glfwSetWindowTitle(window, windowTitle);

		glUniform2f(trans_slot, trans[0], trans[1]);
		glUniform2f(scale_slot, scale[0], scale[1]);
		glUniform2f(shear_slot, shear[0], shear[1]);
		glUniform1f(rot_slot, rotation);

		glClearColor(0, 0, 0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, image.header.imageWidth, image.header.imageHeight);

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

		glVertexAttribPointer(tex_coord_slot,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertex),
			(GLvoid*)(sizeof(float) * 7));

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