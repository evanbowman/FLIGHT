#pragma once

#include <list>
#include <map>
#include <memory>
#include <FLIGHT/Entity/Entity.hpp>
#include <FLIGHT/Core/TerrainManager.hpp>

namespace FLIGHT {
struct TerrainCollider : public Solid {
public:
    inline void Display(ShaderProgram &) override{};
    inline void Update(const Time) override{};
};

class Sector {
    std::vector<std::weak_ptr<Solid>> m_dynamicSolids;
    std::vector<std::weak_ptr<Solid>> m_staticSolids;
    std::vector<std::pair<std::weak_ptr<Solid>, std::weak_ptr<Solid>>> m_pairs;
    bool m_pairsSetIsStale = false;

public:
    void MarkStale();
    std::vector<std::pair<std::weak_ptr<Solid>, std::weak_ptr<Solid>>> &
    GetPairs();
    std::vector<std::weak_ptr<Solid>> & GetStaticSolids();
    std::vector<std::weak_ptr<Solid>> & GetDynamicSolids();
};

class CollisionManager {
    using SectorTree = std::map<std::pair<int, int>, Sector>;
    SectorTree m_sectorTree;
    void PairwiseCollisionTest(Sector & sector);
    void UpdateSector(const std::pair<int, int> & coord, Sector & sector);
    std::mutex m_sectorsMtx;

public:
    void Update();
    void AddSolid(std::shared_ptr<Solid> solid);
};
}
