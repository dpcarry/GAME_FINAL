/**
* Author: Pingchuan Dong
* Assignment: Lunar Lander
* Date due: 2024-10-27, 11:59pm
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

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
#include "GlobalState.h"

void Entity::ai_activate(Entity* player, float delta_time)
{
    switch (m_ai_type)
    {
    case BANCHO:
        ai_bancho(player, delta_time);
        break;
    case ZANGIEF:
        ai_zangief(player, delta_time);
        break;
    case RYU:
        ai_ryu(player, delta_time);
        break;
    case FIREBALL:
        ai_fireball(player, delta_time);
    default:
        break;
    }
}

void Entity::ai_bancho(Entity* player, float delta_time)
{
    static float hurt_timer = 0.0f;
    switch (m_ai_state) {
    case IDLE:
        //std::cout << "idle" << m_position.x << " " << m_position.y << std::endl;
        if (m_position.x > player->get_position().x) {
            m_current_animation = STAND_RHS;
            
            set_animation_state(m_current_animation);
        }
        else {
            m_current_animation = STAND_LHS;
            set_animation_state(m_current_animation);
        }
        if (glm::distance(m_position, player->get_position()) < 2.5f) m_ai_state = WALKING;
        break;
    case WALKING:
        // 根据 x 轴的相对位置决定 x 轴的移动方向
        if (m_position.x > player->get_position().x) {
            m_movement = glm::vec3(-1.0f, 0.0f, 0.0f); // 向左移动
            m_current_animation = WALK_LEFT;
            set_animation_state(m_current_animation);
        }
        else {
            m_movement = glm::vec3(1.0f, 0.0f, 0.0f);  // 向右移动
            m_current_animation = WALK_RIGHT;
            set_animation_state(m_current_animation);
        }
        //if (glm::distance(m_position, player->get_position()) < 2.0f) m_ai_state = ATTACKING;
        break;
    case HURT:
        hurt_timer += delta_time;
        if (m_position.x > player->get_position().x) {
            m_movement = glm::vec3(0.0f, 0.0f, 0.0f); // 向左移动
            m_current_animation = ONHIT_RHS;
            set_animation_state(m_current_animation);
        }
        else {
            m_movement = glm::vec3(0.0f, 0.0f, 0.0f);  // 向右移动
            m_current_animation = ONHIT_LHS;
            set_animation_state(m_current_animation);
        }
        if (hurt_timer > 0.8f) { // 假设 0.5 秒后离开 HURT 状态
            hurt_timer = 0.0f;  // 重置计时器
            m_ai_state = IDLE;
        }
        break;
    /*case ATTACKING:
        //std::cout << "attack" << m_position.x << " " << m_position.y << std::endl;
        if (m_position.x > player->get_position().x) {
            m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
            m_speed = 1.2f;
            m_current_animation = ATTACK_LEFT;
            set_animation_state(ATTACK_LEFT);
        }
        else {
            m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
            m_speed = 1.2f;
            m_current_animation = ATTACK_RIGHT;
            set_animation_state(ATTACK_RIGHT);
        }
        if (glm::distance(m_position, player->get_position()) > 3.0f) m_ai_state = IDLE;*/
    }
}

void Entity::ai_zangief(Entity* player, float delta_time)
{
    static float hurt_timer = 0.0f;
    switch (m_ai_state) {
    case IDLE:
        //std::cout << "idle" << m_position.x << " " << m_position.y << std::endl;
        if (m_position.x > player->get_position().x) {
            m_current_animation = STAND_RHS;

            set_animation_state(m_current_animation);
        }
        else {
            m_current_animation = STAND_LHS;
            set_animation_state(m_current_animation);
        }
        if (glm::distance(m_position, player->get_position()) < 3.5f) m_ai_state = WALKING;
        break;
    case WALKING:
        // 根据 x 轴的相对位置决定 x 轴的移动方向
        if (m_position.x > player->get_position().x) {
            m_movement = glm::vec3(-1.0f, 0.0f, 0.0f); // 向左移动
            m_current_animation = WALK_LEFT;
            set_animation_state(m_current_animation);
        }
        else {
            m_movement = glm::vec3(1.0f, 0.0f, 0.0f);  // 向右移动
            m_current_animation = WALK_RIGHT;
            set_animation_state(m_current_animation);
        }
        if (glm::distance(m_position, player->get_position()) < 2.5f) m_ai_state = ATTACKING;
        break;
    case HURT:
        hurt_timer += delta_time;
        if (m_position.x > player->get_position().x) {
            m_movement = glm::vec3(0.0f, 0.0f, 0.0f); // 向左移动
            m_current_animation = ONHIT_RHS;
            set_animation_state(m_current_animation);
        }
        else {
            m_movement = glm::vec3(0.0f, 0.0f, 0.0f);  // 向右移动
            m_current_animation = ONHIT_LHS;
            set_animation_state(m_current_animation);
        }
        if (hurt_timer > 0.8f) { // 假设 0.5 秒后离开 HURT 状态
            hurt_timer = 0.0f;  // 重置计时器
            m_ai_state = IDLE;
        }
        break;
    case ATTACKING:
        //std::cout << "attack" << m_position.x << " " << m_position.y << std::endl;
        if (m_position.x > player->get_position().x) {
            m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
            m_speed = 1.2f;
            m_current_animation = PUNCH_RHS;
            set_animation_state(m_current_animation);
        }
        else {
            m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
            m_speed = 1.2f;
            m_current_animation = PUNCH_LHS;
            set_animation_state(m_current_animation);
        }
        if (glm::distance(m_position, player->get_position()) > 2.5f) m_ai_state = IDLE;
        break;
    }
}

