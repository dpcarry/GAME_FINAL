#include "LevelC.h"
#include "Utility.h"
#include "Scene.h"

#include <SDL_mixer.h>

#include <iomanip> // for std::setprecision
#include <sstream> // for std::ostringstream

#define LEVEL_WIDTH 25
#define LEVEL_HEIGHT 8
#define PROMPT_TEXT "You Win"
#define PROMPT_TEXT2 "You Lose"


constexpr char SPRITESHEET_FILEPATH[] = "assets/george_0.png",
PLATFORM_FILEPATH[] = "assets/platformPack_tile027.png",
ENEMY_FILEPATH[] = "assets/soph.png";


bool win = false;

unsigned int LEVEL_DATA3[] =
{
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2
};

LevelC::~LevelC()
{
    delete[] m_game_state.enemies;
    delete    m_game_state.player;
    delete    m_game_state.map;
    delete[] m_game_state.projectiles;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelC::initialise()
{
    m_game_state.next_scene_id = -1;
    m_game_state.level_number = 2;

    //PLAYER//
    std::vector<GLuint> player_texture_ids = {
    Utility::load_texture("assets/player1_idle_lhs.png"),   // IDLE lhs spritesheet
    Utility::load_texture("assets/player1_idle_rhs.png"),   // IDLE rhs spritesheet
    Utility::load_texture("assets/player1_double_punch_lhs.png"),   // double punch lhs spritesheet
    Utility::load_texture("assets/player1_double_punch_rhs.png"),   // double punch rhs spritesheet
    Utility::load_texture("assets/player1_rapkick_lhs.png"),   // rapid kick lhs spritesheet
    Utility::load_texture("assets/player1_rapkick_rhs.png"),   // rapid kick rhs spritesheet
    Utility::load_texture("assets/player1_shoot_lhs.png"),   // shoot lhs spritesheet
    Utility::load_texture("assets/player1_shoot_rhs.png"),   // shoot rhs spritesheet
    Utility::load_texture("assets/player1_upkick_lhs.png"),   // upkick lhs spritesheet
    Utility::load_texture("assets/player1_upkick_rhs.png"),   // upkick rhs spritesheet
    Utility::load_texture("assets/player1_walkleft.png"),  // WALK LEFT spritesheet
    Utility::load_texture("assets/player1_walkright.png"),  // WALK RIGHT spritesheet
    Utility::load_texture("assets/player1_backleft.png"),  // Backup LEFT spritesheet
    Utility::load_texture("assets/player1_backright.png"),  // Backup RIGHT spritesheet
    Utility::load_texture("assets/player1_defend_lhs.png"),  // defend lhs spritesheet
    Utility::load_texture("assets/player1_defend_rhs.png"),  // defend rhs spritesheet
    Utility::load_texture("assets/player1_onhit_lhs.png"),   // onhit lhs spritesheet
    Utility::load_texture("assets/player1_onhit_rhs.png"),   // onhit rhs spritesheet
    Utility::load_texture("assets/player1_jump_lhs.png"),   // jump lhs spritesheet
    Utility::load_texture("assets/player1_jump_rhs.png"),   // jump rhs spritesheet

    };
    // ******************* need to add on Entity.h, and player_animations, and the logic that turns player around
    std::vector<std::vector<int>> player_animations = {
        {0, 1, 2},       // IDLE lhs animation frames
        {2, 1, 0},        // IDLE rhs animation frames
        {0, 1, 2},         //double punch lfs animation frames
        {2, 1, 0},        // double punch rfs animation frames
        {0, 1},  // Rapid Kick lhs animation frames
        {1, 0},  // Rapid Kick rhs animation frames
        {0, 1, 2},  //shoot lhs
        {2, 1, 0},  //shoot rhs
        {0, 1, 2, 3},    //upkick lhs
        {3, 2, 1, 0},  //upkick rhs
        {5, 4, 3, 2, 1, 0},   //walk left
        {0, 1, 2, 3, 4, 5},   //walk right
        {5, 4, 3, 2, 1, 0},   //backup left
        {0, 1, 2, 3, 4, 5},   //backup right
        { 0 },  // DEFEND LHS animation frames
        { 0 },  // DEFEND RHS animation frames
        {0, 1},   //ONHIT LHS
        {0, 1},   //ONHIT RHS
        {0},    // JUMP LHS
        {0}    // JUMP RHS
    };

    int player_walking_animation[4][6];
    for (int j = 0; j < 6; ++j) {
        player_walking_animation[0][j] = player_animations[10][j]; // WALK LEFT
        player_walking_animation[1][j] = player_animations[11][j]; // WALK RIGHT
        player_walking_animation[2][j] = player_animations[12][j]; // BACKUP LEFT
        player_walking_animation[3][j] = player_animations[13][j]; // BACKUP RIGHT ///回来看一下转向的逻辑
    }

    GLuint map_texture_id = Utility::load_texture("assets/tiles.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA3, map_texture_id, 1.0f, 4, 1);

    glm::vec3 acceleration = glm::vec3(0.0f, -12.00f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_ids,         // texture id
        3.0f,                      // speed
        acceleration,              // acceleration
        7.0f,                      // jumping power
        player_walking_animation,  // animation index sets
        player_animations,         // hornet movements
        0.0f,                      // animation time
        6,                         // animation frame amount
        0,                         // current animation index
        6,                         // animation column amount
        1,                         // animation row amount
        1.5f,                      // width
        2.0f,                       // height
        PLAYER
    );

    m_game_state.player->set_position(glm::vec3(6.0f, -5.0f, 0.0f));

    // Jumping
    m_game_state.player->set_jumping_power(10.0f);


    /**
Projectile stuff */
    std::vector<GLuint> fireball_texture_ids = {
    Utility::load_texture("assets/fireball_lhs.png"),   // IDLE lhs spritesheet
    Utility::load_texture("assets/fireball_rhs.png"),   // IDLE rhs spritesheet
    Utility::load_texture("assets/fireball_lhs.png"),   // IDLE lhs spritesheet
    Utility::load_texture("assets/fireball_rhs.png"),   // IDLE rhs spritesheet
    Utility::load_texture("assets/fireball_lhs.png"),   // IDLE lhs spritesheet
    Utility::load_texture("assets/fireball_rhs.png"),   // IDLE rhs spritesheet
    Utility::load_texture("assets/fireball_lhs.png"),   // IDLE lhs spritesheet
    Utility::load_texture("assets/fireball_rhs.png"),   // IDLE rhs spritesheet
    Utility::load_texture("assets/fireball_lhs.png"),   // IDLE lhs spritesheet
    Utility::load_texture("assets/fireball_rhs.png"),   // IDLE rhs spritesheet
    Utility::load_texture("assets/fireball_lhs.png"),   // IDLE lhs spritesheet
    Utility::load_texture("assets/fireball_rhs.png"),   // IDLE rhs spritesheet
    Utility::load_texture("assets/fireball_lhs.png"),   // IDLE lhs spritesheet
    Utility::load_texture("assets/fireball_rhs.png"),   // IDLE rhs spritesheet
    Utility::load_texture("assets/fireball_lhs.png"),   // IDLE lhs spritesheet
    Utility::load_texture("assets/fireball_rhs.png"),   // IDLE rhs spritesheet
    Utility::load_texture("assets/fireball_lhs.png"),   // IDLE lhs spritesheet
    Utility::load_texture("assets/fireball_rhs.png"),   // IDLE rhs spritesheet
    Utility::load_texture("assets/fireball_lhs.png"),   // IDLE lhs spritesheet
    Utility::load_texture("assets/fireball_rhs.png"),   // IDLE rhs spritesheet
    };

    std::vector<std::vector<int>> fireball_animations = {
    {0, 1, 2, 3},       // IDLE lhs animation frames
    {3, 2, 1, 0},        // IDLE rhs animation frames
    {6, 5, 4, 3, 2, 1, 0},
    {6, 5, 4, 3, 2, 1, 0},
    {0, 1},
    {1, 0},
    {0, 1, 2},
    {2, 1, 0},
    {0, 1, 2, 3},    //upkick lhs
    {3, 2, 1, 0},  //upkick rhs
    {5, 4, 3, 2, 1, 0},   //walk left
    {0, 1, 2, 3, 4, 5},   //walk right
    {5, 4, 3, 2, 1, 0},
    {0, 1, 2, 3, 4, 5},
    { 0 },
    { 0 },
    {0, 1, 2},   //ONHIT LHS
    {0, 1, 2},   //ONHIT RHS
    {0, 1},
    {0, 1}
    };
    m_game_state.projectiles = new Entity[1];
    m_game_state.projectiles[0] = Entity(fireball_texture_ids, 4.0f, 1.0f, 2.0f, fireball_animations, PROJECTILE, FIREBALL, IDLE);
    m_game_state.projectiles[0].set_movement(glm::vec3(0.0f, 0.0F, 0.0f));
    m_game_state.projectiles[0].set_acceleration(glm::vec3(0.0f, -0.0f, 0.0f));
    m_game_state.projectiles[0].deactivate();


    /**
     Enemies' stuff */

    std::vector<GLuint> ryu_texture_ids = {
    Utility::load_texture("assets/ryu_idle_lhs.png"),   // IDLE lhs spritesheet
    Utility::load_texture("assets/ryu_idle_rhs.png"),   // IDLE rhs spritesheet
    Utility::load_texture("assets/ryu_punch_lhs.png"),   // double punch lhs spritesheet
    Utility::load_texture("assets/ryu_punch_rhs.png"),   // double punch rhs spritesheet
    Utility::load_texture("assets/ryu_spin.png"),   // rapid kick lhs spritesheet
    Utility::load_texture("assets/ryu_spin.png"),   // rapid kick rhs spritesheet
    Utility::load_texture("assets/ryu_shoot_lhs.png"),   // shoot lhs spritesheet
    Utility::load_texture("assets/ryu_shoot_rhs.png"),   // shoot rhs spritesheet
    Utility::load_texture("assets/ryu_high_lhs.png"),   // upkick lhs spritesheet
    Utility::load_texture("assets/ryu_high_rhs.png"),   // upkick rhs spritesheet
    Utility::load_texture("assets/ryu_walkleft.png"),  // WALK LEFT spritesheet
    Utility::load_texture("assets/ryu_walkright.png"),  // WALK RIGHT spritesheet
    Utility::load_texture("assets/bancho_walk_right.png"),  // Backup LEFT spritesheet
    Utility::load_texture("assets/bancho_walk_left.png"),  // Backup RIGHT spritesheet
    Utility::load_texture("assets/ryu_defend_lhs.png"),  // defend lhs spritesheet
    Utility::load_texture("assets/ryu_defend_rhs.png"),  // defend rhs spritesheet
    Utility::load_texture("assets/ryu_onhit_lhs.png"),   // onhit lhs spritesheet
    Utility::load_texture("assets/ryu_onhit_rhs.png"),   // onhit rhs spritesheet
    Utility::load_texture("assets/ryu_jump_rhs.png"),  // jump lhs spritesheet
    Utility::load_texture("assets/ryu_jump_rhs.png"),  // jump rhs spritesheet
    };

    std::vector<std::vector<int>> ryu_animations = {
        {0, 1, 2, 3, 4, 5},       // IDLE lhs animation frames
        {0, 1, 2, 3, 4, 5},        // IDLE rhs animation frames
        {0, 1},
        {0, 1},
        {0, 1, 2},
        {0, 1, 2},
        {0, 1},
        {1, 2},
        {0, 1, 2},    //upkick lhs
        {0, 1, 2},  //upkick rhs
        {0, 1, 2},   //walk left
        {0, 1, 2},   //walk right
        {5, 4, 3, 2, 1, 0},
        {0, 1, 2, 3, 4, 5},
        { 0 },
        { 0 },
        {0, 1},   //ONHIT LHS
        {0, 1},   //ONHIT RHS
        {0, 1, 2, 3, 4, 5},
        {0, 1, 2, 3, 4, 5}
    };

    m_game_state.enemies = new Entity[ENEMY_COUNT];

    m_game_state.enemies[0] = Entity(ryu_texture_ids, 1.3f, 1.5f, 2.4f, ryu_animations, ENEMY, RYU, IDLE);
    m_game_state.enemies[0].set_position(glm::vec3(10.0f, -5.0f, 0.0f)); //为什么敌人位置不对
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));
    m_game_state.enemies[0].set_lives(10);

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

    m_game_state.bgm = Mix_LoadMUS("assets/cut.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(60.0f);

    m_game_state.jump_sfx = Mix_LoadWAV("assets/jump.wav");
    m_game_state.win_sfx = Mix_LoadWAV("assets/win.wav");
    m_game_state.punch_sfx = Mix_LoadWAV("assets/punch_2.wav");
    //std::cout << m_game_state.enemies[0].get_position().x << ", " << m_game_state.enemies[0].get_position().y<<std::endl;
}

void LevelC::update(float delta_time)
{
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    m_game_state.projectiles[0].update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);

    for (int i = 0; i < ENEMY_COUNT; i++)
    {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
        //std::cout << m_game_state.enemies[0].get_position().x << ", " << m_game_state.enemies[0].get_position().y << "-----velocity: " << m_game_state.enemies[0].get_velocity().x << ", " << m_game_state.enemies[0].get_velocity().y << std::endl;

    }
    if ((m_game_state.enemies[0].get_is_active() == false)) {
        Mix_PlayChannel(-1, m_game_state.win_sfx, 0);
        win = true;
    }
}


