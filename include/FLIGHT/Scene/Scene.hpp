#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <stack>

#include <FLIGHT/Core/Camera.hpp>
#include <FLIGHT/Core/ThreadGuard.hpp>
#include <FLIGHT/Core/Time.hpp>
#include <FLIGHT/UI/UI.hpp>
#include <FLIGHT/Core/Util.hpp>
#include <FLIGHT/Graphics/DisplayImpl.hpp>

namespace FLIGHT {
class Scene;

extern std::mutex g_updateMtx;

void DisplayShadowOverlay(const float amount);

using SceneStack = std::stack<std::shared_ptr<Scene>>;

class Scene {
public:
    virtual void UpdateLogic(const Time dt) = 0;
    virtual void UpdateState(SceneStack & state) = 0;
    virtual void Display(DisplayImpl & gfx) = 0;
    virtual ~Scene() {}
};

class CreditsScreen : public Scene {
    Time m_timer;
    Text m_text;
    enum class State { BeginDelay, Enter, Wait, Exit, PostDelay, Done };
    State m_state;

public:
    CreditsScreen();
    Text & GetText() {
	return m_text;
    }
    void UpdateLogic(const Time dt) override;
    void UpdateState(SceneStack & state) override;
    void Display(DisplayImpl & gfx) override;
};

class TitleScreen : public Scene {
    Text m_titleText, m_startText;
    Time m_timer;
    void UpdateProjUniforms();
    std::shared_ptr<Plane> m_plane;
    Time m_transitionTimer;
    enum class State { ShowTitle, PlaneSelectorEntry, PlaneSelector };
    State m_state;

public:
    TitleScreen();
    void UpdateLogic(const Time dt) override;
    void UpdateState(SceneStack & state) override;
    void Display(DisplayImpl & gfx) override;
};

class WorldLoader : public Scene {
    bool m_active;
    ThreadGuard m_terrainThread;

public:
    WorldLoader();
    void UpdateLogic(const Time dt) override;
    void UpdateState(SceneStack & state) override;
    void Display(DisplayImpl & gfx) override;
    ~WorldLoader() { m_active = false; }
};

class World : public Scene {
    void DrawTerrain();
    void DrawSky();
    void DrawOverlays();
    void UpdatePerspProjUniforms();
    void UpdateOrthoProjUniforms();
    
public:
    World();
    void UpdateLogic(const Time dt) override;
    void UpdateState(SceneStack & state) override;
    void Display(DisplayImpl & gfx) override;
};

class WorldTransitionIn : public World {
    Time m_transitionTimer;
public:
    static const Time TRANSITION_TIME = 700000;
    void UpdateLogic(const Time dt) override;
    void UpdateState(SceneStack & state) override;
    void Display(DisplayImpl & gfx) override;
    Time GetTimer() const {
	return m_transitionTimer;
    }
};
}
