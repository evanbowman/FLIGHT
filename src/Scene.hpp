#pragma once

#include "ThreadGuard.hpp"

#include <iostream>

class Scene {
public:
    virtual void Update(const long long dt) = 0;
    virtual void Display() = 0;
    virtual ~Scene() {}
};

class TitleScreen : public Scene {
public:
    void Update(const long long dt) override;
    void Display() override;
};

class WorldLoader : public Scene {
    bool m_active;
    ThreadGuard m_terrainThread;
public:
    WorldLoader();
    void Update(const long long dt) override;
    void Display() override;
    ~WorldLoader() {
	m_active = false;
    }
};

class World : public Scene {
    std::mutex m_updateMtx;
    void DrawTerrain();
    void DrawSky();
    void UpdateProjectionUniforms();
public:
    World();
    void Update(const long long dt) override;
    void Display() override;
};

class Menu : public Scene {
public:
    void Update(const long long dt) override;
    void Display() override;
};
