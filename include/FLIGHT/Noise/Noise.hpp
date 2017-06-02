#pragma once

#include "Matrix.hpp"
#include <FLIGHT/Core/ChunkSize.hpp>
#include <FLIGHT/Util/MemPool32.hpp>

namespace FLIGHT {
using HeightMap = FloatMatrix<CHUNK_SIZE, CHUNK_SIZE>;

using HeightMapRef = std::unique_ptr<HeightMap, void(*)(HeightMap *)>;
    
HeightMapRef MemPoolAllocHeightMap();
}
