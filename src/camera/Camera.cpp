#include "camera/Camera.h"
#include <algorithm>

namespace shooter {
Vec3 Camera::forward() const {
    return normalize({std::cos(radians(yaw))*std::cos(radians(pitch)),
                      std::sin(radians(pitch)),
                      std::sin(radians(yaw))*std::cos(radians(pitch))});
}
Mat4 Camera::getViewMatrix() const {
    return makeLookAt(position, position + forward(), {0,1,0});
}
void Camera::look(float dx, float dy) {
    yaw += dx * 0.12f;
    pitch = std::clamp(pitch - dy * 0.12f, -89.0f, 89.0f);
}
void Camera::move(float f, float r, float u, float dt, bool fast) {
    const Vec3 direction = forward()*f + normalize(cross(forward(),{0,1,0}))*r + Vec3{0,u,0};
    position = position + normalize(direction) * ((fast ? 30.0f : 12.0f) * dt);
    position.y = std::max(position.y, 0.3f);
}
}
