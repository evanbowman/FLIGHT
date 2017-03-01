#pragma once

#include <string>
#include <OpenGL/gl3.h>
#include <iostream>
#include <thread>
#include <tuple>
#include <stdio.h>
#include <chrono>
#include <algorithm>
#include <array>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Sprite.hpp"
#include "AssetManager.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "Entity.hpp"
#include "Camera.hpp"
#include "RedTail.hpp"
#include "Player.hpp"
#include "Error.hpp"
#include "TerrainManager.hpp"
#include "ThreadGuard.hpp"
#include "UpdateCap.hpp"
#include "InputModes.hpp"
#include <stack>
#include "Scene.hpp"
#include "SmoothDTProvider.hpp"
#include "Sky.hpp"
#include "Font.hpp"
#include "CollisionManager.hpp"
#include "ConfigData.hpp"

struct InputWrap {
    std::unique_ptr<Joystick> joystick;
    std::unique_ptr<ButtonSet> buttonSet;
};

class Game {
    ConfigData m_conf;
    sf::Window m_window;
    bool m_running;
    Camera m_camera;
    AssetManager m_assetManager;
    CollisionManager m_collisionManager;
    Player m_player;
    GLuint m_shadowMapFB;
    GLuint m_shadowMapTxtr;
    TerrainManager m_terrainManager;
    SkyManager m_skyManager;
    SmoothDTProvider m_smoothDTProv;
    std::stack<std::shared_ptr<Scene>> m_scenes;
    void SetupShadowMap();
    void PollEvents();
    void UpdateLogic(const Time dt);
    void UpdateGraphics();
    std::mutex m_sceneStackMtx;
    InputWrap m_input;
    bool m_focused;
    std::vector<std::exception_ptr> m_threadExceptions;
    void LogicLoop();
    void PromoteExceptionToOSDialogBox(const std::exception & ex);
public:
    Game(const ConfigData & conf);
    ~Game();
    void Run();
    bool IsRunning() const;
    AssetManager & GetAssetMgr();
    InputWrap & GetInput();
    TerrainManager & GetTerrainMgr();
    SkyManager & GetSkyMgr();
    CollisionManager & GetCollisionMgr();
    Camera & GetCamera();
    Player & GetPlayer();
    GLuint GetShadowMapTxtr() const;
    void NotifyThreadExceptionOccurred(std::exception_ptr ex);
    sf::Vector2<unsigned> GetWindowSize() const;
    Game(const Game &) = delete;
    void DrawShadowMap();
    template <typename T>
    std::shared_ptr<T> CreateSolid() {
	auto solid = std::make_shared<T>();
	m_collisionManager.AddSolid(solid);
	return solid;
    }
};

Game & GetGame();

