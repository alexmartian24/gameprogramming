/**
* Author: Alex Martin
* Assignment: Lunar Lander
* Date due: 2023-07-07, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define LOG(argument) std::cout << argument << '\n'
#define STB_IMAGE_IMPLEMENTATION
#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define NUMBER_OF_ENEMIES 3
#define FIXED_TIMESTEP 0.0166666f
#define ACC_OF_GRAVITY -9.81f
#define PLATFORM_COUNT 3

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"

// ����� STRUCTS AND ENUMS �����//
struct GameState
{
    Entity* player;
    Entity* platforms;
    Entity* goal;
    Entity* message;
};

// ����� CONSTANTS ����� //
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

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;
const char SPRITESHEET_FILEPATH[] = "sprites/george_0.png",
PLATFORM_FILEPATH[] = "sprites/platformPack_tile027.png",
GAME_OVER[] = "sprites/failure.jpg",
CHICKEN_DINNER[] = "sprites/success.jpg",
LANDING_PAD[] = "sprites/flowerbunch.png";


const int NUMBER_OF_TEXTURES = 1;  // to be generated, that is
const GLint LEVEL_OF_DETAIL = 0;  // base image level; Level n is the nth mipmap reduction image
const GLint TEXTURE_BORDER = 0;  // this value MUST be zero

// ����� VARIABLES ����� //
GameState g_state;

SDL_Window* g_display_window;
bool g_game_is_running = true;
bool loser_flag = false;
bool winner_flag = false; //winner winner chicken DINNER

ShaderProgram g_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_time_accumulator = 0.0f;

// ���� GENERAL FUNCTIONS ���� //
GLuint load_texture(const char* filepath)
{
    // STEP 1: Loading the image file
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components, STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    // STEP 2: Generating and binding a texture ID to our image
    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER, GL_RGBA, GL_UNSIGNED_BYTE, image);

    // STEP 3: Setting our texture filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // STEP 4: Setting our texture wrapping modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // the last argument can change depending on what you are looking for
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // STEP 5: Releasing our file from memory and returning our texture id
    stbi_image_free(image);

    return textureID;
}

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("Hello, Physics!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_program.Load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);  // Defines the position (location and orientation) of the camera
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);  // Defines the characteristics of your camera, such as clip planes, field of view, projection method etc.

    g_program.SetProjectionMatrix(g_projection_matrix);
    g_program.SetViewMatrix(g_view_matrix);

    glUseProgram(g_program.programID);

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // ����� PLAYER ����� //
    g_state.player = new Entity();
    g_state.player->set_position(glm::vec3(0.0f));
    g_state.player->set_movement(glm::vec3(0.0f));
    g_state.player->set_acceleration(glm::vec3(0.0f, ACC_OF_GRAVITY * 0.1, 0.0f));
    g_state.player->m_speed = 1.0f;
    g_state.player->m_texture_id = load_texture(SPRITESHEET_FILEPATH);

    g_state.player->m_walking[g_state.player->LEFT] = new int[4]{ 1, 5, 9,  13 };
    g_state.player->m_walking[g_state.player->RIGHT] = new int[4]{ 3, 7, 11,  15 };
    g_state.player->m_walking[g_state.player->UP] = new int[4]{ 2, 6, 10, 14 };
    g_state.player->m_walking[g_state.player->DOWN] = new int[4]{ 0, 4, 8,  12 };

    g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->RIGHT];  // start George looking right
    g_state.player->m_animation_frames = 4;
    g_state.player->m_animation_index = 0;
    g_state.player->m_animation_time = 0.0f;
    g_state.player->m_animation_cols = 4;
    g_state.player->m_animation_rows = 4;

    

    // ����� PLATFORM ����� //
    g_state.platforms = new Entity[PLATFORM_COUNT];

    for (int i = 0; i < PLATFORM_COUNT; i++)
    {
        g_state.platforms[i].m_texture_id = load_texture(PLATFORM_FILEPATH);
        g_state.platforms[i].set_position(glm::vec3(i - 1.0f, -3.0f, 0.0f));
        g_state.platforms[i].update(0.0f, NULL, 0);
    }

    g_state.goal = new Entity;
    g_state.goal->type = GOAL;
    g_state.goal->m_texture_id = load_texture(LANDING_PAD);
    g_state.goal->set_position(glm::vec3(-3.0f, -3.0f, 0.0f));
    g_state.goal->update(0.0f, NULL, 0);
    
    //messages//
    
    g_state.message = new Entity[2]; 
    
    g_state.message[0].deactivate();
    g_state.message[0].m_texture_id = load_texture(GAME_OVER);
    g_state.message[0].set_position(glm::vec3(1.0f, 3.0f, 0.0f));
    g_state.message[0].update(0.0f, NULL, 0);

    g_state.message[1].deactivate();
    g_state.message[1].m_texture_id = load_texture(CHICKEN_DINNER);
    g_state.message[1].set_position(glm::vec3(1.0f, 3.0f, 0.0f));
    g_state.message[1].update(0.0f, NULL, 0);

    // ����� GENERAL ����� //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    // VERY IMPORTANT: If nothing is pressed, we don't want to go anywhere
    g_state.player->set_movement(glm::vec3(0.0f));

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_q:
                // Quit the game with a keystroke
                g_game_is_running = false;
                break;

            default:
                break;
            }

        default:
            break;
        }
    }

    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_LEFT])
    {

        g_state.player->m_is_moving_left = true;
        g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->LEFT];
    }
    else if (key_state[SDL_SCANCODE_RIGHT])
    {
        g_state.player->m_is_moving_right = true;
        g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->RIGHT];
    }
    else if (key_state[SDL_SCANCODE_UP])
    {
        g_state.player->m_is_flying = true;
        g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->UP];
    }
    else if (key_state[SDL_SCANCODE_DOWN])
    {
        //g_state.player->m_is_flying = true;
        g_state.player->m_animation_indices = g_state.player->m_walking[g_state.player->DOWN];
    }
    // This makes sure that the player can't move faster diagonally
    if (glm::length(g_state.player->m_movement) > 1.0f)
    {
        g_state.player->m_movement = glm::normalize(g_state.player->m_movement);
    }
}


void update()
{
    
    // ����� DELTA TIME ����� //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND; // get the current number of ticks
    float delta_time = ticks - g_previous_ticks; // the delta time is the difference from the last frame
    g_previous_ticks = ticks;

    // ����� FIXED TIMESTEP ����� //
    // STEP 1: Keep track of how much time has passed since last step
    delta_time += g_time_accumulator;

    // STEP 2: Accumulate the ammount of time passed while we're under our fixed timestep
    if (delta_time < FIXED_TIMESTEP)
    {
        g_time_accumulator = delta_time;
        return;
    }

    // STEP 3: Once we exceed our fixed timestep, apply that elapsed time into the objects' update function invocation
    while (delta_time >= FIXED_TIMESTEP)
    {
        // Notice that we're using FIXED_TIMESTEP as our delta time
        g_state.player->update(FIXED_TIMESTEP, g_state.platforms, PLATFORM_COUNT);
        g_state.player->update(FIXED_TIMESTEP, g_state.goal, 1);
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_time_accumulator = delta_time;
}

void render()
{
    // ����� GENERAL ����� //
    glClear(GL_COLOR_BUFFER_BIT);

    // ����� PLAYER ����� //
    g_state.player->render(&g_program);

    // ����� PLATFORM ����� //
    for (int i = 0; i < PLATFORM_COUNT; i++) g_state.platforms[i].render(&g_program);
    g_state.goal->render(&g_program);
    
    // ����� MESSAGE ����� //
    if (g_state.player->get_lose()) {
        g_state.message[0].render(&g_program);
        
    }
    if (g_state.player->get_win()) {
        g_state.message[1].render(&g_program);        
    }

    // ����� GENERAL ����� //
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown() { SDL_Quit(); }

// ����� DRIVER GAME LOOP ����� /
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