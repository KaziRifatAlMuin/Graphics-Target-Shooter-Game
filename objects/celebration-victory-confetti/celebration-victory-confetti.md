# celebration-victory-confetti

celebrationObjects(time=1 second, victory=true), particle index 0.

All dimensions are meters; all rotations are degrees. Values below use nine significant digits (enough to recover float values). The renderer uses column vectors and column-major matrix storage. The mathematical application order is:

$$p_w=T R_z R_y R_x H S p_l,\qquad p_{clip}=P V p_w.$$

Scale: (sx*x, sy*y, sz*z). Shear: (x+hxy*y+hxz*z, hyx*x+y+hyz*z, hzx*x+hzy*y+z). For a=degrees*pi/180, c=cos(a), s=sin(a): Rx maps to (x, cy-sz, sy+cz); Ry to (cx+sz, y, -sx+cz); Rz to (cx-sy, sx+cy, z). Translation adds (tx,ty,tz). Each stage uses the previous stage's output. No parent transform is implicitly applied by Renderer: builders have already placed every part in world coordinates. Source listings at the end give exact construction, offset, animation and random-value formulas.

Assembly view eye=(5.64615536, 24.9200001, -25.3538456), center=(5, 24.5, -26), FOV=45 degrees, aspect=4/3, near=0.00152378879, far=13.657093.

![Complete assembly](complete.png)

![Opposite view of the same assembly](reverse.png)

## Cube assembly order

Each image adds one real component, preserving builder order and world transforms.

### Add 1: Victory confetti

![Assembly 1](assembly-1.png)

## Every component transformation

### Victory confetti — `CELEBRATION_0`

Cosmetic transformed cube; ballistic celebration time=1.000000 s; no scoring or collision

Position T = (5, 24.5, -26) m; scale S = (0.239999995, 0.140000001, 0.319999993); rotation (Rx,Ry,Rz) = (90, 120, 150) degrees.

Shear (xy,xz,yx,yz,zx,zy) = (0, 0, 0, 0, 0, 0). Color = (0.119999997, 0.850000024, 0.680000007); specular = 0.119999997; shininess = 24; emission = 0.649999976; flash = 0; material layer = 0; texture scale = 1. Pattern enabled = 0, pattern scale = (1, 1, 1), offset = (0, 0, 0).

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
[ 0.239999995 0 0 0 ]
[ 0 0.140000001 0 0 ]
[ 0 0 0.319999993 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.5, -0.5, -0.5) | (-0.119999997, -0.0700000003, -0.159999996) |
| (-0.5, -0.5, 0.5) | (-0.119999997, -0.0700000003, 0.159999996) |
| (-0.5, 0.5, -0.5) | (-0.119999997, 0.0700000003, -0.159999996) |
| (-0.5, 0.5, 0.5) | (-0.119999997, 0.0700000003, 0.159999996) |
| (0.5, -0.5, -0.5) | (0.119999997, -0.0700000003, -0.159999996) |
| (0.5, -0.5, 0.5) | (0.119999997, -0.0700000003, 0.159999996) |
| (0.5, 0.5, -0.5) | (0.119999997, 0.0700000003, -0.159999996) |
| (0.5, 0.5, 0.5) | (0.119999997, 0.0700000003, 0.159999996) |

#### H

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.119999997, -0.0700000003, -0.159999996) | (-0.119999997, -0.0700000003, -0.159999996) |
| (-0.119999997, -0.0700000003, 0.159999996) | (-0.119999997, -0.0700000003, 0.159999996) |
| (-0.119999997, 0.0700000003, -0.159999996) | (-0.119999997, 0.0700000003, -0.159999996) |
| (-0.119999997, 0.0700000003, 0.159999996) | (-0.119999997, 0.0700000003, 0.159999996) |
| (0.119999997, -0.0700000003, -0.159999996) | (0.119999997, -0.0700000003, -0.159999996) |
| (0.119999997, -0.0700000003, 0.159999996) | (0.119999997, -0.0700000003, 0.159999996) |
| (0.119999997, 0.0700000003, -0.159999996) | (0.119999997, 0.0700000003, -0.159999996) |
| (0.119999997, 0.0700000003, 0.159999996) | (0.119999997, 0.0700000003, 0.159999996) |

#### Rx

