#include "LevelA.h"
#include "Utilities.h"

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8

const char  SPRITESHEET_FILEPATH[] = "img/king.png",
            MAP_TILESET_FILEPATH[] = "img/stone.png",
            TEXT_FILEPATH[] = "img/font1.png",
            BGM_FILEPATH[] = "audio/Boogie Party.mp3",
            JUMP_SFX_FILEPATH[] = "audio/jump.wav",
            FLYING_ENEMY_FILEPATH[] = "img/stalkette.png",
            WALKING_ENEMY_FILEPATH[] = "img/zombie.png",
            BOOMER_FILEPATH[] = "img/boomer.png",
            HEARTS_FILEPATH[] = "img/hearts.png";

unsigned int LEVELA_DATA[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
    3, 1, 1, 1, 1, 1, 0, 1, 1, 2, 2, 2, 2, 2,
    3, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 2
};
unsigned int LEVEL_1_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 1, 1, 1, 1,
    45, 46, 46, 0, 0, 1, 55, 55, 55, 55, 55, 55, 55, 56
};
LevelA::~LevelA()
{
    delete[] m_state.enemies;
    delete    m_state.player;
    delete    m_state.map;
    Mix_FreeChunk(m_state.jump_sfx);
    Mix_FreeMusic(m_state.bgm);
}

void LevelA::initialise()
{
    GLuint map_texture_id = Utility::load_texture("img/rock.png");
    m_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVELA_DATA, map_texture_id, 1.0f, 9, 9);
    // ————— ENEMIES SET-UP ————— //
         //enemy1
    GLuint flyer_texture_id = Utility::load_texture(FLYING_ENEMY_FILEPATH);
    GLuint walker_texture_id = Utility::load_texture(WALKING_ENEMY_FILEPATH);
    GLuint heart_texture_id = Utility::load_texture(HEARTS_FILEPATH);

    m_state.enemies = new Entity[ENEMY_COUNT];
    m_state.enemies[0].set_entity_type(ENEMY);
    m_state.enemies[0].set_ai_type(WALKER);
    m_state.enemies[0].set_ai_state(IDLE);
    m_state.enemies[0].m_texture_id = walker_texture_id;
    m_state.enemies[0].set_position(glm::vec3(7.0f, 0.0f, 0.0f));
    m_state.enemies[0].set_movement(glm::vec3(1.0f, 0.0f, 0.0f));
    m_state.enemies[0].set_speed(1.0f);
    m_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    m_state.enemies[0].m_walking[m_state.enemies[0].RIGHT] = new int[4]{ 4, 5, 6,  7 };
    m_state.enemies[0].m_walking[m_state.enemies[0].LEFT] = new int[4]{ 8, 9, 10, 11 };
    m_state.enemies[0].m_walking[m_state.enemies[0].UP] = new int[4]{ 12, 13, 14, 15 };
    m_state.enemies[0].m_walking[m_state.enemies[0].DOWN] = new int[4]{ 0, 1, 2, 3 };

    m_state.enemies[0].m_animation_indices = m_state.enemies[0].m_walking[m_state.enemies[0].RIGHT];  // start walker lookign left
    m_state.enemies[0].m_animation_frames = 4;
    m_state.enemies[0].m_animation_index = 0;
    m_state.enemies[0].m_animation_time = 0.0f;
    m_state.enemies[0].m_animation_cols = 4;
    m_state.enemies[0].m_animation_rows = 4;
    m_state.enemies[0].set_height(0.8f);
    m_state.enemies[0].set_width(0.8f);

    // ————— GEORGE SET-UP ————— //
    // Existing
    m_state.player = new Entity();
    m_state.player->set_entity_type(PLAYER);
    m_state.player->set_position(glm::vec3(3.0f, 0.0f, 0.0f));
    m_state.player->set_movement(glm::vec3(0.0f));
    m_state.player->set_speed(2.5f);
    m_state.player->set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_state.player->m_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);

    // Walking
    m_state.player->m_walking[m_state.player->LEFT] = new int[4]{ 4, 5, 6,  7 };
    m_state.player->m_walking[m_state.player->RIGHT] = new int[4]{ 8, 9, 10, 11 };
    m_state.player->m_walking[m_state.player->UP] = new int[4]{ 12, 13, 14, 15 };
    m_state.player->m_walking[m_state.player->DOWN] = new int[4]{ 0, 1, 2, 3 };

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
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_state.bgm = Mix_LoadMUS(BGM_FILEPATH);
    Mix_PlayMusic(m_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4.0f);

    m_state.jump_sfx = Mix_LoadWAV(JUMP_SFX_FILEPATH);
}
void LevelA::update(float delta_time)
{

    m_state.player->update(delta_time, m_state.player, m_state.enemies, ENEMY_COUNT, m_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_state.enemies[i].update(delta_time, m_state.player, NULL, NULL, m_state.map);
    }
    if (m_state.player->get_position().y < -10.0f) m_state.next_scene_id = 1;
}


void LevelA::render(ShaderProgram* program)
{
    m_state.map->render(program);
    m_state.player->render(program);

    GLuint font_id = Utility::load_texture(TEXT_FILEPATH);
    if (m_state.player->win_flag) {
        Utility::draw_text(program, font_id, "YOU WIN!", .5f, 0.01f, m_state.player->get_position());
    }
    if (m_state.player->lose_flag) {
        Utility::draw_text(program, font_id, "YOU LOSE!", .5f, 0.01f, m_state.player->get_position());
       m_state.player->deactivate();
    }
    for (int i = 0; i < m_number_of_enemies; i++)
        m_state.enemies[i].render(program);
}