void Entity::ai_ryu(Entity* player, float delta_time)
{
    static float hurt_timer = 0.0f;

    switch (m_ai_state) {
    case IDLE:
        if (m_position.x > player->get_position().x) {
            m_current_animation = STAND_RHS;
            set_animation_state(m_current_animation);
        }
        else {
            m_current_animation = STAND_LHS;
            set_animation_state(m_current_animation);
        }
        if (glm::distance(m_position, player->get_position()) < 5.0f) {
            m_ai_state = WALKING;
        }
        break;

    case WALKING:
        if (m_position.x > player->get_position().x) {
            m_movement = glm::vec3(-1.0f, 0.0f, 0.0f); // 向左移动
            m_current_animation = WALK_LEFT;
        }
        else {
            m_movement = glm::vec3(1.0f, 0.0f, 0.0f);  // 向右移动
            m_current_animation = WALK_RIGHT;
        }
        set_animation_state(m_current_animation);

        if (glm::distance(m_position, player->get_position()) < 2.5f) {
            m_ai_state = ATTACKING;
        }
        else if (glm::distance(m_position, player->get_position()) > 6.0f) {
            m_ai_state = RAPID_APPROACH;
        }
        break;

    case HURT:
        hurt_timer += delta_time;
        if (hurt_timer > 0.8f) {
            hurt_timer = 0.0f;
            m_ai_state = IDLE;
        }
        m_movement = glm::vec3(0.0f, 0.0f, 0.0f); // 向左移动

        m_current_animation = (m_position.x > player->get_position().x) ? ONHIT_RHS : ONHIT_LHS;
        set_animation_state(m_current_animation);
        break;

    case BLOCK:
        if (m_position.x > player->get_position().x) {
            m_current_animation = DEFEND_RHS;
        }
        else {
            m_current_animation = DEFEND_LHS;
        }
        set_animation_state(m_current_animation);
        m_ai_state = IDLE; // 格挡动作完成后回到IDLE
        break;

    case ATTACKING:
        /*if (!m_has_attacked) {
            m_current_animation = (m_position.x > player->get_position().x) ? PUNCH_RHS : PUNCH_LHS;
            set_animation_state(m_current_animation);
            m_has_attacked = true; // Mark attack as initiated
        }

        if (m_animation_index >= m_animation_frames - 1) {
            if (player->m_current_animation == DEFEND_LHS || player->m_current_animation == DEFEND_RHS) {
                m_ai_state = ATTACKING2;
                m_has_attacked = false; // Reset for ATTACKING2
            }
            else if (glm::distance(m_position, player->get_position()) > 4.0f) {
                m_ai_state = RAPID_APPROACH;
                m_has_attacked = false; // Reset for RAPID_APPROACH
            }
            else {
                m_ai_state = IDLE;
                m_has_attacked = false; // Reset for next attack
            }
        }*/
        if (m_position.x > player->get_position().x) {
            m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
            m_speed = 1.2f;
            m_current_animation = PUNCH_RHS;
            set_animation_state(m_current_animation);
        }
        else {
            m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
            m_speed = 1.2f;
            m_current_animation = PUNCH_LHS;
            set_animation_state(m_current_animation);
        }

        if (glm::distance(m_position, player->get_position()) > 2.5f) m_ai_state = IDLE;
  
        break;



    case RAPID_APPROACH:
        m_speed = 3.0f;
        m_movement = (m_position.x > player->get_position().x) ? glm::vec3(-1.0f, 0.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
        m_current_animation = (m_position.x > player->get_position().x) ? RAPID_RHS : RAPID_LHS;
        set_animation_state(m_current_animation);

        if (glm::distance(m_position, player->get_position()) <= 2.0f && !m_has_attacked) {
            m_ai_state = IDLE;
            m_has_attacked = false;
        }
        break;
    }
}


void Entity::ai_fireball(Entity* player, float delta_time) {
    // If fireball is inactive, do nothing
    switch (m_ai_state) {
    case IDLE:
        //std::cout << "idle" << m_position.x << " " << m_position.y << std::endl;
        m_movement = glm::vec3(0.0f, 0.0F, 0.0f);
        if (player->m_current_animation == SHOOT_LHS|| player->m_current_animation == SHOOT_RHS) {

            m_ai_state = ATTACKING;
            m_position = player->get_position();
            activate();
            std::cout << m_is_active << std::endl;
        }
        break;
    case ATTACKING:
        if (player->m_current_animation == SHOOT_LHS) {
            m_current_animation = STAND_LHS;
            set_animation_state(m_current_animation);
            m_movement = glm::vec3(1.0f, 0.0f, 0.0f);
            //std::cout << "shooting" << m_position.x << " " << m_position.y << std::endl;
        }
        else if (player->m_current_animation == SHOOT_RHS) {
            m_current_animation = STAND_RHS;
            set_animation_state(m_current_animation);
            m_movement = glm::vec3(-1.0f, 0.0f, 0.0f);
            //std::cout << "shooting" << m_position.x << " " << m_position.y << std::endl;

        }

        break;
    }
}


// Default constructor
Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f), m_current_animation(STAND_LHS),
    m_texture_ids(0), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 6; ++j) m_walking[i][j] = 0;
}

