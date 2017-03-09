#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace MATH {
template <typename T> T lerp(const T & A, const T & B, const float t) {
    return A * t + (1 - t) * B;
}

inline glm::mat3 MakeXRotMat(const float radians) {
    // clang-format off
        const float rotX[] = {
            1, 0, 0,
            0, std::cos(radians), -std::sin(radians),
            0, std::sin(radians), std::cos(radians)
        };
    // clang-format on
    return glm::make_mat3(rotX);
}

inline glm::mat3 MakeYRotMat(const float radians) {
    // clang-format off
        const float rotY[] {
            std::cos(radians), 0, std::sin(radians),
            0, 1, 0,
            -std::sin(radians), 0, std::cos(radians)
        };
    // clang-format on
    return glm::make_mat3(rotY);
}

inline glm::mat3 MakeZRotMat(const float radians) {
    // clang-format off
        const float rotZ[] {
            std::cos(radians), -std::sin(radians), 0,
            std::sin(radians), std::cos(radians), 0,
            0, 0, 1
        };
    // clang-format on
    return glm::make_mat3(rotZ);
}
}
