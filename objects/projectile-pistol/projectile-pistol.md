# projectile-pistol

Practice startup, simulation time 0; cargo seed 2107042. First encountered instance per assembly type.

All dimensions are meters; all rotations are degrees. Values below use nine significant digits (enough to recover float values). The renderer uses column vectors and column-major matrix storage. The mathematical application order is:

$$p_w=T R_z R_y R_x H S p_l,\qquad p_{clip}=P V p_w.$$

Scale: (sx*x, sy*y, sz*z). Shear: (x+hxy*y+hxz*z, hyx*x+y+hyz*z, hzx*x+hzy*y+z). For a=degrees*pi/180, c=cos(a), s=sin(a): Rx maps to (x, cy-sz, sy+cz); Ry to (cx+sz, y, -sx+cz); Rz to (cx-sy, sx+cy, z). Translation adds (tx,ty,tz). Each stage uses the previous stage's output. No parent transform is implicitly applied by Renderer: builders have already placed every part in world coordinates. Source listings at the end give exact construction, offset, animation and random-value formulas.

Assembly view eye=(-11.6955061, 1.36792123, -8.39550591), center=(-12, 1.16999996, -8.69999981), FOV=45 degrees, aspect=4/3, near=0.00100000005, far=11.7233696.

![Complete assembly](complete.png)

![Opposite view of the same assembly](reverse.png)

## Cube assembly order

Each image adds one real component, preserving builder order and world transforms.

### Add 1: PISTOL

![Assembly 1](assembly-1.png)

## Every component transformation

### PISTOL — `DISPLAY_PROJECTILE_0`

Stationary projectile shape on equipment table; rendered with the same builder/scale as fired projectiles

Position T = (-12, 1.16999996, -8.69999981) m; scale S = (0.100000001, 0.100000001, 0.25); rotation (Rx,Ry,Rz) = (0, -0, 0) degrees.

Shear (xy,xz,yx,yz,zx,zy) = (0, 0, 0, 0, 0, 0). Color = (1, 0.800000012, 0.219999999); specular = 0.800000012; shininess = 24; emission = 0.600000024; flash = 0; material layer = 0; texture scale = 1. Pattern enabled = 0, pattern scale = (1, 1, 1), offset = (0, 0, 0).

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
[ 0.100000001 0 0 0 ]
[ 0 0.100000001 0 0 ]
[ 0 0 0.25 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.5, -0.5, -0.5) | (-0.0500000007, -0.0500000007, -0.125) |
| (-0.5, -0.5, 0.5) | (-0.0500000007, -0.0500000007, 0.125) |
| (-0.5, 0.5, -0.5) | (-0.0500000007, 0.0500000007, -0.125) |
| (-0.5, 0.5, 0.5) | (-0.0500000007, 0.0500000007, 0.125) |
| (0.5, -0.5, -0.5) | (0.0500000007, -0.0500000007, -0.125) |
| (0.5, -0.5, 0.5) | (0.0500000007, -0.0500000007, 0.125) |
| (0.5, 0.5, -0.5) | (0.0500000007, 0.0500000007, -0.125) |
| (0.5, 0.5, 0.5) | (0.0500000007, 0.0500000007, 0.125) |

#### H

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.0500000007, -0.0500000007, -0.125) | (-0.0500000007, -0.0500000007, -0.125) |
| (-0.0500000007, -0.0500000007, 0.125) | (-0.0500000007, -0.0500000007, 0.125) |
| (-0.0500000007, 0.0500000007, -0.125) | (-0.0500000007, 0.0500000007, -0.125) |
| (-0.0500000007, 0.0500000007, 0.125) | (-0.0500000007, 0.0500000007, 0.125) |
| (0.0500000007, -0.0500000007, -0.125) | (0.0500000007, -0.0500000007, -0.125) |
| (0.0500000007, -0.0500000007, 0.125) | (0.0500000007, -0.0500000007, 0.125) |
| (0.0500000007, 0.0500000007, -0.125) | (0.0500000007, 0.0500000007, -0.125) |
| (0.0500000007, 0.0500000007, 0.125) | (0.0500000007, 0.0500000007, 0.125) |

