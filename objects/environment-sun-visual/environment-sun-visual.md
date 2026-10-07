# environment-sun-visual

Practice startup, simulation time 0; cargo seed 2107042. First encountered instance per assembly type.

All dimensions are meters; all rotations are degrees. Values below use nine significant digits (enough to recover float values). The renderer uses column vectors and column-major matrix storage. The mathematical application order is:

$$p_w=T R_z R_y R_x H S p_l,\qquad p_{clip}=P V p_w.$$

Scale: (sx*x, sy*y, sz*z). Shear: (x+hxy*y+hxz*z, hyx*x+y+hyz*z, hzx*x+hzy*y+z). For a=degrees*pi/180, c=cos(a), s=sin(a): Rx maps to (x, cy-sz, sy+cz); Ry to (cx+sz, y, -sx+cz); Rz to (cx-sy, sx+cy, z). Translation adds (tx,ty,tz). Each stage uses the previous stage's output. No parent transform is implicitly applied by Renderer: builders have already placed every part in world coordinates. Source listings at the end give exact construction, offset, animation and random-value formulas.

Assembly view eye=(35.265934, 41.6728592, -69.7340622), center=(25, 35, -80), FOV=45 degrees, aspect=4/3, near=0.0242095347, far=68.1028824.

![Complete assembly](complete.png)

![Opposite view of the same assembly](reverse.png)

## Cube assembly order

Each image adds one real component, preserving builder order and world transforms.

### Add 1: Sun visual

![Assembly 1](assembly-1.png)

## Every component transformation

### Sun visual — `SUN_VIS`

Emissive sun; daytime directional source

Position T = (25, 35, -80) m; scale S = (4, 4, 4); rotation (Rx,Ry,Rz) = (0, 25, 15) degrees.

Shear (xy,xz,yx,yz,zx,zy) = (0, 0, 0, 0, 0, 0). Color = (1, 0.819999993, 0.379999995); specular = 0.119999997; shininess = 24; emission = 1; flash = 0; material layer = 5; texture scale = 3. Pattern enabled = 0, pattern scale = (1, 1, 1), offset = (0, 0, 0).

| Step | Actual renderer image |
| --- | --- |
| Unit cube | ![Unit cube](part-0-0.png) |
| Scale | ![Scale](part-0-1.png) |
| Shear | ![Shear](part-0-2.png) |
| Rotate X | ![Rotate X](part-0-3.png) |
| Rotate Y | ![Rotate Y](part-0-4.png) |
| Rotate Z | ![Rotate Z](part-0-5.png) |
| Translate to world | ![Translate to world](part-0-6.png) |

Steps 1–5 share one camera; the unit cube has its own close view, and step 6 is reframed at the actual world location to keep small objects visible. Reframing changes the view, never the model coordinates. Identity steps deliberately remain visible. Intermediate shapes are instructional states, while step 6 uses the unmodified game object.

Each stage below gives its exact numeric matrix and all eight corner multiplications. For each output row r: p'[r] = A[r,0]p.x + A[r,1]p.y + A[r,2]p.z + A[r,3]p.w.

![Signed translation components](part-0-translation.svg)


#### S

```text
[ 4 0 0 0 ]
[ 0 4 0 0 ]
[ 0 0 4 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.5, -0.5, -0.5) | (-2, -2, -2) |
| (-0.5, -0.5, 0.5) | (-2, -2, 2) |
| (-0.5, 0.5, -0.5) | (-2, 2, -2) |
| (-0.5, 0.5, 0.5) | (-2, 2, 2) |
| (0.5, -0.5, -0.5) | (2, -2, -2) |
| (0.5, -0.5, 0.5) | (2, -2, 2) |
| (0.5, 0.5, -0.5) | (2, 2, -2) |
| (0.5, 0.5, 0.5) | (2, 2, 2) |

#### H

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-2, -2, -2) | (-2, -2, -2) |
| (-2, -2, 2) | (-2, -2, 2) |
| (-2, 2, -2) | (-2, 2, -2) |
| (-2, 2, 2) | (-2, 2, 2) |
| (2, -2, -2) | (2, -2, -2) |
| (2, -2, 2) | (2, -2, 2) |
| (2, 2, -2) | (2, 2, -2) |
| (2, 2, 2) | (2, 2, 2) |

#### Rx

```text
[ 1 0 0 0 ]
[ 0 1 -0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-2, -2, -2) | (-2, -2, -2) |
| (-2, -2, 2) | (-2, -2, 2) |
| (-2, 2, -2) | (-2, 2, -2) |
| (-2, 2, 2) | (-2, 2, 2) |
| (2, -2, -2) | (2, -2, -2) |
| (2, -2, 2) | (2, -2, 2) |
| (2, 2, -2) | (2, 2, -2) |
| (2, 2, 2) | (2, 2, 2) |

