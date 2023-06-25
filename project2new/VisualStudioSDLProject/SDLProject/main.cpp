/**
* Author: Alex Martin
* Assignment: Pong Clone
* Date due: 2023-06-25, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <cmath>

const int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

const float BG_RED = 0.0f,
BG_BLUE = 0.52f,
BG_GREEN = 0.40f,
BG_OPACITY = 1.0f;
//TO DO: see if you can make the background a tennis court image

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const char BALL_SPRITE[] = "sprites/ball.png";
const char RACKET_SPRITE[] = "sprites/racket.png";

const glm::vec3 BALL_INIT_POS = glm::vec3(0.1f, 0.0f, 0.0f),
BALL_INIT_SCA = glm::vec3(1.0f, 1.0f, 0.0f);
const glm::vec3 RACKET_INIT_POS = glm::vec3(-4.8f, 0.0f, 0.0f),
RACKET_INIT_SCA = glm::vec3(4.0f, 4.0f, 4.0f);

const glm::vec3 RACKET2_INIT_POS = glm::vec3(4.8f, 0.0f, 0.0f),
RACKET2_INIT_SCA = glm::vec3(4.0f, 4.0f, 4.0f);

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0,
TEXTURE_BORDER = 0;

const float MILLISECONDS_IN_SECOND = 1000.0;

bool bounce = true;

SDL_Window* g_display_window;
bool g_game_is_running = true;

ShaderProgram g_ball_program;
GLuint        g_ball_texture_id;

ShaderProgram g_racket_program;
GLuint        g_racket_texture_id;

glm::mat4 g_view_matrix,
g_ball_model_matrix,
g_racket_model_matrix,
g_racket2_model_matrix,
g_projection_matrix;

float g_previous_ticks = 0.0f;
//float g_rot_angle = 0.0f;
float g_speed = 1.0f;

glm::vec3 g_ball_movement = glm::vec3(0.0f, 0.0f, 0.0f),
g_ball_position = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_racket_movement = glm::vec3(0.0f, 0.0f, 0.0f),
g_racket_position = glm::vec3(0.0f, 0.0f, 0.0f),
g_racket2_movement = glm::vec3(0.0f, 0.0f, 0.0f),
g_racket2_position = glm::vec3(0.0f, 0.0f, 0.0f);


GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(image);

    return textureID;
}


void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("User input exercise",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    // ———————————————— ball ———————————————— //
    g_ball_program.Load(V_SHADER_PATH, F_SHADER_PATH);

    g_ball_model_matrix = glm::mat4(1.0f);
    g_ball_model_matrix = glm::translate(g_ball_model_matrix, BALL_INIT_POS);
    g_ball_model_matrix = glm::scale(g_ball_model_matrix, BALL_INIT_SCA);

    g_ball_program.SetProjectionMatrix(g_projection_matrix);
    g_ball_program.SetViewMatrix(g_view_matrix);

    glUseProgram(g_ball_program.programID);
    g_ball_texture_id = load_texture(BALL_SPRITE);

    // ———————————————— rackets ———————————————— //
    g_racket_program.Load(V_SHADER_PATH, F_SHADER_PATH);

    g_racket_model_matrix = glm::mat4(1.0f);
    g_racket_model_matrix = glm::translate(g_racket_model_matrix, RACKET_INIT_POS);
    g_racket_model_matrix = glm::scale(g_racket_model_matrix, RACKET_INIT_SCA);

    g_racket2_model_matrix = glm::mat4(1.0f);
    g_racket2_model_matrix = glm::translate(g_racket2_model_matrix, RACKET2_INIT_POS);
    g_racket2_model_matrix = glm::scale(g_racket2_model_matrix, RACKET2_INIT_SCA);

    g_racket_program.SetProjectionMatrix(g_projection_matrix);
    g_racket_program.SetViewMatrix(g_view_matrix);


    glUseProgram(g_racket_program.programID);
    g_racket_texture_id = load_texture(RACKET_SPRITE);

    g_ball_movement.y = -1.0f;
    g_ball_movement.x = -1.0f;

    // ———————————————— GENERAL ———————————————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
}

void process_input()
{
    //maybe add like a start game function or osmething
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = !g_game_is_running;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                g_game_is_running = !g_game_is_running;
                break;

            default: break;
            }
        }
    }
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_UP] && g_racket_position[1] <= 0.5f)
    {
        g_racket_movement.y = 0.5f;
    }
    else if (key_state[SDL_SCANCODE_DOWN] && g_racket_position[1] >= -1.5f)
    {
        g_racket_movement.y = -0.5f;
    }
    if (glm::length(g_racket_movement) > 1.0f)
    {
        g_racket_movement = glm::normalize(g_racket_movement);
    }


    if (key_state[SDL_SCANCODE_W] && g_racket2_position[1] <= 0.5f)
    {
        g_racket2_movement.y = 0.5f;
    }
    else if (key_state[SDL_SCANCODE_S] && g_racket2_position[1] >= -1.5f)
    {
        g_racket2_movement.y = -0.5f;
    }
    if (glm::length(g_racket2_movement) > 1.0f)
    {
        g_racket2_movement = glm::normalize(g_racket2_movement);
    }
}
bool zero(float x, float y) {
    if (x < 0 && x < y) {
        return true;
    }
    return false;
}
void update()
{
    /** ———— COLLISION DETECTION ———— **/
    float collision_factor = 0.007;

    float racket_x_distance = fabs(g_racket_position.x - g_ball_position.x) - ((RACKET_INIT_SCA.x * collision_factor + BALL_INIT_SCA.x * collision_factor) / 2.0f);
    float racket_y_distance = fabs(g_racket_position.y - g_ball_position.y) - ((RACKET_INIT_SCA.y * collision_factor + BALL_INIT_SCA.y * collision_factor) / 2.0f);

    float racket2_x_distance = fabs(g_racket2_position.x - g_ball_position.x) - ((RACKET2_INIT_SCA.x * collision_factor + BALL_INIT_SCA.x * collision_factor) / 2.0f);
    float racket2_y_distance = fabs(g_racket2_position.y - g_ball_position.y) - ((RACKET2_INIT_SCA.y * collision_factor + BALL_INIT_SCA.y * collision_factor) / 2.0f);

    if (g_ball_position.x > 4.5f) {
        g_ball_position.x = 4.5f;
        g_ball_movement.x *= -1.0f;

    }
    else if (g_ball_position.x < -4.7f) {
        g_ball_position.x = -4.7f;
        g_ball_movement.x *= -1.0f;

    }
    if (g_ball_position.y > 3.3f) {
        g_ball_position.y = 3.3f;
        g_ball_movement.y *= -1.0f;
    }
    else if (g_ball_position.y < -3.5f) {
        g_ball_position.y = -3.5f;
        g_ball_movement.y *= -1.0f;
    }

    if (zero(racket_x_distance, racket_y_distance) || zero(racket2_x_distance, racket2_y_distance)) {
        g_ball_position.x = 3.3f;
        g_ball_position.y = 2.2f;
        g_game_is_running = false;

    }
    //TODO: check for paddle and ball collision
    //TODO: check for wall and ball collision

    // ———————————————— DELTA TIME CALCULATIONS ———————————————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    // ———————————————— RESETTING MODEL MATRIX ———————————————— //
    g_ball_model_matrix = glm::mat4(1.0f);
    g_ball_model_matrix = glm::translate(g_ball_model_matrix, BALL_INIT_POS);
    g_ball_model_matrix = glm::scale(g_ball_model_matrix, BALL_INIT_SCA);

    g_racket_model_matrix = glm::mat4(1.0f);
    g_racket_model_matrix = glm::translate(g_racket_model_matrix, RACKET_INIT_POS);
    g_racket_model_matrix = glm::scale(g_racket_model_matrix, RACKET_INIT_SCA);

    g_racket2_model_matrix = glm::mat4(1.0f);
    g_racket2_model_matrix = glm::translate(g_racket2_model_matrix, RACKET2_INIT_POS);
    g_racket2_model_matrix = glm::scale(g_racket2_model_matrix, RACKET2_INIT_SCA);

    // ———————————————— TRANSLATIONS ———————————————— //
    g_racket_position += g_racket_movement * g_speed * delta_time;
    g_racket_model_matrix = glm::translate(g_racket_model_matrix, g_racket_position);
    g_racket_movement = glm::vec3(0.0f, 0.0f, 0.0f);

    g_racket2_position += g_racket2_movement * g_speed * delta_time;
    g_racket2_model_matrix = glm::translate(g_racket2_model_matrix, g_racket2_position);
    g_racket2_movement = glm::vec3(0.0f, 0.0f, 0.0f);


    // ———————————————— BALL MOVEMENT ———————————————— //

    g_ball_position += g_ball_movement * g_speed * delta_time;
    g_ball_model_matrix = glm::translate(g_ball_model_matrix, g_ball_position);
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    // ———————————————— ball ———————————————— //
    float ball_vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    float ball_texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };

    glVertexAttribPointer(g_ball_program.positionAttribute, 2, GL_FLOAT, false, 0, ball_vertices);
    glEnableVertexAttribArray(g_ball_program.positionAttribute);

    glVertexAttribPointer(g_ball_program.texCoordAttribute, 2, GL_FLOAT, false, 0, ball_texture_coordinates);
    glEnableVertexAttribArray(g_ball_program.texCoordAttribute);

    g_ball_program.SetModelMatrix(g_ball_model_matrix);
    glBindTexture(GL_TEXTURE_2D, g_ball_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(g_ball_program.positionAttribute);
    glDisableVertexAttribArray(g_ball_program.texCoordAttribute);

    // ———————————————— racket ———————————————— //
    float racket_vertices[] = {
        -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f
    };

    float racket_texture_coordinates[] = {
        0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    };

    glVertexAttribPointer(g_racket_program.positionAttribute, 2, GL_FLOAT, false, 0, racket_vertices);
    glEnableVertexAttribArray(g_racket_program.positionAttribute);

    glVertexAttribPointer(g_racket_program.texCoordAttribute, 2, GL_FLOAT, false, 0, racket_texture_coordinates);
    glEnableVertexAttribArray(g_racket_program.texCoordAttribute);

    g_racket_program.SetModelMatrix(g_racket_model_matrix);
    glBindTexture(GL_TEXTURE_2D, g_racket_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // ———————————————— racket2 ———————————————— //


    glVertexAttribPointer(g_racket_program.positionAttribute, 2, GL_FLOAT, false, 0, racket_vertices);
    glEnableVertexAttribArray(g_racket_program.positionAttribute);

    glVertexAttribPointer(g_racket_program.texCoordAttribute, 2, GL_FLOAT, false, 0, racket_texture_coordinates);
    glEnableVertexAttribArray(g_racket_program.texCoordAttribute);

    g_racket_program.SetModelMatrix(g_racket2_model_matrix);
    glBindTexture(GL_TEXTURE_2D, g_racket_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(g_racket_program.positionAttribute);
    glDisableVertexAttribArray(g_racket_program.texCoordAttribute);

    // ———————————————— GENERAL ———————————————— //
    SDL_GL_SwapWindow(g_display_window);


}

void shutdown() { SDL_Quit(); }

/**
 Start here—we can see the general structure of a game loop without worrying too much about the details yet.
 */
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}