// Parameterized constructor for player
Entity::Entity(std::vector<GLuint> texture_ids, float speed, glm::vec3 acceleration, float jump_power, int walking[4][6], std::vector<std::vector<int>> animations, float animation_time,
    int animation_frames, int animation_index, int animation_cols,
    int animation_rows, float width, float height, EntityType EntityType)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_acceleration(acceleration), m_jumping_power(jump_power), m_animation_cols(animation_cols),
    m_animation_frames(animation_frames), m_animation_index(animation_index),
    m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_current_animation(STAND_RHS), m_texture_ids(texture_ids), m_animations(animations), m_velocity(0.0f),
    m_width(width), m_height(height), m_entity_type(EntityType)
{
    face_right();
    set_walking(walking);
    set_animation_state(m_current_animation);
}

// Platform Constructor
Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType)
    : m_position(glm::vec3(0.0f, 3.50f, 0.0f)), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height), m_entity_type(EntityType)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 6; ++j) m_walking[i][j] = 0;
}


//ai constructor
Entity::Entity(std::vector<GLuint> texture_ids, float speed, float width, float height, std::vector<std::vector<int>> animations, EntityType EntityType, AIType AIType, AIState AIState) : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
m_animation_rows(1), m_animation_indices(nullptr), m_animation_time(0.0f), m_current_animation(STAND_RHS),
m_texture_ids(texture_ids), m_animations(animations), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height), m_entity_type(EntityType), m_ai_type(AIType), m_ai_state(AIState)
{
    // Initialize m_walking with zeros or any default value
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 6; ++j) m_walking[i][j] = 0;
    set_animation_state(m_current_animation);
}

Entity::~Entity() { }