```text
[ 1 0 0 0 ]
[ 0 -4.37113883e-08 -1 0 ]
[ 0 1 -4.37113883e-08 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.119999997, -0.0700000003, -0.159999996) | (-0.119999997, 0.159999996, -0.0699999928) |
| (-0.119999997, -0.0700000003, 0.159999996) | (-0.119999997, -0.159999996, -0.0700000077) |
| (-0.119999997, 0.0700000003, -0.159999996) | (-0.119999997, 0.159999996, 0.0700000077) |
| (-0.119999997, 0.0700000003, 0.159999996) | (-0.119999997, -0.159999996, 0.0699999928) |
| (0.119999997, -0.0700000003, -0.159999996) | (0.119999997, 0.159999996, -0.0699999928) |
| (0.119999997, -0.0700000003, 0.159999996) | (0.119999997, -0.159999996, -0.0700000077) |
| (0.119999997, 0.0700000003, -0.159999996) | (0.119999997, 0.159999996, 0.0700000077) |
| (0.119999997, 0.0700000003, 0.159999996) | (0.119999997, -0.159999996, 0.0699999928) |

#### Ry

```text
[ -0.50000006 0 0.866025388 0 ]
[ 0 1 0 0 ]
[ -0.866025388 0 -0.50000006 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.119999997, 0.159999996, -0.0699999928) | (-0.000621765852, 0.159999996, 0.138923049) |
| (-0.119999997, -0.159999996, -0.0700000077) | (-0.000621777028, -0.159999996, 0.138923049) |
| (-0.119999997, 0.159999996, 0.0700000077) | (0.120621786, 0.159999996, 0.0689230412) |
| (-0.119999997, -0.159999996, 0.0699999928) | (0.120621778, -0.159999996, 0.0689230412) |
| (0.119999997, 0.159999996, -0.0699999928) | (-0.120621778, 0.159999996, -0.0689230412) |
| (0.119999997, -0.159999996, -0.0700000077) | (-0.120621786, -0.159999996, -0.0689230412) |
| (0.119999997, 0.159999996, 0.0700000077) | (0.000621777028, 0.159999996, -0.138923049) |
| (0.119999997, -0.159999996, 0.0699999928) | (0.000621765852, -0.159999996, -0.138923049) |

#### Rz

```text
[ -0.866025507 -0.499999821 0 0 ]
[ 0.499999821 -0.866025507 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.000621765852, 0.159999996, 0.138923049) | (-0.0794615, -0.138874963, 0.138923049) |
| (-0.000621777028, -0.159999996, 0.138923049) | (0.0805384442, 0.138253197, 0.138923049) |
| (0.120621786, 0.159999996, 0.0689230412) | (-0.184461504, -0.0782532096, 0.0689230412) |
| (0.120621778, -0.159999996, 0.0689230412) | (-0.0244615674, 0.19887495, 0.0689230412) |
| (-0.120621778, 0.159999996, -0.0689230412) | (0.0244615674, -0.19887495, -0.0689230412) |
| (-0.120621786, -0.159999996, -0.0689230412) | (0.184461504, 0.0782532096, -0.0689230412) |
| (0.000621777028, 0.159999996, -0.138923049) | (-0.0805384442, -0.138253197, -0.138923049) |
| (0.000621765852, -0.159999996, -0.138923049) | (0.0794615, 0.138874963, -0.138923049) |

#### T

```text
[ 1 0 0 5 ]
[ 0 1 0 24.5 ]
[ 0 0 1 -26 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.0794615, -0.138874963, 0.138923049) | (4.92053843, 24.3611259, -25.8610764) |
| (0.0805384442, 0.138253197, 0.138923049) | (5.08053827, 24.6382523, -25.8610764) |
| (-0.184461504, -0.0782532096, 0.0689230412) | (4.81553841, 24.4217472, -25.931076) |
| (-0.0244615674, 0.19887495, 0.0689230412) | (4.97553825, 24.6988754, -25.931076) |
| (0.0244615674, -0.19887495, -0.0689230412) | (5.02446175, 24.3011246, -26.068924) |
| (0.184461504, 0.0782532096, -0.0689230412) | (5.18446159, 24.5782528, -26.068924) |
| (-0.0805384442, -0.138253197, -0.138923049) | (4.91946173, 24.3617477, -26.1389236) |
| (0.0794615, 0.138874963, -0.138923049) | (5.07946157, 24.6388741, -26.1389236) |

Final M = T Rz Ry Rx H S:

```text
[ 0.103923075 -0.105000012 0.159999952 5 ]
[ -0.0599999838 0.0606217571 0.27712816 24.5 ]
[ -0.20784609 -0.0700000077 6.99382285e-09 -26 ]
[ 0 0 0 1 ]
```

## Exact construction implementation

The following files are copied from this build's source, not hand-maintained snippets.

### Exact source: `src/gameplay/Effects.cpp`

```cpp
#include "gameplay/Effects.h"
namespace shooter {
std::vector<SceneObject> celebrationObjects(float time,bool victory) {
    std::vector<SceneObject> cubes;
    if (time<0 || time>4) return cubes;
    const Vec3 colors[]={{.12f,.85f,.68f},{1,.67f,.16f},{.26f,.54f,1},{.98f,.90f,.61f}};
    for (int i=0;i<(victory?84:28);++i) {
        const float delay=(i%7)*.045f,age=time-delay;
        if(age<0) continue;
        const float angle=i*2.39996f,speed=5+(i%5)*1.4f;
        const Vec3 origin{0,22,-26};
        const Vec3 p=origin+Vec3{std::cos(angle)*speed*age,(5+(i%4))*age-2.5f*age*age,std::sin(angle)*speed*age};
        auto cube=makeCube("CELEBRATION_"+std::to_string(i),"Celebration","Victory confetti",p,
            {.24f,.14f,.32f},colors[i%4]);
        cube.transform.rotation={age*(90+i),age*(120-i),age*150}; cube.emission=.65f;
        cube.notes="Cosmetic transformed cube; ballistic celebration time="+std::to_string(time)+" s; no scoring or collision";
        cubes.push_back(cube);
    }
    return cubes;
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
