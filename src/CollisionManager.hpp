#pragma once

#include <map>
#include <list>
#include <memory>

#include "Entity.hpp"
#include "TerrainManager.hpp"

namespace FLIGHT {    
    struct TerrainCollider : public Solid {
    public:
	inline void Display(ShaderProgram &) override {};
	inline void Update(const Time) override {};
    };
    
    class Sector {
	std::vector<std::weak_ptr<Solid>> m_solids;
	std::vector<std::pair<std::weak_ptr<Solid>, std::weak_ptr<Solid>>> m_pairs;
	bool m_pairsSetIsStale = false;
    public:
	void MarkStale();
	std::vector<std::pair<std::weak_ptr<Solid>, std::weak_ptr<Solid>>> & GetPairs();
	std::vector<std::weak_ptr<Solid>> & GetSolids();
    };
    
    class CollisionManager {
	using SectorTree = std::map<std::pair<int, int>, Sector>;
	SectorTree m_sectorTree;
	void RelocChildrenIfOutsideSector(const std::pair<int, int> & coord, Sector & sector);
	void PairwiseCollisionTest(Sector & sector);
	void UpdateSector(const std::pair<int, int> & coord, Sector & sector);
	std::mutex m_sectorsMtx;
    public:
	void Update();
	void DisplayAABBs(ShaderProgram & shader);
	void AddSolid(std::shared_ptr<Solid> solid);
    };
}
