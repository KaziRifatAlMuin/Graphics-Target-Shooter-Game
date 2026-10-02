#pragma once
#include "Camera.h"
#include "Target.h"
#include <optional>
namespace shooter {
class BirdEyeCamera {
public:
    Camera overhead,observation;
    bool observing=false;
    BirdEyeCamera();
    void reset();
    void overview();
    void pan(float forward,float right,float dt);
    void zoom(float steps);
    std::optional<Vec3> groundPoint(float u,float v,float aspect) const;
    bool observeAt(float u,float v,float aspect,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets);
    bool validPosition(Vec3 point,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets) const;
    void moveObservation(float forward,float right,float dt,bool fast,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets);
    std::vector<SceneObject> referenceObjects() const;
    Camera activeCamera() const { return observing?observation:overhead; }
};
}