#### Ry

```text
[ 0.906307757 0 0.42261827 0 ]
[ 0 1 0 0 ]
[ -0.42261827 0 0.906307757 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-2, -2, -2) | (-2.65785217, -2, -0.967378974) |
| (-2, -2, 2) | (-0.967378974, -2, 2.65785217) |
| (-2, 2, -2) | (-2.65785217, 2, -0.967378974) |
| (-2, 2, 2) | (-0.967378974, 2, 2.65785217) |
| (2, -2, -2) | (0.967378974, -2, -2.65785217) |
| (2, -2, 2) | (2.65785217, -2, 0.967378974) |
| (2, 2, -2) | (0.967378974, 2, -2.65785217) |
| (2, 2, 2) | (2.65785217, 2, 0.967378974) |

#### Rz

```text
[ 0.965925813 -0.258819044 0 0 ]
[ 0.258819044 0.965925813 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-2.65785217, -2, -0.967378974) | (-2.04964972, -2.61975431, -0.967378974) |
| (-0.967378974, -2, 2.65785217) | (-0.416778207, -2.18222761, 2.65785217) |
| (-2.65785217, 2, -0.967378974) | (-3.08492613, 1.24394894, -0.967378974) |
| (-0.967378974, 2, 2.65785217) | (-1.45205438, 1.68147552, 2.65785217) |
| (0.967378974, -2, -2.65785217) | (1.45205438, -1.68147552, -2.65785217) |
| (2.65785217, -2, 0.967378974) | (3.08492613, -1.24394894, 0.967378974) |
| (0.967378974, 2, -2.65785217) | (0.416778207, 2.18222761, -2.65785217) |
| (2.65785217, 2, 0.967378974) | (2.04964972, 2.61975431, 0.967378974) |

#### T

```text
[ 1 0 0 25 ]
[ 0 1 0 35 ]
[ 0 0 1 -80 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-2.04964972, -2.61975431, -0.967378974) | (22.9503498, 32.3802452, -80.9673767) |
| (-0.416778207, -2.18222761, 2.65785217) | (24.5832214, 32.8177719, -77.3421478) |
| (-3.08492613, 1.24394894, -0.967378974) | (21.9150734, 36.2439499, -80.9673767) |
| (-1.45205438, 1.68147552, 2.65785217) | (23.547945, 36.6814766, -77.3421478) |
| (1.45205438, -1.68147552, -2.65785217) | (26.452055, 33.3185234, -82.6578522) |
| (3.08492613, -1.24394894, 0.967378974) | (28.0849266, 33.7560501, -79.0326233) |
| (0.416778207, 2.18222761, -2.65785217) | (25.4167786, 37.1822281, -82.6578522) |
| (2.04964972, 2.61975431, 0.967378974) | (27.0496502, 37.6197548, -79.0326233) |

Final M = T Rz Ry Rx H S:

```text
[ 3.50170422 -1.03527617 1.63287163 25 ]
[ 0.938278854 3.86370325 0.437526613 35 ]
[ -1.69047308 0 3.62523103 -80 ]
[ 0 0 0 1 ]
```

## Exact construction implementation

The following files are copied from this build's source, not hand-maintained snippets.

### Exact source: `src/world/Environment.cpp`

```cpp
#include "world/Environment.h"
#include "gameplay/Weapon.h"
#include "gameplay/Projectile.h"

