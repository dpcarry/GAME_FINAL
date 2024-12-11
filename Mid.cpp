#include "Mid.h"
#include "Utility.h"

#define PROMPT_TEXT1 "AT MIDNIGHT..."



Mid::~Mid() {
    // �ͷ���Դ����������
    Mix_FreeMusic(m_game_state.bgm);
}

void Mid::initialise() {
    m_game_state.next_scene_id = -1;
    m_game_state.level_number = 5;

    // ��ʼ���������ֻ�����Ԫ��
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_game_state.bgm = Mix_LoadMUS("assets/start_screen_music.mp3");
    if (m_game_state.bgm != nullptr) {
        Mix_PlayMusic(m_game_state.bgm, -1);
        Mix_VolumeMusic(32); // ������������
    }

    // ������Լ�����������Ҫ����Դ�����米��ͼƬ�������
}

void Mid::update(float delta_time) {
    static bool keyHandled = false;  // Static variable to handle key press once

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            exit(0);  // Exit game
        }
        else if (event.type == SDL_KEYDOWN) {
            if (!keyHandled) {  // Check if the key press has been handled
                switch (event.key.keysym.sym) {
                case SDLK_RETURN:
                    m_game_state.next_scene_id = 4;  // Set to switch to LevelA
                    keyHandled = true;  // Mark key as handled
                    break;
                default:
                    break;
                }
            }
        }

    }
}

void Mid::render(ShaderProgram* program) {
    // �����Ļ���ݣ����ñ���ɫ
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // ���ñ���ɫ
    glClear(GL_COLOR_BUFFER_BIT);          // �����ɫ����

    // ʹ����ɫ������
    glUseProgram(program->get_program_id());

    // ������������
    GLuint font_texture_id = Utility::load_texture("assets/font1.png");

    // ��Ⱦ�������ʾ��Ϣ
    Utility::draw_text(program, font_texture_id, PROMPT_TEXT1, 0.4f, -0.01f, glm::vec3(4.5f, -5.0f, 0.0f));


}