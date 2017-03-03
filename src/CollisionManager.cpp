#include "CollisionManager.hpp"

namespace FLIGHT {    
    void CollisionManager::Update() {
	for (auto it = m_sectorTree.begin(); it != m_sectorTree.end();) {
	    if (!it->second.GetSolids().empty()) {
		UpdateSector(it->first, it->second);
		++it;
	    } else {
		it = m_sectorTree.erase(it);
	    }
	}
    }

    // NOTE: this is the max height value generated in libnoise
    // height fields. I wish the library used normalized values so
    // I don't have to define mystery constants... sigh...
    static const float MAX_TERRAIN_HEIGHT = 1.46094;

    static std::pair<int, int> CalcTargetSector(const glm::vec3 & pos) {
	const float displ =
	    TerrainChunk::vertSpacing * TerrainChunk::GetSidelength();
	return {pos.x / displ, pos.z / displ};
    }

    std::vector<std::weak_ptr<Solid>> & Sector::GetSolids() { return m_solids; }

    std::list<std::pair<std::shared_ptr<Solid>, std::shared_ptr<Solid>>>
    Sector::GetPairs() const {
	decltype(((Sector *)nullptr)->GetPairs()) pairs;
	for (size_t i = 0; i < m_solids.size(); ++i) {
	    for (size_t j = i + 1; j < m_solids.size(); ++j) {
		pairs.push_back({m_solids[i].lock(), m_solids[j].lock()});
	    }
	}
	return pairs;
    }

    void CollisionManager::UpdateSector(const std::pair<int, int> & coord,
					Sector & sector) {
	for (auto it = sector.GetSolids().begin();
	     it != sector.GetSolids().end();) {
	    if (auto solid = it->lock()) {
		auto currentLoc = CalcTargetSector(solid->GetPosition());
		if (currentLoc != coord) {
		    m_sectorTree[currentLoc].GetSolids().push_back(*it);
		    it = sector.GetSolids().erase(it);
		} else {
		    ++it;
		}
	    } else {
		it = sector.GetSolids().erase(it);
	    }
	}
	auto pairs = sector.GetPairs();
	for (auto & pair : pairs) {
	    if (pair.first->GetAABB().Intersects(pair.second->GetAABB())) {
		// TODO: collisions should be WAAAAY more precise
		// Idea: if AABBs intersect, get set of component OBBs and test
		// those
		pair.first->OnCollide(*pair.second);
		pair.second->OnCollide(*pair.first);
	    }
	}
    }

    void CollisionManager::AddSolid(std::shared_ptr<Solid> solid) {
	m_sectorTree[CalcTargetSector(solid->GetPosition())].GetSolids().push_back(
										   solid);
    }
}