namespace shooter {
std::vector<SceneObject> createEquipmentDisplay() {
    std::vector<SceneObject> result;
    result.push_back(makeCube("EQUIPMENT_TABLE","Environment","Equipment table",{-9,.55f,-8},
        {12,1.1f,3},{.28f,.32f,.34f}));
    result.push_back(makeCube("EQUIPMENT_SURFACE","Environment","Equipment mat",{-9,1.11f,-8},
        {11.8f,.02f,2.8f},{.16f,.21f,.23f}));
    for (int slot=0;slot<3;++slot) {
        auto label=makeCube("EQUIPMENT_LABEL_"+std::to_string(slot),"Environment","Display identification plate",
            {-13.f+slot*4,1.13f,-6.8f},{1.1f,.025f,.22f},{.82f,.65f,.28f});
        label.parent="EQUIPMENT_TABLE"; result.push_back(label);
    }
    int index=0;
    for (auto type:{WeaponType::Pistol,WeaponType::Shotgun,WeaponType::Rifle}) {
        Camera pose; pose.position={-13.0f+index*4,1.65f,-8}; pose.yaw=-90; pose.pitch=0;
        auto parts=createWeapon(type,pose);
        for (auto& part:parts) {
            part.id="DISPLAY_"+part.id;
            part.notes="Stationary equipment exhibit near spawn; actual rendered weapon assembled by createWeapon";
            result.push_back(part);
        }
        Projectile shape{0,type,pose.position+Vec3{1,-.48f,-.7f},{0,0,-1},0,0};
        auto visual=projectileObject(shape);
        visual.id="DISPLAY_PROJECTILE_"+std::to_string(index++);
        visual.notes="Stationary projectile shape on equipment table; rendered with the same builder/scale as fired projectiles";
        result.push_back(visual);
    }
    return result;
}
std::vector<SceneObject> createCelestialObjects(bool night) {
    if (night) return {}; // Night has no celestial light or visible moon.
    Transform t; t.position={25,35,-80}; t.scale={4,4,4}; t.rotation={0,25,15};
    SceneObject sun{"SUN_VIS","Environment","Sun visual",t,{1,.82f,.38f},"Emissive sun; daytime directional source"};
    sun.emission=1; sun.parent="SUN";
    return {sun};
}
}

```

### Exact source: `src/core/Transform.h`

```cpp
#pragma once

#include <array>
#include <cmath>

