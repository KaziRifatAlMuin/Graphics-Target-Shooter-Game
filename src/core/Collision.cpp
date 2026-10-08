#include "core/Collision.h"
#include <algorithm>
#include <limits>

namespace shooter {
namespace {
constexpr float infinity=std::numeric_limits<float>::infinity();
// Bound a transformed cube generously: radius <= 0.5*(|sx|+|sy|+|sz|)*(1+sum|shear|).
float conservativeRadius(const Transform& box) {
    float shearBound=1;
    for (float h:box.shear) shearBound+=std::abs(h);
    return .5f*(std::abs(box.scale.x)+std::abs(box.scale.y)+std::abs(box.scale.z))*shearBound;
}
}
// Find the first ray hit, p(t)=origin+t*direction, up to maximum; infinity means no contact.
float intersectCube(Vec3 origin, Vec3 direction, const Transform& box, float maximum) {
    const float radius=conservativeRadius(box);
    const auto offset=origin-box.position;
    // Reject boxes outside the ray's possible reach before doing the more expensive exact test.
    const float reach=radius+maximum*length(direction);
    if (dot(offset,offset)>reach*reach) return infinity;
    const Mat4 m=composeModelMatrix(box);
    // Invert the affine 3x3 basis. This also handles the wall supports' shears.
    const Vec3 a{m.at(0,0),m.at(1,0),m.at(2,0)}, b{m.at(0,1),m.at(1,1),m.at(2,1)},
               c{m.at(0,2),m.at(1,2),m.at(2,2)};
    // det=a.(b x c); a nearly zero determinant means this transform cannot be reliably inverted.
    const float determinant=dot(a,cross(b,c));
    if (std::abs(determinant)<1e-8f) return infinity;
    auto inverse=[&](Vec3 v) { return Vec3{dot(cross(b,c),v)/determinant,
        dot(cross(c,a),v)/determinant,dot(cross(a,b),v)/determinant}; };
    // Undo the box transform on the ray so the test uses a simple cube spanning -0.5 to +0.5.
    const Vec3 p=inverse(origin-box.position), d=inverse(direction);
    const float origins[]={p.x,p.y,p.z}, directions[]={d.x,d.y,d.z};
    // Intersect the three axis intervals: enter=max(entry times), leave=min(exit times); enter>leave misses.
    float enter=0, leave=maximum;
    for (int axis=0; axis<3; ++axis) {
        if (std::abs(directions[axis])<1e-7f) {
            if (origins[axis]<-.5f || origins[axis]>.5f) return infinity;
        } else {
            // Solve each face crossing with t=(facePosition-rayOrigin)/rayDirection.
            float first=(-.5f-origins[axis])/directions[axis], last=(.5f-origins[axis])/directions[axis];
            if (first>last) std::swap(first,last);
            enter=std::max(enter,first); leave=std::min(leave,last);
            if (enter>leave) return infinity;
        }
    }
    return enter;
}
// Reject positions outside the arena or inside an obstacle's padded horizontal footprint.
bool canStandAt(Vec3 p, const std::vector<SceneObject>& staticObjects) {
    if (p.x<-26 || p.x>26 || p.z<-96 || p.z>-4) return false;
    // Conservative footprints keep the walking player outside rotated crates/supports.
    for (const auto& o:staticObjects) {
        if (o.type!="Cargo" && o.component!="Sheared stone support" && o.type!="Lighting" && o.component!="Equipment table") continue;
        // Fast reject dense cargo before computing a transformed footprint.
        const float radius=conservativeRadius(o.transform)+.45f;
        if (std::abs(p.x-o.transform.position.x)>radius || std::abs(p.z-o.transform.position.z)>radius) continue;
        const Mat4 m=composeModelMatrix(o.transform);
        // Transformed half-extent on axis i = 0.5*sum_j |M[i,j]|; add 0.45 m for player clearance.
        const float extentX=.5f*(std::abs(m.at(0,0))+std::abs(m.at(0,1))+std::abs(m.at(0,2)))+.45f;
        const float extentZ=.5f*(std::abs(m.at(2,0))+std::abs(m.at(2,1))+std::abs(m.at(2,2)))+.45f;
        if (std::abs(p.x-o.transform.position.x)<extentX && std::abs(p.z-o.transform.position.z)<extentZ)
            return false;
    }
    return true;
}
}
