#ifndef ENTITY_H
#define ENTITY_H
#define ACC_OF_GRAVITY -9.81f //prob not supposed to do this for a variety of reasons but oh well  ¯\_(ツ)_/¯
#define FUEL 100

enum EntityType { PLATFORM, PLAYER, MESSAGE, GOAL };

class Entity
{
private:
    bool m_is_active = true,
        winner_flag = false,
        loser_flag = false;

    int* m_animation_right = NULL, // move to the right
        * m_animation_left = NULL, // move to the left
        * m_animation_up = NULL, // move upwards
        * m_animation_down = NULL; // move downwards

    glm::vec3 m_position;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    int m_width = 1,
        m_height = 1;

public:
    // ————— STATIC VARIABLES ————— //
    static const int SECONDS_PER_FRAME = 4;
    static const int LEFT = 0,
        RIGHT = 1,
        UP = 2,
        DOWN = 3;

    EntityType type;
    // ————— ANIMATION ————— //
    int** m_walking = new int* [4]
    {
        m_animation_left,
        m_animation_right,
        m_animation_up,
        m_animation_down
    };

    int m_animation_frames = 0,
        m_animation_index = 0,
        m_animation_cols = 0,
        m_animation_rows = 0;

    int* m_animation_indices = NULL;
    float m_animation_time = 0.0f;

    // ————— TRANSFORMATIONS ————— //
    float     m_speed;
    glm::vec3 m_movement;
    glm::mat4 m_model_matrix;

    GLuint    m_texture_id;

    // ––––– PHYSICS (COLLISIONS) ––––– //
    bool m_collided_top = false;
    bool m_collided_bottom = false;
    bool m_collided_left = false;
    bool m_collided_right = false;
    
    bool m_is_flying = false;
    bool m_is_moving_left = false;
    bool m_is_moving_right = false;
    float m_flying_power = 0;

    // ————— METHODS ————— //
    Entity();
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    void const check_collision_x(Entity* other, int);
    void const check_collision_y(Entity* other, int);
    bool const check_collision(Entity* other) const;

    void update(float delta_time, Entity* collidable_entities, int entity_count);
    void render(ShaderProgram* program);

    void activate() { m_is_active = true; };
    void deactivate() { m_is_active = false; };

    // ————— GETTERS ————— //
    glm::vec3 const get_position()     const { return m_position; };
    glm::vec3 const get_velocity()     const { return m_velocity; };
    glm::vec3 const get_acceleration() const { return m_acceleration; };
    glm::vec3 const get_movement()     const { return m_movement; };
    
    bool const get_win() const { return winner_flag; }
    bool const get_lose() const { return loser_flag; }

        //———— SETTERS ————— //
    void const set_position(glm::vec3 new_position) { m_position = new_position; };
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; };
    void const set_acceleration(glm::vec3 new_accel) { m_acceleration = new_accel; };
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; };
    void const lose(bool lose) { loser_flag = lose; }
    void const win(bool win) { winner_flag = win; }
};
#endif