namespace shooter {
constexpr float pi = 3.14159265358979323846f;
inline float radians(float degrees) { return degrees * pi / 180.0f; }

struct Vec3 { float x = 0, y = 0, z = 0; };
struct Vec4 { float x, y, z, w; };
inline Vec3 operator+(Vec3 a, Vec3 b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
inline Vec3 operator-(Vec3 a, Vec3 b) { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
inline Vec3 operator*(Vec3 a, float s) { return {a.x*s, a.y*s, a.z*s}; }
inline float dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
inline Vec3 cross(Vec3 a, Vec3 b) {
    return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x};
}
inline Vec3 normalize(Vec3 v) {
    const float length = std::sqrt(dot(v, v));
    return length > 0 ? v * (1.0f / length) : Vec3{};
}

// Column-major storage, column vectors: upload directly with GL_FALSE.
struct Mat4 {
    std::array<float, 16> data{};
    float& at(int row, int col) { return data[col*4+row]; }
    float at(int row, int col) const { return data[col*4+row]; }
    static Mat4 identity() {
        Mat4 result;
        for (int i = 0; i < 4; ++i) result.at(i, i) = 1;
        return result;
    }
};
inline Mat4 operator*(const Mat4& a, const Mat4& b) {
    Mat4 result;
    for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 4; ++col)
            for (int k = 0; k < 4; ++k)
                result.at(row, col) += a.at(row, k) * b.at(k, col);
    return result;
}
inline Vec4 transformPoint(const Mat4& m, const Vec4& p) {
    return {
        m.at(0,0)*p.x + m.at(0,1)*p.y + m.at(0,2)*p.z + m.at(0,3)*p.w,
        m.at(1,0)*p.x + m.at(1,1)*p.y + m.at(1,2)*p.z + m.at(1,3)*p.w,
        m.at(2,0)*p.x + m.at(2,1)*p.y + m.at(2,2)*p.z + m.at(2,3)*p.w,
        m.at(3,0)*p.x + m.at(3,1)*p.y + m.at(3,2)*p.z + m.at(3,3)*p.w
    };
}
inline Mat4 makeTranslation(float x, float y, float z) {
    Mat4 m = Mat4::identity();
    m.at(0,3)=x; m.at(1,3)=y; m.at(2,3)=z;
    return m;
}
inline Mat4 makeScale(float x, float y, float z) {
    Mat4 m = Mat4::identity();
    m.at(0,0)=x; m.at(1,1)=y; m.at(2,2)=z;
    return m;
}
inline Mat4 makeRotationX(float degrees) {
    Mat4 m = Mat4::identity();
    const float c=std::cos(radians(degrees)), s=std::sin(radians(degrees));
    m.at(1,1)=c; m.at(1,2)=-s; m.at(2,1)=s; m.at(2,2)=c;
    return m;
}
inline Mat4 makeRotationY(float degrees) {
    Mat4 m = Mat4::identity();
    const float c=std::cos(radians(degrees)), s=std::sin(radians(degrees));
    m.at(0,0)=c; m.at(0,2)=s; m.at(2,0)=-s; m.at(2,2)=c;
    return m;
}
inline Mat4 makeRotationZ(float degrees) {
    Mat4 m = Mat4::identity();
    const float c=std::cos(radians(degrees)), s=std::sin(radians(degrees));
    m.at(0,0)=c; m.at(0,1)=-s; m.at(1,0)=s; m.at(1,1)=c;
    return m;
}
inline Mat4 makeShear(float xy, float xz, float yx, float yz, float zx, float zy) {
    Mat4 m = Mat4::identity();
    m.at(0,1)=xy; m.at(0,2)=xz; m.at(1,0)=yx;
    m.at(1,2)=yz; m.at(2,0)=zx; m.at(2,1)=zy;
    return m;
}
struct Transform {
    Vec3 position{};
    Vec3 rotation{}; // Degrees about X, Y, Z.
    Vec3 scale{1,1,1};
    std::array<float, 6> shear{}; // xy, xz, yx, yz, zx, zy.
};
inline constexpr const char* modelMatrixOrder = "T*Rz*Ry*Rx*H*S";
struct TransformStage {
    const char* name;
    Mat4 matrix;
};
inline std::array<TransformStage, 6> modelTransformStages(const Transform& t) {
    const auto& h = t.shear;
    // Application order; identity operations remain explicit for every object.
    return {{{"S", makeScale(t.scale.x,t.scale.y,t.scale.z)},
             {"H", makeShear(h[0],h[1],h[2],h[3],h[4],h[5])},
             {"Rx", makeRotationX(t.rotation.x)},
             {"Ry", makeRotationY(t.rotation.y)},
             {"Rz", makeRotationZ(t.rotation.z)},
             {"T", makeTranslation(t.position.x,t.position.y,t.position.z)}}};
}
inline Mat4 composeModelMatrix(const Transform& t) {
    Mat4 model = Mat4::identity();
    // Left-multiply each stage: the resulting matrix is T * Rz * Ry * Rx * H * S.
    for (const auto& stage : modelTransformStages(t)) model = stage.matrix * model;
    return model;
}
inline std::array<Vec4, 7> traceTransformPoint(const Transform& t, Vec4 localPoint) {
    std::array<Vec4, 7> points{};
    points[0] = localPoint;
    const auto stages = modelTransformStages(t);
    for (std::size_t i = 0; i < stages.size(); ++i)
        points[i+1] = transformPoint(stages[i].matrix, points[i]);
    return points; // local, scaled, sheared, rotated X/Y/Z, world.
}
inline Mat4 makePerspective(float fovDegrees, float aspect, float nearPlane, float farPlane) {
    Mat4 m;
    const float f = 1.0f / std::tan(radians(fovDegrees) / 2);
    m.at(0,0)=f/aspect; m.at(1,1)=f;
    m.at(2,2)=(farPlane+nearPlane)/(nearPlane-farPlane);
    m.at(2,3)=2*farPlane*nearPlane/(nearPlane-farPlane);
    m.at(3,2)=-1;
    return m;
}
inline Mat4 makeLookAt(Vec3 eye, Vec3 center, Vec3 up) {
    const Vec3 f=normalize(center-eye), s=normalize(cross(f,up)), u=cross(s,f);
    Mat4 m = Mat4::identity();
    m.at(0,0)=s.x; m.at(0,1)=s.y; m.at(0,2)=s.z; m.at(0,3)=-dot(s,eye);
    m.at(1,0)=u.x; m.at(1,1)=u.y; m.at(1,2)=u.z; m.at(1,3)=-dot(u,eye);
    m.at(2,0)=-f.x; m.at(2,1)=-f.y; m.at(2,2)=-f.z; m.at(2,3)=dot(f,eye);
    return m;
}
} // namespace shooter

```