void LevelC::render(ShaderProgram* g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    m_game_state.player->render(g_shader_program);
    m_game_state.projectiles[0].render(g_shader_program);
    for (int i = 0; i < m_number_of_enemies; i++) {
        m_game_state.enemies[i].render(g_shader_program);

        //std::cout << i << std::endl;
    }
    GLuint font_texture_id = Utility::load_texture("assets/font1.png");

    glm::vec3 player_position = m_game_state.player->get_position();
    glm::vec3 enemy_position = m_game_state.enemies[0].get_position();
    glm::vec3 text_life1 = player_position + glm::vec3(-0.5f, 2.0f, 0.0f);
    glm::vec3 text_life2 = enemy_position + glm::vec3(-0.5f, 2.0f, 0.0f);
    glm::vec3 text_msg = player_position + glm::vec3(1.0f, 3.0f, 0.0f);
    std::ostringstream player_life_stream;
    player_life_stream << std::fixed << std::setprecision(1) << m_game_state.player->get_life();
    std::string player_life_text = player_life_stream.str();
    std::ostringstream enemy_life_stream;
    enemy_life_stream << std::fixed << std::setprecision(1) << m_game_state.enemies[0].get_life();
    std::string enemy_life_text = enemy_life_stream.str();
    if (win == true) {
        Utility::draw_text(g_shader_program, font_texture_id, PROMPT_TEXT, 0.6f, -0.02f, text_msg);
        m_game_state.enemies[0].deactivate();
        //std::cout << "won" << std::endl;
    }
    glm::vec3 center_position();
    if (m_game_state.player->get_is_active() == false) {
        Utility::draw_text(g_shader_program, font_texture_id, PROMPT_TEXT2, 0.6f, -0.02f, text_msg);
    }
    Utility::draw_text(g_shader_program, font_texture_id, player_life_text, 0.6f, -0.02f, text_life1);
    Utility::draw_text(g_shader_program, font_texture_id, enemy_life_text, 0.6f, -0.02f, text_life2);
}
