#pragma once

#include "ThreadGuard.hpp"
#include <iostream>
#include <memory>
#include <stack>

class Scene;

using SceneStack = std::stack<std::unique_ptr<Scene>>;

class Scene {
public:
    virtual void UpdateLogic(const long long dt) = 0;
    virtual void UpdateState(SceneStack & state) = 0;
    virtual void Display() = 0;
    virtual ~Scene() {}
};

class TitleScreen : public Scene {
public:
    void UpdateLogic(const long long dt) override;
    void UpdateState(SceneStack & state) override;
    void Display() override;
};

class WorldLoader : public Scene {
    bool m_active;
    ThreadGuard m_terrainThread;
public:
    WorldLoader();
    void UpdateLogic(const long long dt) override;
    void UpdateState(SceneStack & state) override;
    void Display() override;
    ~WorldLoader() {
	m_active = false;
    }
};

class World : public Scene {
    void DrawTerrain();
    void DrawSky();
    void DrawOverlays();
    void UpdatePerspProjUniforms();
    void UpdateOrthoProjUniforms();
public:
    World();
    void UpdateLogic(const long long dt) override;
    void UpdateState(SceneStack & state) override;
    void Display() override;
};

class Menu : public Scene {
public:
    void UpdateLogic(const long long dt) override;
    void UpdateState(SceneStack & state) override;
    void Display() override;
};

class MenuTransitionIn : public World {
    long long m_transitionTimer;
    static const long long TRANSITION_TIME = 300000;
public:
    MenuTransitionIn();
    void UpdateLogic(const long long dt) override;
    void UpdateState(SceneStack & state) override;
    void Display() override;
};

class MenuTransitionOut : public World {
    long long m_transitionTimer;
    static const long long TRANSITION_TIME = 300000;
public:
    MenuTransitionOut();
    void UpdateLogic(const long long dt) override;
    void UpdateState(SceneStack & state) override;
    void Display() override;
};
