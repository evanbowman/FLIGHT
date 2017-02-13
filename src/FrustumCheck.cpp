#include "FrustumCheck.hpp"

static const float frustIncidence = 45.f;

static const float leftRot[] = {
    std::cos(glm::radians(-frustIncidence)), 0, std::sin(glm::radians(-frustIncidence)),
    0, 1, 0,
    -std::sin(glm::radians(-frustIncidence)), 0, std::cos(glm::radians(-frustIncidence))
};

static const float rightRot[] = {
    std::cos(glm::radians(frustIncidence)), 0, std::sin(glm::radians(frustIncidence)),
    0, 1, 0,
    -std::sin(glm::radians(frustIncidence)), 0, std::cos(glm::radians(frustIncidence))
};

static const float upRot[] = {
    1, 0, 0,
    0, std::cos(glm::radians(-frustIncidence)), std::sin(glm::radians(-frustIncidence)),
    0, -std::sin(glm::radians(-frustIncidence)), std::cos(glm::radians(-frustIncidence))
};

static const float downRot[] = {
    1, 0, 0,
    0, std::cos(glm::radians(frustIncidence)), std::sin(glm::radians(frustIncidence)),
    0, -std::sin(glm::radians(frustIncidence)), std::cos(glm::radians(frustIncidence))
};

static const glm::mat3 lRotMat = glm::make_mat3(leftRot);
static const glm::mat3 rRotMat = glm::make_mat3(rightRot);
static const glm::mat3 uRotMat = glm::make_mat3(upRot);
static const glm::mat3 dRotMat = glm::make_mat3(downRot);

bool IntersectsFrustum(const glm::vec3 & objectPos,
		       const glm::vec3 & eyePos,
		       const glm::vec3 & viewDir,
		       const float margin) {
    const auto leftPlaneDir = lRotMat * viewDir;
    const auto rightPlaneDir = rRotMat * viewDir;
    const auto upPlaneDir = uRotMat * viewDir;
    const auto downPlaneDir = dRotMat * viewDir;
    const auto objToEye = objectPos - eyePos;
    const double leftDot = glm::dot(objToEye, leftPlaneDir);
    const double rightDot = glm::dot(objToEye, rightPlaneDir);
    const double upDot = glm::dot(objToEye, upPlaneDir);
    const double downDot = glm::dot(objToEye, downPlaneDir);
    return leftDot < margin && rightDot < margin && upDot < margin && downDot < margin;
}
