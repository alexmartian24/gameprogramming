#include "LevelC.h"
#include "Utilities.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8
#define ENEMY_COUNT 2

const char FLYING_ENEMY_FILEPATH[] = "img/stalkette.png",
            WALKING_ENEMY_FILEPATH[] = "img/zombie.png";

unsigned int LEVELC_DATA[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

LevelC::~LevelC()
{
    delete[] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelC::initialise()
{
    GLuint map_texture_id = Utility::load_texture("img/stone.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELC_DATA, map_texture_id, 1.0f, 9, 9);

    GLuint flyer_texture_id = Utility::load_texture(FLYING_ENEMY_FILEPATH);
    GLuint walker_texture_id = Utility::load_texture(WALKING_ENEMY_FILEPATH);
    //flyer 
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(FLYER);
    m_state.enemies[0].set_ai_state(IDLE);
    m_state.enemies[0].m_texture_id = flyer_texture_id;
    m_state.enemies[0].set_position(glm::vec3(4.5f, 1.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(0.0f, 1.0f, 0.0f));
    m_state.enemies[0].set_speed(1.0f);
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, 0.0f, 0.0f)); //fix thsi

    m_state.enemies[0].m_flying[m_state.enemies[1].UP] = new int[4]{ 0, 1 };
    m_state.enemies[0].m_flying[m_state.enemies[1].DOWN] = new int[4]{ 2, 3 };

    m_state.enemies[0].m_animation_indices = m_state.enemies[0].m_flying[m_state.enemies[0].UP];
    m_state.enemies[0].m_animation_frames = 2;
    m_state.enemies[0].m_animation_index = 0;
    m_state.enemies[0].m_animation_time = 0.0f;
    m_state.enemies[0].m_animation_cols = 2;
    m_state.enemies[0].m_animation_rows = 2;
    m_state.enemies[0].set_height(0.4f);
    m_state.enemies[0].set_width(0.4f);

    //GUARD
    m_state.enemies[1].set_entity_type(ENEMY);
    m_state.enemies[1].set_ai_type(GUARD);
    m_state.enemies[1].set_ai_state(IDLE);
    m_state.enemies[1].m_texture_id = walker_texture_id; // i had a separate guard texture but it was too much work
    m_state.enemies[1].set_position(glm::vec3(7.0f, 3.0f, 0.0f));
    m_state.enemies[1].set_movement(glm::vec3(1.0f, 0.0f, 0.0f));
    m_state.enemies[1].set_speed(1.0f);
    m_state.enemies[1].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    m_state.enemies[1].m_walking[m_state.enemies[2].RIGHT] = new int[4]{ 4, 5, 6,  7 };
    m_state.enemies[1].m_walking[m_state.enemies[2].LEFT] = new int[4]{ 8, 9, 10, 11 };
    m_state.enemies[1].m_walking[m_state.enemies[2].UP] = new int[4]{ 12, 13, 14, 15 };
    m_state.enemies[1].m_walking[m_state.enemies[2].DOWN] = new int[4]{ 0, 1, 2, 3 };

    m_state.enemies[1].m_animation_indices = m_state.enemies[2].m_walking[m_state.enemies[2].LEFT];  // start walker lookign left
    m_state.enemies[1].m_animation_frames = 4;
    m_state.enemies[1].m_animation_index = 0;
    m_state.enemies[1].m_animation_time = 0.0f;
    m_state.enemies[1].m_animation_cols = 4;
    m_state.enemies[1].m_animation_rows = 4;
    m_state.enemies[1].set_height(0.8f);
    m_state.enemies[1].set_width(0.8f);

    m_state.player->set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture("assets/george_0.png");

    // Walking
    m_state.player->m_walking[m_state.player->LEFT] = new int[4]{ 1, 5, 9,  13 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4]{ 3, 7, 11, 15 };
    m_state.player->m_walking[m_state.player->UP] = new int[4]{ 2, 6, 10, 14 };
    m_state.player->m_walking[m_state.player->DOWN] = new int[4]{ 0, 4, 8,  12 };

    m_state.player->m_animation_indices = m_state.player->m_walking[m_state.player->RIGHT];  // start George looking left
    m_state.player->m_animation_frames = 4;
    m_state.player->m_animation_index = 0;
    m_state.player->m_animation_time = 0.0f;
    m_state.player->m_animation_cols = 4;
    m_state.player->m_animation_rows = 4;
    m_state.player->set_height(0.8f);
    m_state.player->set_width(0.8f);

    // Jumping
    m_state.player->m_jumping_power = 5.0f;

    /**
     Enemies'

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS("audio/dooblydoo.mp3");
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(0.0f);

    m_state.jump_sfx = Mix_LoadWAV("audio/bounce.wav");
}

void LevelC::update(float delta_time)
{
    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].update(delta_time, m_state.player, NULL, NULL, m_state.map);
    }
}


void LevelC::render(ShaderProgram* program)
{
    m_state.map->render(program);
    m_state.player->render(program);
}