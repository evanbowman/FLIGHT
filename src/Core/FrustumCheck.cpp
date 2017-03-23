#include <FLIGHT/Core/FrustumCheck.hpp>

namespace FLIGHT {
static const float frustIncidence = 45.f;

static const float leftRot[] = {std::cos(glm::radians(-frustIncidence)),
                                0,
                                std::sin(glm::radians(-frustIncidence)),
                                0,
                                1,
                                0,
                                -std::sin(glm::radians(-frustIncidence)),
                                0,
                                std::cos(glm::radians(-frustIncidence))};

static const float rightRot[] = {std::cos(glm::radians(frustIncidence)),
                                 0,
                                 std::sin(glm::radians(frustIncidence)),
                                 0,
                                 1,
                                 0,
                                 -std::sin(glm::radians(frustIncidence)),
                                 0,
                                 std::cos(glm::radians(frustIncidence))};

static const float upRot[] = {1,
                              0,
                              0,
                              0,
                              std::cos(glm::radians(-frustIncidence)),
                              std::sin(glm::radians(-frustIncidence)),
                              0,
                              -std::sin(glm::radians(-frustIncidence)),
                              std::cos(glm::radians(-frustIncidence))};

static const float downRot[] = {1,
                                0,
                                0,
                                0,
                                std::cos(glm::radians(frustIncidence)),
                                std::sin(glm::radians(frustIncidence)),
                                0,
                                -std::sin(glm::radians(frustIncidence)),
                                std::cos(glm::radians(frustIncidence))};

static const glm::mat3 lRotMat = glm::make_mat3(leftRot);
static const glm::mat3 rRotMat = glm::make_mat3(rightRot);
static const glm::mat3 uRotMat = glm::make_mat3(upRot);
static const glm::mat3 dRotMat = glm::make_mat3(downRot);

namespace cached {
static glm::vec3 leftPlaneDir, rightPlaneDir, upPlaneDir, downPlaneDir;
static glm::vec3 eyePos{};
}

bool IntersectsFrustum(const glm::vec3 & objectPos, const glm::vec3 & eyePos,
                       const glm::vec3 & viewDir, const float margin) {
    if (cached::eyePos != eyePos) {
        cached::leftPlaneDir = lRotMat * viewDir;
        cached::rightPlaneDir = rRotMat * viewDir;
        cached::upPlaneDir = uRotMat * viewDir;
        cached::downPlaneDir = dRotMat * viewDir;
    }
    const auto objToEye = objectPos - eyePos;
    const double leftDot = glm::dot(objToEye, cached::leftPlaneDir);
    const double rightDot = glm::dot(objToEye, cached::rightPlaneDir);
    // const double upDot = glm::dot(objToEye, cached::upPlaneDir);
    // const double downDot = glm::dot(objToEye, cached::downPlaneDir);
    return leftDot < margin && rightDot < margin; // && upDot < margin &&
    // downDot < margin;
}
}
