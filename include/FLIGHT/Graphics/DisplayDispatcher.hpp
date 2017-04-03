#pragma once

namespace FLIGHT {    
    class Plane;
    class Coin;
    class TerrainChunk;
    class Bullet;
    class Thruster;
    class TitleScreen;
    class CreditsScreen;
    class WorldLoader;
    class World;
    class WorldTransitionIn;
    class WorldTransitionOut;
    class Menu;
    class MenuTransitionIn;
    class MenuTransitionOut;
    class SkyManager;
    
    class DisplayDispatcher {
    public:
	virtual void Dispatch(Plane & plane) = 0;
	virtual void Dispatch(Coin & coin) = 0;
	virtual void Dispatch(TerrainChunk & chunk) = 0;
	virtual void Dispatch(Bullet & bullet) = 0;
	virtual void Dispatch(Thruster & thruster) = 0;
	virtual void Dispatch(TitleScreen & titleScreen) = 0;
	virtual void Dispatch(CreditsScreen & creditsScreen) = 0;
	virtual void Dispatch(WorldLoader & worldLoader) = 0;
	virtual void Dispatch(World & world) = 0;
	virtual void Dispatch(WorldTransitionIn & wtin) = 0;
	virtual void Dispatch(WorldTransitionOut & wtout) = 0;
	virtual void Dispatch(Menu & menu) = 0;
	virtual void Dispatch(MenuTransitionIn & mtin) = 0;
	virtual void Dispatch(MenuTransitionOut & mtout) = 0;
	virtual void Dispatch(SkyManager & sky) = 0;
    };

    class OpenGLDisplayImpl : public DisplayDispatcher {
    public:
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
	void Dispatch(WorldTransitionOut & wtout) override;
	void Dispatch(Menu & menu) override;
	void Dispatch(MenuTransitionIn & mtin) override;
	void Dispatch(MenuTransitionOut & mtout) override;
	void Dispatch(SkyManager & sky) override;
    };
}
