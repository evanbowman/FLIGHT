#pragma once

#include "ThreadGuard.hpp"

#include <iostream>

class Scene {
public:
    virtual void Update(const long long dt) = 0;
    virtual void Display() = 0;
    virtual void TransitionIn(const float) = 0;
    virtual void TransitionOut(const float) = 0;
    virtual ~Scene() {}
};

// class Transition : public Scene { ... }

class TitleScreen : public Scene {
public:
    void Update(const long long dt) override;
    void Display() override;
    void TransitionIn(const float) override;
    void TransitionOut(const float) override;
};

class WorldLoader : public Scene {
    bool m_active;
    ThreadGuard m_terrainThread;
public:
    WorldLoader();
    void Update(const long long dt) override;
    void Display() override;
    void TransitionIn(const float) override;
    void TransitionOut(const float) override;
    ~WorldLoader() {
	m_active = false;
    }
};

class World : public Scene {
    std::mutex m_updateMtx;
    void DrawTerrain();
    void UpdateProjectionUniforms();
public:
    World();
    void Update(const long long dt) override;
    void Display() override;
    void TransitionIn(const float) override;
    void TransitionOut(const float) override; 
};
