#include <FLIGHT/Util/Random.hpp>

namespace FLIGHT {
namespace RANDOM {
static thread_local std::mt19937 g_twister;

void Seed(const unsigned value) { g_twister.seed(value); }

unsigned Get() { return g_twister(); }
}
}
