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
	virtual void Dispatch(const Plane & plane) = 0;
	virtual void Dispatch(const Coin & coin) = 0;
	virtual void Dispatch(const TerrainChunk & chunk) = 0;
	virtual void Dispatch(const Bullet & bullet) = 0;
	virtual void Dispatch(const Thruster & thruster) = 0;
	virtual void Dispatch(const TitleScreen & titleScreen) = 0;
	virtual void Dispatch(const CreditsScreen & creditsScreen) = 0;
	virtual void Dispatch(const WorldLoader & worldLoader) = 0;
	virtual void Dispatch(const World & world) = 0;
	virtual void Dispatch(const WorldTransitionIn & wtin) = 0;
	virtual void Dispatch(const WorldTransitionOut & wtout) = 0;
	virtual void Dispatch(const Menu & menu) = 0;
	virtual void Dispatch(const MenuTransitionIn & mtin) = 0;
	virtual void Dispatch(const MenuTransitionOut & mtout) = 0;
	virtual void Dispatch(const SkyManager & sky) = 0;
    };

    class OpenGLDisplayImpl : public DisplayDispatcher {
    public:
	void Dispatch(const Plane & plane) override;
	void Dispatch(const Coin & coin) override;
	void Dispatch(const TerrainChunk & chunk) override;
	void Dispatch(const Bullet & bullet) override;
	void Dispatch(const Thruster & thruster) override;
	void Dispatch(const TitleScreen & titleScreen) override;
	void Dispatch(const CreditsScreen & creditsScreen) override;
	void Dispatch(const WorldLoader & worldLoader) override;
	void Dispatch(const World & world) override;
	void Dispatch(const WorldTransitionIn & wtin) override;
	void Dispatch(const WorldTransitionOut & wtout) override;
	void Dispatch(const Menu & menu) override;
	void Dispatch(const MenuTransitionIn & mtin) override;
	void Dispatch(const MenuTransitionOut & mtout) override;
	void Dispatch(const SkyManager & sky) override;
    };
}