void Entity::set_animation_state(Animation new_animation)
{

    m_current_animation = new_animation;
    m_animation_indices = m_animations[m_current_animation].data();

    //std::cout << m_entity_type << m_animations[m_current_animation].data() << std::endl;
    // Update the texture and animation indices based on the current animation
    m_animation_frames = m_animations[new_animation].size();
    m_animation_cols = m_animations[new_animation].size();

    /*else if (m_entity_type == ENEMY) {
        if (m_ai_type == BANCHO) {
            // std::cout << "fly" << std::endl; yes
            //std::cout << m_current_animation << std::endl; 0
            if (new_animation == )
            {
                m_animation_frames = m_animations[2].size();
                m_animation_cols = m_animations[2].size();
            }
            else if (new_animation == STAND_RHS)
            {
                m_animation_frames = m_animations[0].size();
                m_animation_cols = m_animations[0].size();
                //std::cout << m_animation_frames << "  " << m_animation_cols << std::endl;
            }
            else if (new_animation == MOVE_RIGHT)
            {
                m_animation_frames = m_animations[4].size();
                m_animation_cols = m_animations[4].size();
                //std::cout << m_animation_frames << "  " << m_animation_cols << std::endl;
            }
        }
    }*/



}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program)
{
    GLuint current_texture = NULL;
    if (m_entity_type != PLATFORM) {
        current_texture = m_texture_ids[m_current_animation];  // Get the right texture
        //std::cout << "Drawing entity of type: " << m_entity_type
          //<< ", Animation: " << m_current_animation
            //<< ", Texture ID: " << current_texture << ", Animation Index" << m_animation_index << std::endl;

    }


    //Platform
    else {
        current_texture = m_texture_id;
    }

    float u_coord = (float)(m_animation_index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(m_animation_index / m_animation_cols) / (float)m_animation_rows;

    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;


    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width,
        v_coord, u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };

    glBindTexture(GL_TEXTURE_2D, current_texture);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0,
        vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0,
        tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());


}

bool const Entity::check_collision(Entity* other) const
{
    float x_distance = fabs(m_position.x - other->m_position.x) - ((m_width + other->m_width) / 2.0f);
    float y_distance = fabs(m_position.y - other->m_position.y) - ((m_height + other->m_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count) {
    for (int i = 0; i < collidable_entity_count; i++) {
        Entity* collidable_entity = &collidable_entities[i];
        if (!collidable_entity->m_is_active) continue;

        if (check_collision(collidable_entity)) {
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);
            float y_overlap = fabs(y_distance - (m_height / 2.0f) - (collidable_entity->m_height / 2.0f));

            // Ensure x-distance is small enough to consider a vertical collision
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap_threshold = (m_width + collidable_entity->m_width) / 2.0f;

            if (m_velocity.y > 0 && x_distance < 0.83) {
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
            }
            else if (m_velocity.y < 0 && x_distance < 0.83) {
                //std::cout << "pushing up by " << y_overlap << " because of " << x_distance << std::endl;
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
            }
        }
    }
}

void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count) {
    for (int i = 0; i < collidable_entity_count; i++) {
        Entity* collidable_entity = &collidable_entities[i];
        if (!collidable_entity->m_is_active) continue;

        if (check_collision(collidable_entity)) {
            float x_distance = fabs(m_position.x - collidable_entity->m_position.x);
            float x_overlap = fabs(x_distance - (m_width / 2.0f) - (collidable_entity->m_width / 2.0f));
            float y_distance = fabs(m_position.y - collidable_entity->m_position.y);

            // 确保在合理的 y 范围内处理 x 碰撞
            if (y_distance < (m_height / 2.0f + collidable_entity->m_height / 2.0f)) {
                // 如果敌人有速度，则根据敌人运动方向推动角色
                if (collidable_entity->get_velocity().x > 0) { // 敌人向右运动
                    m_position.x += x_overlap; // 推动角色向右
                    m_velocity.x = collidable_entity->get_velocity().x * 0.5f; // 角色被推的速度
                    m_collided_left = true;
                }
                else if (collidable_entity->get_velocity().x < 0) { // 敌人向左运动
                    m_position.x -= x_overlap; // 推动角色向左
                    m_velocity.x = collidable_entity->get_velocity().x * 0.5f; // 角色被推的速度
                    m_collided_right = true;
                }
                else {
                    // 如果敌人不动，则只停止角色的 x 轴速度
                    if (m_velocity.x > 0) {
                        m_position.x -= x_overlap;
                        m_velocity.x = 0;
                        m_collided_right = true;
                    }
                    else if (m_velocity.x < 0) {
                        m_position.x += x_overlap;
                        m_velocity.x = 0;
                        m_collided_left = true;
                    }
                }
            }
        }
    }
}

void const Entity::check_collision_y(Map* map)
{
    // Probes for tiles above
    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (m_width / 2), m_position.y + (m_height / 2), m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (m_width / 2), m_position.y + (m_height / 2), m_position.z);

    // Probes for tiles below
    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (m_width / 2), m_position.y - (m_height / 2), m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (m_width / 2), m_position.y - (m_height / 2), m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    // If the map is solid, check the top three points
    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;

        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0)
    {
        m_position.y -= penetration_y;
        m_velocity.y = 0;

        m_collided_top = true;
    }

    // And the bottom three points
    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;

        m_collided_bottom = true;
        //std::cout << "bot" << std::endl;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;

        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0)
    {
        m_position.y += penetration_y;
        m_velocity.y = 0;

        m_collided_bottom = true;

    }
}

