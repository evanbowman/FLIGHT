#pragma once
#include <array>
#include <new>

template <typename ValueType, size_t Capacity>
class MemPool32 {
	std::array<uint8_t, Capacity * sizeof(ValueType)> m_memory;
	std::array<uint32_t, Capacity / 32> m_mask;
	
public:
	MemPool32() : m_memory{}, m_mask{} {
		static_assert(Capacity % 32 == 0,
					  "Pool capacity must be a multiple of 32");
	}
	
	ValueType * New() {
		for (int i = 0; i < m_mask.size(); ++i) {
			const auto m = m_mask[i];
			if (m != 0xFFFFFFFF) {
				static const int deBruijnTable[32] = {
					0,  1,  28, 2,  29, 14, 24, 3,
					30, 22, 20, 15, 25, 17, 4,  8,
					31, 27, 13, 23, 21, 19, 16, 7,
					26, 12, 18, 6,  11, 5,  10, 9
				};
				int r = deBruijnTable[((uint32_t)((~m & -~m) * 0x077CB531U)) >> 27];
				m_mask[i] |= ~m & (m + 1);
				auto result = (ValueType *)(&m_memory[i * 32 * sizeof(ValueType)]
											+ r * sizeof(ValueType));
				new(result) ValueType;
				return result;
			}
		}
		return nullptr;
	}
	
	void Delete(ValueType * mem) {
		if (mem) {
			mem->~ValueType();
			const auto off = (mem - (ValueType *)m_memory.data()) % 32;
			const auto bin = (mem - (ValueType *)m_memory.data()) / 32;
			m_mask[bin] &= ~(static_cast<uint32_t>(1) << off);
		}
	}
};
