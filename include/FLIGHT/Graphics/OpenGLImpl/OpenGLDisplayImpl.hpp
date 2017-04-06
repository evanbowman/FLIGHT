 #pragma once

#include <FLIGHT/Graphics/DisplayImpl.hpp>
#ifdef FLIGHT_MAC
#include <OpenGL/gl3.h>
#elif FLIGHT_LINUX
#include <GL/glew.h>
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SFML/Window.hpp>

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
