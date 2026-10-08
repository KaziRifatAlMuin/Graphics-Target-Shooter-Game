#pragma once
#include "camera/Camera.h"
#include "gameplay/Target.h"
#include <optional>
namespace shooter {
// Keep overhead navigation and ground-level observation as two separate camera poses.
class BirdEyeCamera {
public:
    Camera overhead,observation;
    bool observing=false;
    // Create both overhead and ground observation cameras in their initial positions.
    BirdEyeCamera();
    // Place the overhead camera almost straight down; a small tilt avoids parallel forward/up vectors.
    void reset();
    // Switch back to the overhead camera without changing its position.
    void overview();
    // Move the overhead view horizontally by speed*dt and keep it inside the arena limits.
    void pan(float forward,float right,float dt);
    // Change camera height by 8 meters per wheel step, clamped to 65-180 meters.
    void zoom(float steps);
    // Convert normalized screen coordinates (u,v) into a world ray and intersect it with ground Y=0.
    std::optional<Vec3> groundPoint(float u,float v,float aspect) const;
    // Place the observer 1.7 m above clear clicked ground, keeping overhead yaw but looking horizontally.
    bool observeAt(float u,float v,float aspect,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets);
    // Accept only ground positions clear of solid obstacles and target stands.
    bool validPosition(Vec3 point,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets) const;
    // Move the observation camera in small collision-checked horizontal steps.
    void moveObservation(float forward,float right,float dt,bool fast,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets);
    // Build a visible marker showing the ground observation camera's location.
    std::vector<SceneObject> referenceObjects() const;
    Camera activeCamera() const { return observing?observation:overhead; }
};
}
