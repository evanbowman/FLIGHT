#pragma once

#include "GameState.hpp"

#include "../RedTail.hpp"

struct LoadScene : public GameState {
    LoadScene(Game & game);
    void Update(const long long dt, Game & game) override;
    void Display(Game & game) override;
};