#### Rx

```text
[ 1 0 0 0 ]
[ 0 1 -0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.0500000007, -0.0500000007, -0.125) | (-0.0500000007, -0.0500000007, -0.125) |
| (-0.0500000007, -0.0500000007, 0.125) | (-0.0500000007, -0.0500000007, 0.125) |
| (-0.0500000007, 0.0500000007, -0.125) | (-0.0500000007, 0.0500000007, -0.125) |
| (-0.0500000007, 0.0500000007, 0.125) | (-0.0500000007, 0.0500000007, 0.125) |
| (0.0500000007, -0.0500000007, -0.125) | (0.0500000007, -0.0500000007, -0.125) |
| (0.0500000007, -0.0500000007, 0.125) | (0.0500000007, -0.0500000007, 0.125) |
| (0.0500000007, 0.0500000007, -0.125) | (0.0500000007, 0.0500000007, -0.125) |
| (0.0500000007, 0.0500000007, 0.125) | (0.0500000007, 0.0500000007, 0.125) |

#### Ry

```text
[ 1 0 -0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.0500000007, -0.0500000007, -0.125) | (-0.0500000007, -0.0500000007, -0.125) |
| (-0.0500000007, -0.0500000007, 0.125) | (-0.0500000007, -0.0500000007, 0.125) |
| (-0.0500000007, 0.0500000007, -0.125) | (-0.0500000007, 0.0500000007, -0.125) |
| (-0.0500000007, 0.0500000007, 0.125) | (-0.0500000007, 0.0500000007, 0.125) |
| (0.0500000007, -0.0500000007, -0.125) | (0.0500000007, -0.0500000007, -0.125) |
| (0.0500000007, -0.0500000007, 0.125) | (0.0500000007, -0.0500000007, 0.125) |
| (0.0500000007, 0.0500000007, -0.125) | (0.0500000007, 0.0500000007, -0.125) |
| (0.0500000007, 0.0500000007, 0.125) | (0.0500000007, 0.0500000007, 0.125) |

#### Rz

```text
[ 1 -0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.0500000007, -0.0500000007, -0.125) | (-0.0500000007, -0.0500000007, -0.125) |
| (-0.0500000007, -0.0500000007, 0.125) | (-0.0500000007, -0.0500000007, 0.125) |
| (-0.0500000007, 0.0500000007, -0.125) | (-0.0500000007, 0.0500000007, -0.125) |
| (-0.0500000007, 0.0500000007, 0.125) | (-0.0500000007, 0.0500000007, 0.125) |
| (0.0500000007, -0.0500000007, -0.125) | (0.0500000007, -0.0500000007, -0.125) |
| (0.0500000007, -0.0500000007, 0.125) | (0.0500000007, -0.0500000007, 0.125) |
| (0.0500000007, 0.0500000007, -0.125) | (0.0500000007, 0.0500000007, -0.125) |
| (0.0500000007, 0.0500000007, 0.125) | (0.0500000007, 0.0500000007, 0.125) |

#### T

```text
[ 1 0 0 -12 ]
[ 0 1 0 1.16999996 ]
[ 0 0 1 -8.69999981 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.0500000007, -0.0500000007, -0.125) | (-12.0500002, 1.12, -8.82499981) |
| (-0.0500000007, -0.0500000007, 0.125) | (-12.0500002, 1.12, -8.57499981) |
| (-0.0500000007, 0.0500000007, -0.125) | (-12.0500002, 1.21999991, -8.82499981) |
| (-0.0500000007, 0.0500000007, 0.125) | (-12.0500002, 1.21999991, -8.57499981) |
| (0.0500000007, -0.0500000007, -0.125) | (-11.9499998, 1.12, -8.82499981) |
| (0.0500000007, -0.0500000007, 0.125) | (-11.9499998, 1.12, -8.57499981) |
| (0.0500000007, 0.0500000007, -0.125) | (-11.9499998, 1.21999991, -8.82499981) |
| (0.0500000007, 0.0500000007, 0.125) | (-11.9499998, 1.21999991, -8.57499981) |

