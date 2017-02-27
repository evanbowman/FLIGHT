#pragma once

#include <map>
#include <list>
#include <memory>

#include "Entity.hpp"
#include "TerrainManager.hpp"

struct TerrainCollider : public Solid {
public:
    inline void OnCollide(Solid &) override {};
    inline void Display(const GLuint) override {};
    inline void Update(const Time) override {};
};

struct Sector {
    std::list<std::weak_ptr<Solid>> solids;
};

class CollisionManager {
protected:
    using SectorTree = std::map<std::pair<int, int>, Sector>;
    SectorTree m_sectorTree;
    void UpdateSector(const std::pair<int, int> & coord, Sector & sector);
    std::pair<int, int> CalcTargetSector(const glm::vec3 & pos);
public:
    void Update();
    void AddSolid(std::shared_ptr<Solid> solid);
};
