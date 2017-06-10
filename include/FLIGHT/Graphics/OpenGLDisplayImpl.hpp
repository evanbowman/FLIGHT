#pragma once

#include <FLIGHT/Graphics/DisplayImpl.hpp>
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace FLIGHT {
class OpenGLDisplayImpl : public DisplayImpl {
    struct {
        GLuint chunkIndicesHQ;
        GLuint chunkIndicesMQ;
        GLuint chunkIndicesLQ;
        GLuint chunkIndicesDQ;
    } m_chunkInfo;

    GLuint m_vao;
    GLuint m_shadowMapFBO;
    GLuint m_shadowMapTxtr;

    void InitChunkIndexBufs();
    void SetupShadowMap();

public:
    OpenGLDisplayImpl();
    ~OpenGLDisplayImpl();
    OpenGLDisplayImpl & operator=(const OpenGLDisplayImpl &) = delete;
    void Dispatch(Plane & plane) override;
    void Dispatch(Coin & coin) override;
    void Dispatch(TerrainChunk & chunk) override;
    void Dispatch(Bullet & bullet) override;
    void Dispatch(Thruster & thruster) override;
    void Dispatch(TitleScreen & titleScreen) override;
    void Dispatch(CreditsScreen & creditsScreen) override;
    void Dispatch(WorldLoader & worldLoader) override;
    void Dispatch(World & world) override;
    void Dispatch(WorldTransitionIn & wtin) override;
    void Dispatch(SkyManager & sky) override;
    void DrawShadowMap();
};
}
