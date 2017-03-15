#pragma once

#include <string>
#ifdef FLIGHT_MAC
#include <OpenGL/gl3.h>
#elif FLIGHaT_WINDOWS
#include <GL/glew.h>
#include <SFML/Window.hpp>
#endif
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <forward_list>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <stack>
#include <stdio.h>
#include <thread>
#include <tuple>

#include "AssetManager.hpp"
#include "Camera.hpp"
#include "CollisionManager.hpp"
#include "ConfigData.hpp"
#include "Entity.hpp"
#include "Error.hpp"
#include "Font.hpp"
#include "InputModes.hpp"
#include "LoadBlueprints.hpp"
#include "Player.hpp"
#include "Scene.hpp"
#include "Sky.hpp"
#include "SmoothDTProvider.hpp"
#include "Sprite.hpp"
#include "TerrainManager.hpp"
#include "ThreadGuard.hpp"
#include "UpdateCap.hpp"

namespace FLIGHT {
struct InputWrap {
    std::unique_ptr<Joystick> joystick;
    std::unique_ptr<ButtonSet> buttonSet;
};

class Game {
    friend class Patch;
    ConfigData m_conf;
    sf::Window m_window;
    bool m_running;
    std::unique_ptr<Camera> m_camera;
    AssetManager m_assetManager;
    CollisionManager m_collisionManager;
    Player m_player;
    Texture m_stash;
    GLuint m_shadowMapFB;
    GLuint m_shadowMapTxtr;
    TerrainManager m_terrainManager;
    SkyManager m_skyManager;
    SmoothDTProvider m_smoothDTProv;
    std::stack<std::shared_ptr<Scene>> m_scenes;
    void SetupShadowMap();
    void PollEvents();
    std::mutex m_sceneStackMtx;
    InputWrap m_input;
    bool m_focused;
    PlaneRegistry m_planesRegistry;
    std::vector<std::exception_ptr> m_threadExceptions;
    void LogicLoop();
    void SetupGL();
    void TryBindGamepad(const sf::Joystick::Identification & ident);
    std::mutex m_entitiesMtx;
    std::forward_list<std::shared_ptr<Entity>> m_entities;
    time_t m_seed;

public:
    Game(const ConfigData & conf);
    ~Game();
    void SetSeed(const time_t seed);
    time_t GetSeed() const;
    void Run();
    bool IsRunning() const;
    AssetManager & GetAssetMgr();
    ConfigData & GetConf();
    InputWrap & GetInput();
    PlaneRegistry & GetPlaneRegistry();
    TerrainManager & GetTerrainMgr();
    SkyManager & GetSkyMgr();
    CollisionManager & GetCollisionMgr();
    void StashWindow();
    void DisplayStash();
    Camera & GetCamera();
    void Restart();
    void SetCamera(std::unique_ptr<Camera> camera);
    Player & GetPlayer();
    void UpdateEntities(const Time dt);
    GLuint GetShadowMapTxtr() const;
    void NotifyThreadExceptionOccurred(std::exception_ptr ex);
    sf::Vector2<unsigned> GetWindowSize() const;
    Game(const Game &) = delete;
    void DrawShadowMap();
    template <typename T, typename... Args>
    std::shared_ptr<T> CreateSolid(Args &&... args) {
        auto solid = std::make_shared<T>(args...);
        {
            std::lock_guard<std::mutex> lk(m_entitiesMtx);
            m_entities.insert_after(m_entities.before_begin(), solid);
        }
        m_collisionManager.AddSolid(solid);
        return solid;
    }
};

Game & GetGame();

struct Patch {
    static void SubvertMacOSKernelPanics(Game & game);
    static void ShadowMapPreliminarySweep(Game & game);
};
}
