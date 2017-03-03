#pragma once

#include <map>
#include <list>
#include <memory>

#include "Entity.hpp"
#include "TerrainManager.hpp"

namespace FLIGHT {    
    struct TerrainCollider : public Solid {
    public:
	inline void OnCollide(Solid &) override {};
	inline void Display(ShaderProgram &) override {};
	inline void Update(const Time) override {};
    };

    class Sector {
	std::vector<std::weak_ptr<Solid>> m_solids;
    public:
	std::list<std::pair<std::shared_ptr<Solid>, std::shared_ptr<Solid>>> GetPairs() const;
	std::vector<std::weak_ptr<Solid>> & GetSolids();
    };

    class CollisionManager {
	void Relocate(const std::pair<int, int> & locus); 
	using SectorTree = std::map<std::pair<int, int>, Sector>;
	SectorTree m_sectorTree;
	void UpdateSector(const std::pair<int, int> & coord, Sector & sector);
    public:
	void Update();
	void AddSolid(std::shared_ptr<Solid> solid);
    };
}
