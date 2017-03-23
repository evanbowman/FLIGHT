#include <FLIGHT/Core/Blueprint.hpp>

namespace FLIGHT {
void Blueprint::AddPart(const Part & part) { m_parts.push_back(part); }

const std::vector<Blueprint::Part> & Blueprint::GetParts() const {
    return m_parts;
}
}
