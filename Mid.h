#pragma once
#include "Scene.h"

class Mid : public Scene {
public:
    // ！！！！！ STATIC ATTRIBUTES ！！！！！ //
    int ENEMY_COUNT = 0;

    // ！！！！！ DESTRUCTOR ！！！！！ //
    ~Mid();

    // ！！！！！ METHODS ！！！！！ //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};
