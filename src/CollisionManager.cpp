#include "CollisionManager.hpp"
#include "Game.hpp"

namespace FLIGHT {
void CollisionManager::Update() {
    std::lock_guard<std::mutex> lk(m_sectorsMtx);
    for (auto it = m_sectorTree.begin(); it != m_sectorTree.end();) {
        if (!it->second.GetStaticSolids().empty() ||
            !it->second.GetDynamicSolids().empty()) {
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

std::vector<std::weak_ptr<Solid>> & Sector::GetStaticSolids() {
    return m_staticSolids;
}

std::vector<std::weak_ptr<Solid>> & Sector::GetDynamicSolids() {
    return m_dynamicSolids;
}

void Sector::MarkStale() { m_pairsSetIsStale = true; }

std::vector<std::pair<std::weak_ptr<Solid>, std::weak_ptr<Solid>>> &
Sector::GetPairs() {
    if (m_pairsSetIsStale) {
        m_pairs.clear();
        for (size_t i = 0; i < m_dynamicSolids.size(); ++i) {
            for (size_t j = i + 1; j < m_dynamicSolids.size(); ++j) {
                m_pairs.push_back({m_dynamicSolids[i], m_dynamicSolids[j]});
            }
            for (size_t j = 0; j < m_staticSolids.size(); ++j) {
                m_pairs.push_back({m_dynamicSolids[i], m_staticSolids[j]});
            }
        }
        m_pairsSetIsStale = false;
    }
    return m_pairs;
}

inline static void TerrainCollisionTest(Solid & solid,
                                        const std::pair<int, int> & coord) {
    MBS mbs = solid.GetMBS();
    if ((mbs.GetCenter() - mbs.GetRadius()).y <
        TerrainChunk::GetMaxElevation()) {
        // TODO: ...
    }
}

inline static bool PreciseCollisionCheck(Solid & lhs, Solid & rhs) {
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
                if (PreciseCollisionCheck(*lhs, *rhs)) {
                    rhs->SendMessage(std::make_unique<Collision>(lhs));
                    lhs->SendMessage(std::make_unique<Collision>(rhs));
                }
            }
        }
    }
}

void CollisionManager::UpdateSector(const std::pair<int, int> & coord,
                                    Sector & sector) {
    for (auto it = sector.GetDynamicSolids().begin();
         it != sector.GetDynamicSolids().end();) {
        if (auto solid = it->lock()) {
            auto currentLoc = CalcTargetSector(solid->GetPosition());
            if (currentLoc != coord) {
                m_sectorTree[currentLoc].GetDynamicSolids().push_back(*it);
                m_sectorTree[currentLoc].MarkStale();
                it = sector.GetDynamicSolids().erase(it);
                sector.MarkStale();
            } else {
                TerrainCollisionTest(*solid, coord);
                ++it;
            }
        } else {
            it = sector.GetDynamicSolids().erase(it);
            sector.MarkStale();
        }
    }
    for (auto it = sector.GetStaticSolids().begin();
         it != sector.GetStaticSolids().end();) {
        if (!it->lock()) {
            it = sector.GetStaticSolids().erase(it);
            sector.MarkStale();
        } else {
            ++it;
        }
    }
    PairwiseCollisionTest(sector);
}

void CollisionManager::AddSolid(std::shared_ptr<Solid> solid) {
    std::lock_guard<std::mutex> lk(m_sectorsMtx);
    auto targetSector = CalcTargetSector(solid->GetPosition());
    if (solid->IsStatic()) {
        m_sectorTree[targetSector].GetStaticSolids().push_back(solid);
    } else {
        m_sectorTree[targetSector].GetDynamicSolids().push_back(solid);
    }
}
}
