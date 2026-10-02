#pragma once
#include "core/Transform.h"

namespace shooter {
class Camera {
public:
    Vec3 position{22, 24, 18};
    float yaw = -108, pitch = -24;
    Vec3 forward() const;
    Mat4 getViewMatrix() const;
    void look(float dx, float dy);
    void move(float forwardInput, float rightInput, float upInput, float deltaTime, bool fast);
};
}