void const Entity::check_collision_x(Map* map)
{
    // Probes for tiles; the x-checking is much simpler
    glm::vec3 left = glm::vec3(m_position.x - (m_width / 2), m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (m_width / 2), m_position.y, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0)
    {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0)
    {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}

void Entity::lose_life() {
    m_position = glm::vec3(1.0f, -5.0f, 0.0f);

    if (player_lives > 0) {
        player_lives--;

    }
    if (player_lives == 0) {
        m_is_active = false;
    }
}
void Entity::check_window_collision(float window_width, float window_height)
{
    float half_width = m_width / 2.0f;
    float half_height = m_height / 2.0f;

    // 左侧边界碰撞
    if (m_position.x - half_width < -window_width / 2.0f)
    {
        m_position.x = -window_width / 2.0f + half_width;
        m_velocity.x = 0.0f;
        m_collided_left = true;
    }
    // 右侧边界碰撞
    else if (m_position.x + half_width > window_width / 2.0f)
    {
        m_position.x = window_width / 2.0f - half_width;
        m_velocity.x = 0.0f;
        m_collided_right = true;
    }

    // 底部边界碰撞
    if (m_position.y - half_height < -window_height / 2.0f)
    {
        m_position.y = -window_height / 2.0f + half_height;
        m_velocity.y = 0.0f;
        m_collided_bottom = true;
        lose_life();
    }
    // 顶部边界碰撞
    else if (m_position.y + half_height > window_height / 2.0f)
    {
        m_position.y = window_height / 2.0f - half_height;
        m_velocity.y = 0.0f;

        m_collided_top = true;
    }
}



void Entity::check_collision_with_enemy(Entity* enemies, int collidable_enemy_count, float delta_time)
{
    // 更新计时器
    float x_limit = 1.0f;
    float y_limit = 1.0f;

    if (has_hit_frame_0)
        hit_window_timer_0 += delta_time;
    if (has_hit_frame_2)
        hit_window_timer_2 += delta_time;
    if (has_hit_frame_1)
        hit_window_timer_1 += delta_time;
    if (has_hit_frame_3)
        hit_window_timer_3 += delta_time;
    // 超出时间窗口后重置标记
    if (hit_window_timer_0 > hit_window_duration)
    {
        has_hit_frame_0 = false;
        hit_window_timer_0 = 0.0f;
    }
    if (hit_window_timer_2 > hit_window_duration)
    {
        has_hit_frame_2 = false;
        hit_window_timer_2 = 0.0f;
    }
    if (hit_window_timer_1 > hit_window_duration)
    {
        has_hit_frame_1 = false;
        hit_window_timer_1 = 0.0f;
    }
    if (hit_window_timer_3 > hit_window_duration)
    {
        has_hit_frame_3 = false;
        hit_window_timer_3 = 0.0f;
    }


    // 如果是火球
    if (m_entity_type == PROJECTILE) {
        if (m_position.x <= 2.0f || m_position.x >= 20.0f) {
            deactivate();
            m_fire_hit = true;
            m_ai_state = IDLE;

            return;

        }
        for (int i = 0; i < collidable_enemy_count; i++) {
            Entity* enemy = &enemies[i];
            if (!enemy->m_is_active) continue;
            float x_diff = fabs(m_position.x - enemy->get_position().x);
            float y_diff = fabs(m_position.y - enemy->get_position().y);

            // 检查火球和敌人的碰撞
            if ((enemy->m_ai_type == BANCHO)) {
                x_limit = 1.0f;
                y_limit = 2.0f;
            }
            else if ((enemy->m_ai_type == RYU)) {
                x_limit = 1.5f;
                y_limit = 2.0f;

            }
            else if ((enemy->m_ai_type == ZANGIEF)) {
                x_limit = 1.65f;//////////
                y_limit = 2.5f;

            }
            if (x_diff <= x_limit && y_diff <= y_limit) {
                //std::cout << "collision" << std::endl;
                if (enemy->get_ai_state() == DEFEND_LHS || enemy->get_ai_state() == DEFEND_RHS) {
                    // 敌人正在防御
                    enemy->set_lives(enemy->get_life() - 0.2f);
                    std::cout << "Enemy blocked fireball! Life: " << enemy->get_life() << std::endl;
                    deactivate();
                    m_fire_hit = true;
                    m_ai_state = IDLE;
                }
                else {
                    if (enemy->m_ai_type != RYU) {
                        // 敌人没有防御
                        enemy->set_lives(enemy->get_life() - 0.3f);
                        std::cout << "Enemy hit by fireball! Life: " << enemy->get_life() << std::endl;
                        enemy->set_ai_state(HURT); // 敌人进入受击状态
                        deactivate();
                        m_fire_hit = true;
                        m_ai_state = IDLE;
                        //std::cout << m_is_active << std::endl;
                    }
                    else {
                        deactivate();
                        m_fire_hit = true;
                        m_ai_state = IDLE;
                    }

                }

                // 火球失效
                //std::cout << "its here" << std::endl;
                deactivate();
                return; // 避免火球多次检测到碰撞
            }
        }
        return; // 火球没有碰撞任何敌人
    }
    else {
        for (int i = 0; i < collidable_enemy_count; i++)
        {
            Entity* enemy = &enemies[i];
            if (!enemy->m_is_active) continue;

            // 如果玩家和敌人发生碰撞
            if (check_collision(enemy))
            {
                if (enemy->m_ai_type == RYU) {
                    if (enemy->m_current_animation == PUNCH_LHS || enemy->m_current_animation == PUNCH_RHS) {
                        if (!has_hit_frame_1) {
                            has_hit_frame_1 = true;
                            hit_window_timer_1 = 0.0f;
                            if (m_current_animation == DEFEND_LHS || m_current_animation == DEFEND_RHS)
                            {
                                enemy->m_ai_state = HURT; // ZANGIEF enters ONHIT state
                                std::cout << "ZANGIEF blocked by player" << std::endl;
                            }
                            else
                            {
                                // Player takes damage
                                set_lives(get_life() - 2.0f);
                                std::cout << "Player hit by ZANGIEF! Life: " << get_life() << std::endl;
                                m_current_animation = ONHIT_LHS;
                            }
                        }
                    }
                    else if (enemy->m_current_animation == UPKICK_LHS || enemy->m_current_animation == UPKICK_RHS) {
                        int current_frame = enemy->m_animation_index;
                        if (current_frame == 2 && !has_hit_frame_1) {
                            has_hit_frame_1 = true;
                            hit_window_timer_1 = 0.0f;
                            if (m_current_animation == DEFEND_LHS || m_current_animation == DEFEND_RHS)
                            {
                                enemy->m_ai_state = HURT; // ZANGIEF enters ONHIT state
                                std::cout << "ZANGIEF blocked by player" << std::endl;
                            }
                            else
                            {
                                // Player takes damage
                                set_lives(get_life() - 2.0f);
                                std::cout << "Player hit by ZANGIEF! Life: " << get_life() << std::endl;
                                m_current_animation = ONHIT_LHS;
                            }
                        }
                    }
                    else if (enemy->m_current_animation == RAPID_LHS || enemy->m_current_animation == RAPID_RHS) {
                        int current_frame = enemy->m_animation_index;
                        if (current_frame == 2 && !has_hit_frame_1) {
                            has_hit_frame_1 = true;
                            hit_window_timer_1 = 0.0f;
                            if (m_current_animation == DEFEND_LHS || m_current_animation == DEFEND_RHS)
                            {
                                enemy->m_ai_state = HURT; // ZANGIEF enters ONHIT state
                                std::cout << "RYU blocked by player" << std::endl;
                            }
                            else
                            {
                                // Player takes damage
                                set_lives(get_life() - 2.0f);
                                std::cout << "Player hit by RYU! Life: " << get_life() << std::endl;
                                m_current_animation = ONHIT_LHS;
                            }
                        }
                    }
                }
                if (enemy->m_ai_type == ZANGIEF)
                {
                    // 检查 ZANGIEF 是否在攻击状态
                    if (enemy->m_current_animation == PUNCH_LHS || enemy->m_current_animation == PUNCH_RHS)
                    {
                        if (glm::distance(m_position, enemy->get_position()) <= 1.5f) // 碰撞距离
                        {
                            int current_frame = enemy->m_animation_index;
                            if (current_frame == 1 && !has_hit_frame_1)
                            {
                                has_hit_frame_1 = true; // Mark frame 1 as hit
                                hit_window_timer_1 = 0.0f;

                                // Check player defense
                                if (m_current_animation == DEFEND_LHS || m_current_animation == DEFEND_RHS)
                                {
                                    enemy->m_ai_state = HURT; // ZANGIEF enters ONHIT state
                                    std::cout << "ZANGIEF blocked by player" << std::endl;
                                }
                                else
                                {
                                    // Player takes damage
                                    set_lives(get_life() - 5.0f);
                                    std::cout << "Player hit by ZANGIEF! Life: " << get_life() << std::endl;
                                    m_current_animation = ONHIT_LHS;
                                }
                            }
                            else if (current_frame == 3 && !has_hit_frame_3)
                            {
                                has_hit_frame_3 = true; // Mark frame 3 as hit
                                hit_window_timer_3 = 0.0f;

                                // Check player defense
                                if (m_current_animation == DEFEND_LHS || m_current_animation == DEFEND_RHS)
                                {
                                    enemy->m_ai_state = HURT; // ZANGIEF enters ONHIT state
                                    std::cout << "ZANGIEF blocked by player" << std::endl;
                                }
                                else
                                {
                                    // Player takes damage
                                    set_lives(get_life() - 5.0f);
                                    std::cout << "Player hit by ZANGIEF! Life: " << get_life() << std::endl;
                                    m_current_animation = ONHIT_LHS;
                                }
                            }
                        }
                    }
                    else if (enemy->m_current_animation != PUNCH_LHS && enemy->m_current_animation != PUNCH_RHS)
                    {
                        enemy->m_has_attacked = false;
                    }
                }


                // 如果玩家在攻击状态 (PUNCH_LHS 或 PUNCH_RHS)
                if (m_current_animation == PUNCH_LHS || m_current_animation == PUNCH_RHS)
                {
                    int current_frame = m_animation_index;

                    // 确保只有第 0 帧和第 2 帧能打到
                    if (current_frame == 0 && !has_hit_frame_0)
                    {
                        has_hit_frame_0 = true; // 标记第 0 帧已经触发
                        hit_window_timer_0 = 0.0f; // 重置计时器
                        if ((enemy->get_ai_state() != DEFEND_LHS) && (enemy->get_ai_state() != DEFEND_RHS))
                        {
                            enemy->set_lives(enemy->get_life() - 1.0f);
                            std::cout << "Enemy Life: " << enemy->get_life() << std::endl;
                            enemy->set_ai_state(HURT);
                        }
                    }
                    else if (current_frame == 2 && !has_hit_frame_2)
                    {
                        has_hit_frame_2 = true; // 标记第 2 帧已经触发
                        hit_window_timer_2 = 0.0f; // 重置计时器
                        if ((enemy->get_ai_state() != DEFEND_LHS) && (enemy->get_ai_state() != DEFEND_RHS))
                        {
                            enemy->set_lives(enemy->get_life() - 1.0f);
                            std::cout << "Enemy Life: " << enemy->get_life() << std::endl;
                            enemy->set_ai_state(HURT);
                        }
                    }
                }
            }
        }
    }

}


void Entity::update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Map* map)
{
    if (m_lives <= 0) {
        m_is_active = false;
        //std::cout << "dead" << std::endl;
    }
    if (m_is_onhit) {
        m_onhit_timer += delta_time;
        if (m_onhit_timer >= m_onhit_duration) {
            m_is_onhit = false;         // Exit ONHIT state
            m_onhit_timer = 0.0f;       // Reset timer
            m_current_animation = STAND_RHS; // Transition to IDLE or other appropriate animation
            set_animation_state(m_current_animation);
        }
        else {
            // Stay in ONHIT state, no need to update other logic
            return;
        }
    }
    if (m_ai_type == RYU) {
        std::cout << m_current_animation << std::endl;
    }
    if (m_entity_type == PROJECTILE) {
        // 即使火球是非激活状态，也运行 ai_activate 以检查激活条件
        ai_activate(player, delta_time);
        //std::cout << m_is_active << std::endl;
        
        // 如果火球未激活，直接返回（避免运行多余逻辑）
        if (!m_is_active) {
            //std::cout << m_is_active << std::endl;
            //std::cout << "inactive" << std::endl;
            return;
        }
        //std::cout << m_is_active << std::endl;

    }
    else if (!m_is_active) {
        return; // 非 PROJECTILE 类型直接退出
    }
    if (m_entity_type == PROJECTILE) {
        //std::cout << "its here" << std::endl;
    }
    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;

    if (m_entity_type == ENEMY) {
        ai_activate(player, delta_time);/////
        //std::cout << "is" << m_ai_type << std::endl; yes
    }
    if (m_animation_indices != NULL)
    {
        m_animation_time += delta_time;
        float frames_per_second = (float)1 / SECONDS_PER_FRAME;
        //std::cout << m_ai_type << std::endl;
        if (m_animation_time >= frames_per_second)
        {
            m_animation_time = 0.0f;
            m_animation_index++;

            if (m_animation_index >= m_animation_frames)
            {
                m_animation_index = 0;
            }
        }
    }
    check_collision_with_enemy(collidable_entities, collidable_entity_count, delta_time);

    if (std::abs(m_velocity.y) > 1000) {
        //std::cerr << "Abnormal velocity detected: " << m_velocity.y << std::endl;
        m_velocity.y = 0.0f;
    }
    m_velocity.x = m_movement.x * m_speed;
    m_velocity.y += m_acceleration.y * delta_time;

    m_position.y += m_velocity.y * delta_time;

    check_collision_y(collidable_entities, collidable_entity_count);
    check_collision_y(map);


    m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count);
    check_collision_x(map);

    check_window_collision(80.0f, 18.0f);
    if (m_is_jumping)
    {
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
        //std::cout << m_velocity.y << std::endl;
    }
    if (m_is_punching) {
        // 检查动画是否播放完成
        if (m_animation_index >= 2) {
            // 停止 Punch 动作并恢复到默认状态
            m_is_punching = false;
        }
    }

    //std::cout << m_velocity.y << std::endl;
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    if (m_entity_type == PLAYER) {
        if (m_current_animation == WALK_RIGHT ||
            m_current_animation == BACK_LEFT ||
            m_current_animation == WALK_LEFT ||
            m_current_animation == BACK_RIGHT) {
            m_model_matrix = glm::scale(m_model_matrix, glm::vec3(1.2f, 2.0f, 0.0f));
        }
        else if (m_current_animation == JUMP_LHS ||
            m_current_animation == JUMP_RHS) {
            m_model_matrix = glm::scale(m_model_matrix, glm::vec3(1.8f, 1.5f, 0.0f));
        }
        else if (m_current_animation == DEFEND_LHS ||
            m_current_animation == DEFEND_RHS) {
            m_model_matrix = glm::scale(m_model_matrix, glm::vec3(1.2f, 2.0f, 0.0f));
        }
        else {
            m_model_matrix = glm::scale(m_model_matrix, glm::vec3(1.5f, 2.0f, 0.0f));
        }
    }
    else if (m_ai_type == BANCHO){
        if (m_current_animation == ONHIT_LHS || m_current_animation == ONHIT_RHS) {
            m_model_matrix = glm::scale(m_model_matrix, glm::vec3(1.8f, 2.2f, 0.0f));

        }
        else {
            m_model_matrix = glm::scale(m_model_matrix, glm::vec3(1.6f, 2.0f, 0.0f));
        }
        
        //std::cout << m_collided_bottom << "  " << m_collided_top <<std::endl;

    }
    if (m_ai_type == ZANGIEF) {
        if (m_current_animation == PUNCH_LHS || m_current_animation == PUNCH_RHS) {
            m_model_matrix = glm::scale(m_model_matrix, glm::vec3(2.7f, 2.5f, 0.0f));
        }
        else {
            m_model_matrix = glm::scale(m_model_matrix, glm::vec3(2.3f, 2.4f, 0.0f));

        }
    }
    if (m_ai_type == RYU) {
        if (m_current_animation == PUNCH_LHS || m_current_animation == PUNCH_RHS ||
            m_current_animation == UPKICK_LHS || m_current_animation == UPKICK_RHS) {
            m_model_matrix = glm::scale(m_model_matrix, glm::vec3(2.0f, 2.4f, 0.0f));
        }
        else {
            m_model_matrix = glm::scale(m_model_matrix, glm::vec3(2.0f, 2.4f, 0.0f));

        }
    }

}




void Entity::render(ShaderProgram* program)
{
    if (!m_is_active) {
        //std::cout << "inactive" << std::endl;

        return;
    }
    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != NULL)
    {
        draw_sprite_from_texture_atlas(program);
        //std::cout << m_entity_type << std::endl;

        return;
    }

    //std::cout << "here" << std::endl;

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
    //std::cout << "Rendered entity at position: " << m_position.x << ", " << m_position.y << std::endl;

}

