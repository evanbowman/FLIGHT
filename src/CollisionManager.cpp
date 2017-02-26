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
	if (auto entity = it->lock()) {
	    auto currentLoc = CalcTargetSector(entity->GetPosition());
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
}

std::pair<int, int> CollisionManager::CalcTargetSector(const glm::vec3 & pos) {
    const float displ = TerrainManager::vertSpacing * Chunk::GetSidelength();
    return {pos.x / displ, pos.z / displ};
}

void CollisionManager::AddSolid(std::shared_ptr<Solid> entity) {
    m_sectorTree[CalcTargetSector(entity->GetPosition())].solids.push_back(entity);
}
