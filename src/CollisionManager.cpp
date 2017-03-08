#include "CollisionManager.hpp"
#include "Game.hpp"

namespace FLIGHT {
void CollisionManager::Update() {
    std::lock_guard<std::mutex> lk(m_sectorsMtx);
    for (auto it = m_sectorTree.begin(); it != m_sectorTree.end();) {
        if (!it->second.GetSolids().empty()) {
            UpdateSector(it->first, it->second);
            ++it;
        } else {
            it = m_sectorTree.erase(it);
        }
    }
}

static std::pair<int, int> CalcTargetSector(const glm::vec3 & pos) {
    const float displ =
        TerrainChunk::vertSpacing * TerrainChunk::GetSidelength();
    return {pos.x / displ, pos.z / displ};
}

std::vector<std::weak_ptr<Solid>> & Sector::GetSolids() { return m_solids; }

void Sector::MarkStale() { m_pairsSetIsStale = true; }

std::vector<std::pair<std::weak_ptr<Solid>, std::weak_ptr<Solid>>> &
Sector::GetPairs() {
    if (m_pairsSetIsStale) {
        m_pairs.clear();
        for (size_t i = 0; i < m_solids.size(); ++i) {
            for (size_t j = i + 1; j < m_solids.size(); ++j) {
                m_pairs.push_back({m_solids[i], m_solids[j]});
            }
        }
        m_pairsSetIsStale = false;
    }
    return m_pairs;
}

void CollisionManager::DisplayAABBs(ShaderProgram & shader) {
    std::lock_guard<std::mutex> lk(m_sectorsMtx);
    for (auto & sectorNode : m_sectorTree) {
        for (auto & solid : sectorNode.second.GetSolids()) {
            if (auto solidSp = solid.lock()) {
                solidSp->GetAABB().Display(shader);
            }
        }
    }
}

void CollisionManager::RelocChildrenIfOutsideSector(
    const std::pair<int, int> & coord, Sector & sector) {
    for (auto it = sector.GetSolids().begin();
         it != sector.GetSolids().end();) {
        if (auto solid = it->lock()) {
            auto currentLoc = CalcTargetSector(solid->GetPosition());
            if (currentLoc != coord) {
                m_sectorTree[currentLoc].GetSolids().push_back(*it);
                m_sectorTree[currentLoc].MarkStale();
                it = sector.GetSolids().erase(it);
                sector.MarkStale();
            } else {
                ++it;
            }
        } else {
            it = sector.GetSolids().erase(it);
            sector.MarkStale();
        }
    }
}

inline static bool PreciseCollisionTest(Solid & lhs, Solid & rhs) {
    // TODO: implement me
    return true;
}

void CollisionManager::PairwiseCollisionTest(Sector & sector) {
    auto & pairs = sector.GetPairs();
    for (auto & pair : pairs) {
        auto lhs = pair.first.lock();
        auto rhs = pair.second.lock();
        if (!lhs || !rhs) {
            continue;
        }
	if (lhs->GetMBS().Intersects(rhs->GetMBS())) {
	    if (lhs->GetAABB().Intersects(rhs->GetAABB())) {
		if (PreciseCollisionTest(*lhs, *rhs)) {
		    rhs->SendMessage(std::make_unique<Collision>(lhs));
		    lhs->SendMessage(std::make_unique<Collision>(rhs));
		}
	    }
	}
    }
}

void CollisionManager::UpdateSector(const std::pair<int, int> & coord,
                                    Sector & sector) {
    RelocChildrenIfOutsideSector(coord, sector);
    PairwiseCollisionTest(sector);
}

void CollisionManager::AddSolid(std::shared_ptr<Solid> solid) {
    std::lock_guard<std::mutex> lk(m_sectorsMtx);
    m_sectorTree[CalcTargetSector(solid->GetPosition())].GetSolids().push_back(
        solid);
}
}
