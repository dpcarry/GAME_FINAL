/**
* Author: Pingchuan Dong
* Assignment: Path to Darkness
* Date due: Dec. 11th, 2:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "Start.h"
#include "Mid.h"
#include "GlobalState.h"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH = 1280,
WINDOW_HEIGHT = 960;

constexpr float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;



constexpr char V_SHADER_PATH[] = "shaders/vertex_lit.glsl",
F_SHADER_PATH[] = "shaders/fragment_lit.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr glm::vec3 HORNET_ATTACK_SCALE = glm::vec3(1.5f, 0.8f, 0.0f);
constexpr glm::vec3 HORNET_SUPER_SCALE = glm::vec3(0.8f, 1.0f, 0.0f);

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
Scene* g_current_scene;
Start* g_start;
LevelA* g_levelA;
LevelB* g_levelB;
Mid* g_mid;
LevelC* g_levelC;


Scene* g_levels[5];

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
ShaderProgram g_shader_lit_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;
int player_lives = 3;  // 初始生命值
int won = 1;

void switch_to_scene(Scene* scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();


void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Road to Darkness",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    g_shader_lit_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    // Pass the current level as an integer (e.g., levelC == 3)
    //glUniform1i(currentLevelLocation, g_current_scene->get_state().level_number);
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_start = new Start();
    g_levelA = new LevelA();
    g_levelB = new LevelB();
    g_mid = new Mid();
    g_levelC = new LevelC();

    g_levels[0] = g_start;
    g_levels[1] = g_levelA;
    g_levels[2] = g_levelB;
    g_levels[3] = g_mid;
    g_levels[4] = g_levelC;

    // ————— LEVEL A SETUP ————— //
    //g_level_a = new LevelA();
    switch_to_scene(g_levels[0]);
    //switch_to_scene(g_level_a);

    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    if (g_current_scene->get_state().player) {
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // ————— KEYSTROKES ————— //
            switch (event.type) {
                // ————— END GAME ————— //
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_q:
                    // Quit the game with a keystroke
                    g_app_status = TERMINATED;
                    break;

                case SDLK_SPACE:
                    // ————— JUMPING ————— //
                    if (g_current_scene->get_state().player->get_collided_bottom())
                    {
                        g_current_scene->get_state().player->jump();
                        Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                    }
                    break;


                default:
                    break;
                }

            default:
                break;
            }
        }


        // ————— KEY HOLD ————— //
        const Uint8* key_state = SDL_GetKeyboardState(NULL);
        if (g_current_scene->get_state().player->get_position().x > g_current_scene->get_state().enemies->get_position().x) { ///enemies是否是数组
            g_current_scene->get_state().player->set_animation_state(STAND_RHS);
            //std::cout << "current animation" << g_current_scene->get_state().player->get_animation() << std::endl;
            g_current_scene->get_state().player->set_scale(glm::vec3(1.0f, 1.0f, 0));
            g_current_scene->get_state().player->set_width(0.7f);
            g_current_scene->get_state().player->set_speed(3.0f);
            g_current_scene->get_state().player->set_acceleration(glm::vec3(0.0f, -14.00f, 0.0f));
            if (key_state[SDL_SCANCODE_A]) {
                g_current_scene->get_state().player->move_left_rhs();
                if (g_current_scene->get_state().player->get_position().y >= -5.2) {
                    g_current_scene->get_state().player->set_animation_state(JUMP_RHS);
                }
                else if (key_state[SDL_SCANCODE_O]) {
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().player->set_animation_state(DEFEND_RHS);
                }
                else if (key_state[SDL_SCANCODE_J]) {
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().player->set_animation_state(PUNCH_RHS);
                }
                else if (key_state[SDL_SCANCODE_L]) {
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().player->set_animation_state(SHOOT_RHS);
                }
                else {
                    g_current_scene->get_state().player->set_animation_state(WALK_LEFT);
                    //std::cout << g_current_scene->get_state().player->get_animation() << ", " << g_current_scene->get_state().player->get_indices()[g_current_scene->get_state().player->get_index()] << std::endl;
                }
                /*if (key_state[SDL_SCANCODE_A]) {
                    g_current_scene->get_state().player->set_animation_state(ATTACK_LEFT);
                    g_current_scene->get_state().player->set_scale(HORNET_ATTACK_SCALE);
                    g_current_scene->get_state().player->set_width(1.2f);
                    g_current_scene->get_state().player->set_speed(5.0f);

                }
                else if (key_state[SDL_SCANCODE_S]) {
                    g_current_scene->get_state().player->set_animation_state(DEATH);
                    g_current_scene->get_state().player->set_scale(HORNET_SUPER_SCALE);
                    g_current_scene->get_state().player->set_width(0.7f);
                    g_current_scene->get_state().player->set_acceleration(glm::vec3(0.0f, -18.0f, 0.0f));
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, -1.0f, 0.0f));
                }*/
            }

            else if (key_state[SDL_SCANCODE_D]) {
                g_current_scene->get_state().player->move_right_rhs();
                if (g_current_scene->get_state().player->get_position().y >= -5.2) {
                    g_current_scene->get_state().player->set_animation_state(JUMP_LHS);
                }
                else if (key_state[SDL_SCANCODE_O]) {
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().player->set_animation_state(DEFEND_RHS);
                }
                else if (key_state[SDL_SCANCODE_J]) {
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().player->set_animation_state(PUNCH_RHS);
                }
                else if (key_state[SDL_SCANCODE_L]) {
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().player->set_animation_state(SHOOT_RHS);
                }
                else {
                    g_current_scene->get_state().player->set_animation_state(BACK_RIGHT);
                    //std::cout << g_current_scene->get_state().player->get_animation() << ", " << g_current_scene->get_state().player->get_index() << std::endl;
                }

                /*if (key_state[SDL_SCANCODE_A]) {
                    g_current_scene->get_state().player->set_animation_state(ATTACK_RIGHT);
                    g_current_scene->get_state().player->set_scale(HORNET_ATTACK_SCALE);
                    g_current_scene->get_state().player->set_width(1.2f);
                    g_current_scene->get_state().player->set_speed(5.0f);
                }
                else if (key_state[SDL_SCANCODE_S]) {
                    g_current_scene->get_state().player->set_animation_state(DEATH);
                    g_current_scene->get_state().player->set_scale(HORNET_SUPER_SCALE);
                    g_current_scene->get_state().player->set_width(0.7f);
                    g_current_scene->get_state().player->set_acceleration(glm::vec3(0.0f, -18.0f, 0.0f));
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, -1.0f, 0.0f));
                }*/
            }
            else if (key_state[SDL_SCANCODE_J]) {
                g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                g_current_scene->get_state().player->set_animation_state(PUNCH_RHS);
            }
            else if (key_state[SDL_SCANCODE_O]) {
                g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                g_current_scene->get_state().player->set_animation_state(DEFEND_RHS);
            }
            else if (key_state[SDL_SCANCODE_L]) {
                g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                g_current_scene->get_state().player->set_animation_state(SHOOT_RHS);
            }
            else {
                // 没有按键时恢复为 STAND_RHS
                g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
                g_current_scene->get_state().player->set_animation_state(STAND_RHS);
            }
        }
        else { //player在enemy左侧
            g_current_scene->get_state().player->set_animation_state(STAND_LHS);
            g_current_scene->get_state().player->set_scale(glm::vec3(1.0f, 1.0f, 0));
            g_current_scene->get_state().player->set_width(0.7f);
            g_current_scene->get_state().player->set_speed(3.0f);
            g_current_scene->get_state().player->set_acceleration(glm::vec3(0.0f, -14.00f, 0.0f));

            if (key_state[SDL_SCANCODE_A]) {
                g_current_scene->get_state().player->move_left_lhs();
                if (g_current_scene->get_state().player->get_position().y >= -5.2) {
                    g_current_scene->get_state().player->set_animation_state(JUMP_RHS);
                }
                else if (key_state[SDL_SCANCODE_O]) {
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().player->set_animation_state(DEFEND_LHS);
                }
                else if (key_state[SDL_SCANCODE_J]) {
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().player->set_animation_state(PUNCH_LHS);
                }
                else if (key_state[SDL_SCANCODE_L]) {
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().player->set_animation_state(SHOOT_LHS);
                }

                else {
                    g_current_scene->get_state().player->set_animation_state(BACK_LEFT);
                    //std::cout << g_current_scene->get_state().player->get_animation() << ", " << g_current_scene->get_state().player->get_index() << std::endl;

                    /*if (key_state[SDL_SCANCODE_A]) {
                        g_current_scene->get_state().player->set_animation_state(ATTACK_LEFT);
                        g_current_scene->get_state().player->set_scale(HORNET_ATTACK_SCALE);
                        g_current_scene->get_state().player->set_width(1.2f);
                        g_current_scene->get_state().player->set_speed(5.0f);

                    }
                    else if (key_state[SDL_SCANCODE_S]) {
                        g_current_scene->get_state().player->set_animation_state(DEATH);
                        g_current_scene->get_state().player->set_scale(HORNET_SUPER_SCALE);
                        g_current_scene->get_state().player->set_width(0.7f);
                        g_current_scene->get_state().player->set_acceleration(glm::vec3(0.0f, -18.0f, 0.0f));
                        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, -1.0f, 0.0f));
                    }*/
                }

            }

            else if (key_state[SDL_SCANCODE_D]) {
                g_current_scene->get_state().player->move_right_rhs();
                if (g_current_scene->get_state().player->get_position().y >= -5.2) {
                    g_current_scene->get_state().player->set_animation_state(JUMP_LHS);
                }
                else if (key_state[SDL_SCANCODE_O]) {
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().player->set_animation_state(DEFEND_LHS);
                }
                else if (key_state[SDL_SCANCODE_J]) {
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().player->set_animation_state(PUNCH_LHS);
                }
                else if (key_state[SDL_SCANCODE_L]) {
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                    g_current_scene->get_state().player->set_animation_state(SHOOT_LHS);
                }
                else {
                    g_current_scene->get_state().player->set_animation_state(WALK_RIGHT);
                    //std::cout << g_current_scene->get_state().player->get_animation() << ", "<< g_current_scene->get_state().player->get_index()<<std::endl;

                }
                /*if (key_state[SDL_SCANCODE_A]) {
                    g_current_scene->get_state().player->set_animation_state(ATTACK_RIGHT);
                    g_current_scene->get_state().player->set_scale(HORNET_ATTACK_SCALE);
                    g_current_scene->get_state().player->set_width(1.2f);
                    g_current_scene->get_state().player->set_speed(5.0f);
                }
                else if (key_state[SDL_SCANCODE_S]) {
                    g_current_scene->get_state().player->set_animation_state(DEATH);
                    g_current_scene->get_state().player->set_scale(HORNET_SUPER_SCALE);
                    g_current_scene->get_state().player->set_width(0.7f);
                    g_current_scene->get_state().player->set_acceleration(glm::vec3(0.0f, -18.0f, 0.0f));
                    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, -1.0f, 0.0f));
                }
            }
*/
            }
            else if (key_state[SDL_SCANCODE_O]) {
                g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                g_current_scene->get_state().player->set_animation_state(DEFEND_LHS);
            }
            else if (key_state[SDL_SCANCODE_J]) {
                g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                g_current_scene->get_state().player->set_animation_state(PUNCH_LHS);
            }
            else if (key_state[SDL_SCANCODE_L]) {
                g_current_scene->get_state().player->set_movement(glm::vec3(0.0f, 0.0f, 0.0f));
                g_current_scene->get_state().player->set_animation_state(SHOOT_LHS);
            }
            else {
                // 没有按键时恢复为 STAND_LHS
                g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
                g_current_scene->get_state().player->set_animation_state(STAND_LHS);
            }
        }


        if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
            g_current_scene->get_state().player->normalise_movement();
    }
    /*if (g_current_scene == g_start) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // ————— KEYSTROKES ————— //
            switch (event.type) {
                // ————— END GAME ————— //
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_RETURN:
                    // Quit the game with a keystroke
                    switch_to_scene(g_levelA);
                    break;
                }
            }
        }
    }*/

}

void update()
{
    g_shader_program.set_is_c(g_current_scene == g_levelC);

    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);

        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;


    // ————— PLAYER CAMERA ————— //
    if (g_current_scene->get_state().player) {
        g_view_matrix = glm::mat4(1.0f);

        if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, 3.75, 0));
        }
        else {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
        }
    }

}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    if (g_current_scene->get_state().level_number == 1 || g_current_scene->get_state().level_number == 2 || g_current_scene->get_state().level_number == 3 ) {
        g_shader_program.set_light_position_matrix(g_current_scene->get_state().player->get_position());

    }

    glClear(GL_COLOR_BUFFER_BIT);

    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);

    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    // ————— DELETING LEVEL A DATA (i.e. map, character, enemies...) ————— //
    delete g_levelA;
    delete g_start;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        if (g_current_scene->get_state().next_scene_id >= 0) switch_to_scene(g_levels[g_current_scene->get_state().next_scene_id]);
        render();
    }

    shutdown();
    return 0;
}