Final M = T Rz Ry Rx H S:

```text
[ 0.100000001 0 0 -12 ]
[ 0 0.100000001 0 1.16999996 ]
[ 0 0 0.25 -8.69999981 ]
[ 0 0 0 1 ]
```

## Exact construction implementation

The following files are copied from this build's source, not hand-maintained snippets.

### Exact source: `src/gameplay/Projectile.cpp`

```cpp
#include "gameplay/Projectile.h"
#include <algorithm>
#include "core/Collision.h"

namespace shooter {
void updateProjectiles(std::vector<Projectile>& projectiles, const std::vector<Target>& targets,
                       const std::vector<SceneObject>& obstacles, float dt, const TargetHitCallback& hit,
                       const std::vector<NpcCollider>& npcs, const NpcHitCallback& npcHit) {
    for (auto& p:projectiles) {
        const auto& spec=weaponSpec(p.weapon);
        const float travel=std::min(spec.speed*dt,spec.range-p.travelled);
        float nearest=travel; int hitTarget=-1,hitRing=-1; bool blocked=false;
        for (const auto& o:obstacles) {
            const float d=intersectCube(p.position,p.direction,o.transform,nearest);
            if (d<=nearest) { nearest=d; blocked=true; }
        }
        for (std::size_t i=0;i<targets.size();++i) if (targets[i].respawn<=0 && !targets[i].eliminated) {
            const auto contact=intersectTarget(p.position,p.direction,targets[i],nearest);
            if (contact.distance<nearest) { nearest=contact.distance; hitTarget=int(i); hitRing=contact.ring; blocked=true; }
        }
        const auto npc=intersectNpcs(p.position,p.direction,nearest,npcs);
        if (npc.distance<nearest) { nearest=npc.distance; hitTarget=-1; blocked=true; }
        p.position=p.position+p.direction*nearest;
        p.travelled+=nearest;
        if (hitTarget>=0 && hitRing>=0) hit(std::size_t(hitTarget),hitRing,p.shotId);
        if (npc.collider>=0 && npc.distance<=nearest && npcHit) {
            const auto& collider=npcs[npc.collider]; npcHit(collider.human,collider.index,p.shotId);
        }
        if (blocked) p.travelled=spec.range;
    }
    projectiles.erase(std::remove_if(projectiles.begin(),projectiles.end(),[](const Projectile& p) {
        return p.travelled>=weaponSpec(p.weapon).range-.0001f;
    }),projectiles.end());
}
SceneObject projectileObject(const Projectile& p) {
    auto o=makeCube("PROJECTILE_"+std::to_string(p.id),"Projectile",weaponSpec(p.weapon).name,
                p.position,weaponSpec(p.weapon).projectileScale,{1,.8f,.22f});
    o.transform.rotation={std::asin(std::clamp(p.direction.y,-1.0f,1.0f))*180/pi,
                          std::atan2(-p.direction.x,-p.direction.z)*180/pi,0};
    o.emission=.6f; o.specular=.8f;
    return o;
}
}

```

### Exact source: `src/gameplay/Weapon.cpp`

