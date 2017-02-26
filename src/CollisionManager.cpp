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
    for (auto & solidWp : sector.solids) {
	auto solid = solidWp.lock();
	static const float TERRAIN_MAX_HEIGHT = 9.f;
	if (solid->GetPosition().y < TERRAIN_MAX_HEIGHT) {
	    // check collision with terrain...
	}
    }
}

std::pair<int, int> CollisionManager::CalcTargetSector(const glm::vec3 & pos) {
    const float displ = TerrainManager::vertSpacing * Chunk::GetSidelength();
    return {pos.x / displ, pos.z / displ};
}

void CollisionManager::AddSolid(std::shared_ptr<Solid> solid) {
    m_sectorTree[CalcTargetSector(solid->GetPosition())].solids.push_back(solid);
}
