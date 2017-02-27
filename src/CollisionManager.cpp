#include "CollisionManager.hpp"

void CollisionManager::Update() {
    for (auto it = m_sectorTree.begin(); it != m_sectorTree.end();) {
	if (!it->second.solids.empty()) {
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

void CollisionManager::UpdateSector(const std::pair<int, int> & coord, Sector & sector) {
    for (auto it = sector.solids.begin(); it != sector.solids.end();) {
	if (auto solid = it->lock()) {
	    auto currentLoc = CalcTargetSector(solid->GetPosition());
	    if (currentLoc != coord) {
		m_sectorTree[currentLoc].solids.push_back(*it);
		it = sector.solids.erase(it);
	    } else {
		++it;
	    }
	} else {
	    it = sector.solids.erase(it);
	}
    }
    std::list<std::pair<std::weak_ptr<Solid>, std::weak_ptr<Solid>>> pairs;
    for (size_t i = 0; i < sector.solids.size(); ++i) {
	for (size_t j = i + 1; j < sector.solids.size(); ++j) {
	    pairs.push_back({sector.solids[i], sector.solids[j]});
	}
    }
    for (auto & pair : pairs) {
	// ...
    }
}

std::pair<int, int> CollisionManager::CalcTargetSector(const glm::vec3 & pos) {
    const float displ = TerrainChunk::vertSpacing * TerrainChunk::GetSidelength();
    return {pos.x / displ, pos.z / displ};
}

void CollisionManager::AddSolid(std::shared_ptr<Solid> solid) {
    m_sectorTree[CalcTargetSector(solid->GetPosition())].solids.push_back(solid);
}