```cpp
#include "gameplay/Weapon.h"

namespace shooter {
const WeaponSpec& weaponSpec(WeaponType type) {
    static const WeaponSpec specs[]={
        {"PISTOL",25,42,.28f,0,1,{.10f,.10f,.25f}},
        {"SHOTGUN",18,36,.8f,.075f,9,{.08f,.08f,.18f}},
        {"ASSAULT RIFLE",70,65,.11f,.009f,1,{.13f,.13f,.30f}}
    };
    return specs[static_cast<int>(type)];
}
namespace {
Mat4 orientation(const Camera& player) {
    return makeRotationY(-player.yaw-90)*makeRotationX(player.pitch);
}
Vec3 worldOffset(const Camera& player, Vec3 offset) {
    const auto p=transformPoint(orientation(player),{offset.x,offset.y,offset.z,0});
    return player.position+Vec3{p.x,p.y,p.z};
}
}
Vec3 weaponMuzzle(WeaponType type, const Camera& player) {
    return worldOffset(player,{.32f,-.22f,type==WeaponType::Pistol?-1.02f:-1.52f});
}
std::vector<SceneObject> createWeapon(WeaponType type, const Camera& player, float recoil) {
    std::vector<SceneObject> result;
    const std::string prefix=type==WeaponType::Pistol?"PISTOL":type==WeaponType::Shotgun?"SHOTGUN":"RIFLE";
    const Vec3 metal{.22f,.27f,.31f}, grip{.30f,.21f,.14f}, accent{.91f,.64f,.23f};
    auto part=[&](const char* name, Vec3 offset, Vec3 size, Vec3 color, float shear=0) {
        Transform t;
        offset.z+=recoil*.20f;
        t.position=worldOffset(player,offset);
        t.rotation={player.pitch,-player.yaw-90,0}; t.scale=size; t.shear[3]=shear;
        result.push_back({prefix+"_"+name,"Weapon",name,t,color,
            "Game model; "+std::to_string(int(weaponSpec(type).range))+" m range; follows player aim"});
        result.back().parent=prefix;
        result.back().specular=.75f; result.back().shininess=80;
        if (std::string(name)=="MUZZLE_FLASH") result.back().emission=1;
    };
    if (type==WeaponType::Pistol) {
        part("BODY",{.32f,-.25f,-.68f},{.22f,.22f,.56f},metal);
        part("GRIP",{.32f,-.46f,-.50f},{.18f,.37f,.20f},grip,-.3f);
        part("BARREL",{.32f,-.22f,-.96f},{.13f,.12f,.16f},accent);
        part("SIGHT",{.32f,-.10f,-.89f},{.045f,.07f,.07f},accent);
    } else {
        const bool shotgun=type==WeaponType::Shotgun;
        part("BODY",{.32f,-.28f,-.79f},{shotgun?.29f:.23f,.25f,.70f},metal);
        part("BARREL",{.32f,-.22f,-1.31f},{shotgun?.21f:.11f,.13f,.43f},metal);
        part("STOCK",{.32f,-.36f,-.28f},{.24f,.30f,.45f},grip);
        part("GRIP",{.32f,-.50f,-.58f},{.17f,.27f,.20f},grip,-.4f);
        part("FORE_END",{.32f,-.35f,-1.05f},{.28f,.13f,.34f},shotgun?grip:accent);
        if (!shotgun) part("MAGAZINE",{.32f,-.51f,-.83f},{.15f,.35f,.24f},metal,-.22f);
        part("FRONT_SIGHT",{.32f,-.10f,-1.34f},{.05f,.13f,.07f},accent);
        part("REAR_SIGHT",{.32f,-.10f,-.59f},{.13f,.10f,.06f},metal);
    }
    const bool pistol=type==WeaponType::Pistol;
    part("MUZZLE_INSET",{.32f,-.22f,pistol?-1.043f:-1.531f},{pistol?.085f:.09f,.075f,.012f},{.045f,.055f,.065f});
    part("TRIGGER_GUARD",{.32f,-.48f,pistol?-.68f:-.76f},{.13f,.035f,.19f},metal);
    part("GUARD_FRONT",{.32f,-.40f,pistol?-.76f:-.84f},{.13f,.16f,.035f},metal);
    for (int i=0;i<4;++i) {
        const std::string name="DETAIL_"+std::to_string(i);
        if(pistol) part(name.c_str(),{.433f,-.23f,-.49f-i*.045f},{.015f,.13f,.013f},{.10f,.13f,.16f});
        else part(name.c_str(),{.32f,-.34f,-.95f-i*.058f},{.30f,.15f,.018f},type==WeaponType::Shotgun?Vec3{.16f,.11f,.075f}:Vec3{.10f,.14f,.16f});
    }
    if (recoil>.65f) part("MUZZLE_FLASH",{.32f,-.22f,type==WeaponType::Pistol?-1.1f:-1.62f},
                          {.18f,.18f,.23f},{1,.83f,.30f});
    return result;
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
