/**
* Author: Alex Martin
* Assignment: Simple 2D Scene
* Date due: 2023-06-11, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <iostream>
#include "glm/mat4x4.hpp"                // 4x4 Matrix
#include "glm/gtc/matrix_transform.hpp"  // Matrix transformation methods
#include "ShaderProgram.h"               // We'll talk about these later in the course
#include "stb_image.h"

const int WINDOW_WIDTH = 640,
		  WINDOW_HEIGHT = 480;

const float BG_RED = 0.1922f,
			BG_BLUE = 0.549f,
			BG_GREEN = 0.9059f,
			BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
		VIEWPORT_Y = 0,
		VIEWPORT_WIDTH = WINDOW_WIDTH,
		VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl";     // make sure not to use std::string objects for these!
const char F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const char SPRITE1[] = "img/ansprite.jpg";
const char SPRITE2[] = "img/theonlycokeido.jpg";

GLuint sprite1_texture_id;
GLuint sprite2_texture_id;

float model_x = -0.1f;
float model_y;
float z_rotate = glm::radians(0.5f);

int g_frame_counter = 0;
bool g_is_growing = true;

float g_previous_ticks = 0.0f;

SDL_Window* displayWindow;
bool gameIsRunning = true;

// New stuff
ShaderProgram g_program;


glm::mat4 g_view_matrix;        // Defines the position (location and orientation) of the camera
glm::mat4 sprite1_model_matrix;
glm::mat4 sprite2_model_matrix;       // Defines every translation, rotation, and/or scaling applied to an object; we'll look at these next week

glm::mat4 g_projection_matrix;  // Defines the characteristics of your camera, such as clip panes, field of view, projection method, etc.

const int TEXTURE_NUM = 1;
const GLint DETAIL_LEVEL = 0;
const GLint TEXTURE_BORDER = 0;


void print_matrix(glm::mat4&, int);

GLuint load_texture(const char* filepath) {
	int width, height, component_num;
	unsigned char* image = stbi_load(filepath, &width, &height, &component_num, STBI_rgb_alpha);
	if (image == NULL) {
		std::cout << "Failed to load texture" << std::endl;
		assert(false);
	}
	GLuint textureID;
	glGenTextures(TEXTURE_NUM, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, DETAIL_LEVEL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);

	return textureID;

}

void initialize() {

	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Project 1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	#ifdef _WINDOWS
		glewInit();
	#endif
	
		glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
		g_program.Load(V_SHADER_PATH, F_SHADER_PATH);

		//std::cout << sprite1_texture_id << std::endl;
		//std::cout << sprite2_texture_id << std::endl;

		g_view_matrix = glm::mat4(1.0f);
		sprite1_model_matrix = glm::mat4(1.0f);
		sprite2_model_matrix = glm::mat4(1.0f);
		//g_model_matrix = glm::rotate(g_model_matrix, INIT_TRIANGLE_ANGLE, glm::vec3(0.0f, 1.0f, 0.0f));
		g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
		
		g_program.SetViewMatrix(g_view_matrix);
		g_program.SetProjectionMatrix(g_projection_matrix);
		
		glUseProgram(g_program.programID);
		glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
		// enable blending

		sprite1_texture_id = load_texture(SPRITE1);
		sprite2_texture_id = load_texture(SPRITE2);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


}

void process_input() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			gameIsRunning = false;
		}
	}
}
void update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;  // get the current number of ticks
	float delta_time = ticks - g_previous_ticks;       // the delta time is the difference from the last frame
	g_previous_ticks = ticks;

	sprite1_model_matrix = glm::translate(sprite1_model_matrix, glm::vec3(model_x, 0.0f, 0.0f));
	sprite2_model_matrix = glm::rotate(sprite2_model_matrix, z_rotate, glm::vec3(0.0f, 0.0f, 0.1f));
	if (model_x < -3) {
		model_x += 0.0001 * delta_time * 0.001f;
	}
	else {
		model_x += -0.0001 * delta_time * 0.001f;
	}
}
void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id){
	g_program.SetModelMatrix(object_model_matrix);
	glBindTexture(GL_TEXTURE_2D, object_texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 6); // we are now drawing 2 triangles, so we use 6 instead of 3
}
void render() {
	// Step 1
	glClear(GL_COLOR_BUFFER_BIT);

	// Step 3
	float vertices[] =
	{
		 3.0f, 0.0f, 4.0f, 0.0f, 4.0f, 1.0f,
		 3.0f, 0.0f, 4.0f, 1.0f, 3.0, 1.0f
	};

	float vertices2[] =
	{
		-3.0, 0.0f, -4.0f, 0.0f, -4.0f, -1.0f,
		-3.0f, 0.0f, -4.0f, -1.0f, -3.0f, -1.0f
	};
	float texture_coordinates[] = {
	   0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,     // triangle 1
	   0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,     // triangle 2
	};

	glVertexAttribPointer(g_program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(g_program.positionAttribute);

	glVertexAttribPointer(g_program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates);
	glEnableVertexAttribArray(g_program.texCoordAttribute);
	
	draw_object(sprite1_model_matrix, sprite1_texture_id);

	glDisableVertexAttribArray(g_program.positionAttribute);
	glDisableVertexAttribArray(g_program.texCoordAttribute);

	glVertexAttribPointer(g_program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
	glEnableVertexAttribArray(g_program.positionAttribute);

	glVertexAttribPointer(g_program.texCoordAttribute, 2, GL_FLOAT, false, 0, texture_coordinates);
	glEnableVertexAttribArray(g_program.texCoordAttribute);
	
	draw_object(sprite2_model_matrix, sprite2_texture_id);

	glDisableVertexAttribArray(g_program.positionAttribute);
	glDisableVertexAttribArray(g_program.texCoordAttribute);


	// Step 4
	SDL_GL_SwapWindow(displayWindow);

}
void shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	std::cout << "starting" << std::endl;
	initialize();

	while (gameIsRunning) {
		process_input();
		update();
		render();
	}
	shutdown();

	return 0;
}