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

struct InputWrap {
    std::unique_ptr<RotationProvider> joystick;
};

static const glm::mat4 LIGHT_PROJ_MAT = glm::ortho(-4.f, 4.f, -4.f, 4.f, -5.f, 12.f);	

class Game {
    sf::Window m_window;
    bool m_running;
    Camera m_camera;
    AssetManager m_assetManager;
    Player m_player;
    GLuint m_shadowMapFB;
    GLuint m_shadowMapTxtr;
    TerrainManager m_terrainManager;
    SkyManager m_skyManager;
    SmoothDTProvider m_smoothDTProv;
    std::stack<std::unique_ptr<Scene>> m_scenes;
    void SetupShadowMap();
    void PollEvents();
    void UpdateLogic(const Time dt);
    void UpdateGraphics();
    std::mutex m_sceneStackMtx;
    InputWrap m_input;
public:
    Game(const std::string & name);
    void Run();
    bool IsRunning() const;
    AssetManager & GetAssets();
    InputWrap & GetInput();
    TerrainManager & GetTerrain();
    SkyManager & GetSky();
    Camera & GetCamera();
    Player & GetPlayer();
    GLuint GetShadowMapTxtr() const;
    sf::Vector2<unsigned> GetWindowSize() const;
    Game(const Game &) = delete;
    void DrawShadowMap();
};

Game & GetGame();

