#include "camera/Camera.h"
#include <algorithm>

namespace shooter {
// Yaw/pitch give direction=(cos(yaw)*cos(pitch), sin(pitch), sin(yaw)*cos(pitch)).
Vec3 Camera::forward() const {
    return normalize({std::cos(radians(yaw))*std::cos(radians(pitch)),
                      std::sin(radians(pitch)),
                      std::sin(radians(yaw))*std::cos(radians(pitch))});
}
// Move the world into camera coordinates, looking from position toward position+forward.
Mat4 Camera::getViewMatrix() const {
    return makeLookAt(position, position + forward(), {0,1,0});
}
// Mouse movement changes yaw/pitch by 0.12 degrees per unit; clamp pitch to avoid flipping at the poles.
void Camera::look(float dx, float dy) {
    yaw += dx * 0.12f;
    pitch = std::clamp(pitch - dy * 0.12f, -89.0f, 89.0f);
}
// Move by unit(inputDirection)*speed*dt, so diagonal movement is no faster than straight movement.
void Camera::move(float f, float r, float u, float dt, bool fast) {
    const Vec3 direction = forward()*f + normalize(cross(forward(),{0,1,0}))*r + Vec3{0,u,0};
    position = position + normalize(direction) * ((fast ? 30.0f : 12.0f) * dt);
    position.y = std::max(position.y, 0.3f);
}
}
