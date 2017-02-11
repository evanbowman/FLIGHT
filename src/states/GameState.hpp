#pragma once

class Game;

struct GameState {
    friend class Game;
    virtual void Update(const long long dt, Game &) = 0;
    virtual void Display(Game &) = 0;
};

struct Scene : public GameState {
    Scene(Game & game) {
	
    }
    void Update(const long long dt, Game & game) override {
	
    }
    void Display(Game & game) override {
	
    }
};


