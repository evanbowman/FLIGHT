#pragma once

#include <list>
#include <map>
#include <memory>

#include "Entity.hpp"
#include "TerrainManager.hpp"

namespace FLIGHT {
/// @class Sector
/// @brief A slab of the world's 3D space maintained by the CollisionManager
///
/// Sector maintains lists of weak references to StaticSolids and DynamicSolids.
/// Whenever these lists change, it rebuilds a vector containing the complete
/// set of all pairs of dynamic solids, and a set of all pairs of dynamic solids
/// and static solids. The sector does not generate pairs for static solids,
/// because they do not move and it may be assumed that they will not collide to
/// do any motion of their own.
class Sector {
    std::vector<std::weak_ptr<Solid>> m_dynamicSolids;
    std::vector<std::weak_ptr<Solid>> m_staticSolids;
    std::vector<std::pair<std::weak_ptr<Solid>, std::weak_ptr<Solid>>> m_pairs;
    bool m_pairsSetIsStale = false;

public:
    /// @brief Tell the sector to recalculate its pairs list.
    void MarkStale();

    /// @brief Get a reference to the sector's pairs list.
    /// @return The pairs list.
    std::vector<std::pair<std::weak_ptr<Solid>, std::weak_ptr<Solid>>> &
    GetPairs();

    /// @brief Get a reference to the sector's vector of StaticSolids.
    /// @return The vector.
    std::vector<std::weak_ptr<Solid>> & GetStaticSolids();

    /// @brief Get a reference to the sector's vector of DynamicSolids.
    /// @return The vector.
    std::vector<std::weak_ptr<Solid>> & GetDynamicSolids();
};

/// @class CollisionManager
/// @brief Maintains a tree of Sector instances and tracks collisions.
///
/// The collsion manager is an opaque class that handles all of the game's
/// collision tests. Implementation-wise, it stores a tree structure of Sector
/// instances, and moves DynamicSolid instances to the correct Sector when
/// they leave the bounds of the current one. When a sector has no static
/// or dynamic solids, the collision manager deallocates it.
class CollisionManager {
    using SectorTree = std::map<std::pair<int, int>, Sector>;
    SectorTree m_sectorTree;

    /// @brief Tests for collisions for all pairs in a sector's pairs list.
    /// @param The Sector to test.
    void PairwiseCollisionTest(Sector & sector);

    /// @brief Update a sector, and relocate it's DynamicSolids if they've left
    /// the bounds of their current sector.
    void UpdateSector(const std::pair<int, int> & coord, Sector & sector);

    std::mutex m_sectorsMtx;

public:
    /// @brief Run collision testing, and send Solids a Collision Message if
    /// they intersect.
    void Update();

    /// @brief Add a solid to be observed by the CollisionManager. It only
    /// stores a weak references, so it won't keep alive any solids that the
    /// Game has disposed of.
    void AddSolid(std::shared_ptr<Solid> solid);
};
}
