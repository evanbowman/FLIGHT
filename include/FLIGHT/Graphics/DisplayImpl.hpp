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

class DisplayImpl {
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
    virtual void Dispatch(SkyManager & sky) = 0;
};
}
