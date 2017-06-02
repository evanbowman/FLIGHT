#include <FLIGHT/Noise/Noise.hpp>

namespace FLIGHT {
// 1024 Should be more than sufficient, but feel free to mess with it if FLIGHT
// uses too much memory.
static MemPool32<HeightMap, 1024> heightMapPool;

static void MemPoolDeallocHeightMap(HeightMap * map) {
    heightMapPool.Delete(map);
}

HeightMapRef MemPoolAllocHeightMap() {
    return HeightMapRef(heightMapPool.New(), MemPoolDeallocHeightMap);
}
}
