#pragma once
#include "core/Transform.h"

namespace shooter {
// Camera position is in world meters; yaw turns left/right and pitch looks up/down in degrees.
class Camera {
public:
    Vec3 position{22, 24, 18};
    float yaw = -108, pitch = -24;
    // Yaw/pitch give direction=(cos(yaw)*cos(pitch), sin(pitch), sin(yaw)*cos(pitch)).
    Vec3 forward() const;
    // Move the world into camera coordinates, looking from position toward position+forward.
    Mat4 getViewMatrix() const;
    // Mouse movement changes yaw/pitch by 0.12 degrees per unit; clamp pitch to avoid flipping at the poles.
    void look(float dx, float dy);
    // Move by unit(inputDirection)*speed*dt, so diagonal movement is no faster than straight movement.
    void move(float forwardInput, float rightInput, float upInput, float deltaTime, bool fast);
};
}
