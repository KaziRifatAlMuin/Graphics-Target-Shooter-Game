# boundary-corner-cap

Practice startup, simulation time 0; cargo seed 2107042. First encountered instance per assembly type.

All dimensions are meters; all rotations are degrees. Values below use nine significant digits (enough to recover float values). The renderer uses column vectors and column-major matrix storage. The mathematical application order is:

$$p_w=T R_z R_y R_x H S p_l,\qquad p_{clip}=P V p_w.$$

Scale: (sx*x, sy*y, sz*z). Shear: (x+hxy*y+hxz*z, hyx*x+y+hyz*z, hzx*x+hzy*y+z). For a=degrees*pi/180, c=cos(a), s=sin(a): Rx maps to (x, cy-sz, sy+cz); Ry to (cx+sz, y, -sx+cz); Rz to (cx-sy, sx+cy, z). Translation adds (tx,ty,tz). Each stage uses the previous stage's output. No parent transform is implicitly applied by Renderer: builders have already placed every part in world coordinates. Source listings at the end give exact construction, offset, animation and random-value formulas.

Assembly view eye=(-23.6146393, 14.150485, -93.6146393), center=(-30, 10, -100), FOV=45 degrees, aspect=4/3, near=0.0150582129, far=46.1397133.

![Complete assembly](complete.png)

![Opposite view of the same assembly](reverse.png)

## Cube assembly order

Each image adds one real component, preserving builder order and world transforms.

### Add 1: Corner cap

![Assembly 1](assembly-1.png)

## Every component transformation

### Corner cap — `CORNER_CAP_1`

Fortified boundary; 1 unit = 1 m

Position T = (-30, 10, -100) m; scale S = (4.19999981, 1, 4.19999981); rotation (Rx,Ry,Rz) = (0, 0, 0) degrees.

Shear (xy,xz,yx,yz,zx,zy) = (0, 0, 0, 0, 0, 0). Color = (0.620000005, 0.680000007, 0.720000029); specular = 0.119999997; shininess = 24; emission = 0; flash = 0; material layer = 2; texture scale = 0.5. Pattern enabled = 0, pattern scale = (1, 1, 1), offset = (0, 0, 0).

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
[ 4.19999981 0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 4.19999981 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.5, -0.5, -0.5) | (-2.0999999, -0.5, -2.0999999) |
| (-0.5, -0.5, 0.5) | (-2.0999999, -0.5, 2.0999999) |
| (-0.5, 0.5, -0.5) | (-2.0999999, 0.5, -2.0999999) |
| (-0.5, 0.5, 0.5) | (-2.0999999, 0.5, 2.0999999) |
| (0.5, -0.5, -0.5) | (2.0999999, -0.5, -2.0999999) |
| (0.5, -0.5, 0.5) | (2.0999999, -0.5, 2.0999999) |
| (0.5, 0.5, -0.5) | (2.0999999, 0.5, -2.0999999) |
| (0.5, 0.5, 0.5) | (2.0999999, 0.5, 2.0999999) |

#### H

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-2.0999999, -0.5, -2.0999999) | (-2.0999999, -0.5, -2.0999999) |
| (-2.0999999, -0.5, 2.0999999) | (-2.0999999, -0.5, 2.0999999) |
| (-2.0999999, 0.5, -2.0999999) | (-2.0999999, 0.5, -2.0999999) |
| (-2.0999999, 0.5, 2.0999999) | (-2.0999999, 0.5, 2.0999999) |
| (2.0999999, -0.5, -2.0999999) | (2.0999999, -0.5, -2.0999999) |
| (2.0999999, -0.5, 2.0999999) | (2.0999999, -0.5, 2.0999999) |
| (2.0999999, 0.5, -2.0999999) | (2.0999999, 0.5, -2.0999999) |
| (2.0999999, 0.5, 2.0999999) | (2.0999999, 0.5, 2.0999999) |

#### Rx

```text
[ 1 0 0 0 ]
[ 0 1 -0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-2.0999999, -0.5, -2.0999999) | (-2.0999999, -0.5, -2.0999999) |
| (-2.0999999, -0.5, 2.0999999) | (-2.0999999, -0.5, 2.0999999) |
| (-2.0999999, 0.5, -2.0999999) | (-2.0999999, 0.5, -2.0999999) |
| (-2.0999999, 0.5, 2.0999999) | (-2.0999999, 0.5, 2.0999999) |
| (2.0999999, -0.5, -2.0999999) | (2.0999999, -0.5, -2.0999999) |
| (2.0999999, -0.5, 2.0999999) | (2.0999999, -0.5, 2.0999999) |
| (2.0999999, 0.5, -2.0999999) | (2.0999999, 0.5, -2.0999999) |
| (2.0999999, 0.5, 2.0999999) | (2.0999999, 0.5, 2.0999999) |

#### Ry

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ -0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-2.0999999, -0.5, -2.0999999) | (-2.0999999, -0.5, -2.0999999) |
| (-2.0999999, -0.5, 2.0999999) | (-2.0999999, -0.5, 2.0999999) |
| (-2.0999999, 0.5, -2.0999999) | (-2.0999999, 0.5, -2.0999999) |
| (-2.0999999, 0.5, 2.0999999) | (-2.0999999, 0.5, 2.0999999) |
| (2.0999999, -0.5, -2.0999999) | (2.0999999, -0.5, -2.0999999) |
| (2.0999999, -0.5, 2.0999999) | (2.0999999, -0.5, 2.0999999) |
| (2.0999999, 0.5, -2.0999999) | (2.0999999, 0.5, -2.0999999) |
| (2.0999999, 0.5, 2.0999999) | (2.0999999, 0.5, 2.0999999) |

#### Rz

```text
[ 1 -0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-2.0999999, -0.5, -2.0999999) | (-2.0999999, -0.5, -2.0999999) |
| (-2.0999999, -0.5, 2.0999999) | (-2.0999999, -0.5, 2.0999999) |
| (-2.0999999, 0.5, -2.0999999) | (-2.0999999, 0.5, -2.0999999) |
| (-2.0999999, 0.5, 2.0999999) | (-2.0999999, 0.5, 2.0999999) |
| (2.0999999, -0.5, -2.0999999) | (2.0999999, -0.5, -2.0999999) |
| (2.0999999, -0.5, 2.0999999) | (2.0999999, -0.5, 2.0999999) |
| (2.0999999, 0.5, -2.0999999) | (2.0999999, 0.5, -2.0999999) |
| (2.0999999, 0.5, 2.0999999) | (2.0999999, 0.5, 2.0999999) |

#### T

```text
[ 1 0 0 -30 ]
[ 0 1 0 10 ]
[ 0 0 1 -100 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-2.0999999, -0.5, -2.0999999) | (-32.0999985, 9.5, -102.099998) |
| (-2.0999999, -0.5, 2.0999999) | (-32.0999985, 9.5, -97.9000015) |
| (-2.0999999, 0.5, -2.0999999) | (-32.0999985, 10.5, -102.099998) |
| (-2.0999999, 0.5, 2.0999999) | (-32.0999985, 10.5, -97.9000015) |
| (2.0999999, -0.5, -2.0999999) | (-27.8999996, 9.5, -102.099998) |
| (2.0999999, -0.5, 2.0999999) | (-27.8999996, 9.5, -97.9000015) |
| (2.0999999, 0.5, -2.0999999) | (-27.8999996, 10.5, -102.099998) |
| (2.0999999, 0.5, 2.0999999) | (-27.8999996, 10.5, -97.9000015) |

Final M = T Rz Ry Rx H S:

```text
[ 4.19999981 0 0 -30 ]
[ 0 1 0 10 ]
[ 0 0 4.19999981 -100 ]
[ 0 0 0 1 ]
```

## Exact construction implementation

The following files are copied from this build's source, not hand-maintained snippets.

### Exact source: `src/world/Arena.cpp`

```cpp
#include "world/Arena.h"

namespace shooter {
std::vector<SceneObject> createArena() {
    std::vector<SceneObject> objects;
    const Vec3 stone{0.49f,0.55f,0.61f}, top{0.62f,0.68f,0.72f};
    auto add = [&](std::string id, std::string type, std::string component,
                   Vec3 position, Vec3 scale, Vec3 color, float yaw = 0,
                   std::string notes = "Fortified boundary; 1 unit = 1 m") {
        Transform t;
        t.position=position; t.scale=scale; t.rotation.y=yaw;
        objects.push_back({id,type,component,t,color,notes});
    };
    add("ARENA_FLOOR","Arena","Floor slab",{0,-0.1f,-50},{60,0.2f,100},
        {0.29f,0.36f,0.30f},0,"60 x 100 m; top face at Y = 0; forward = -Z");
    for (int lane:{-1,1}) for (int z=-8;z>=-94;z-=4)
        add("AISLE_MARK_"+std::to_string(lane)+"_"+std::to_string(-z),"Arena","Painted route marker",
            {lane*2.8f,.012f,float(z)},{.075f,.018f,1.35f},{.74f,.66f,.35f},0,"Thin cube paint strip; navigable central aisle");
    // Four continuous 8 m walls. Supports decorate them without cutting openings.
    add("WALL_N","Boundary","North wall",{0,4,-100},{60,8,1},stone);
    add("WALL_W","Boundary","West wall",{-30,4,-50},{100,8,1},stone,90);
    add("WALL_E","Boundary","East wall",{30,4,-50},{100,8,1},stone,90);
    add("WALL_S","Boundary","South wall",{0,4,0},{60,8,1},stone);
    for (int course=1;course<=3;++course) {
        for (int side:{-1,1})
            add("STONE_COURSE_"+std::to_string(side)+"_"+std::to_string(course),"Boundary","Masonry string course",
                {side*29.44f,float(course)*2,-50},{.14f,.13f,99},{.37f,.41f,.46f});
        for (int end:{0,-100})
            add("END_COURSE_"+std::to_string(end)+"_"+std::to_string(course),"Boundary","Masonry string course",
                {0,float(course)*2,end==0?-.56f:-99.44f},{59,.13f,.14f},{.37f,.41f,.46f});
    }
    int block = 0;
    for (int x=-27; x<=27; x+=6) {
        add("BATTLEMENT_"+std::to_string(++block),"Boundary","North top block",
            {float(x),8.75f,-100},{2,1.5f,1.5f},top);
        add("BATTLEMENT_"+std::to_string(++block),"Boundary","South top block",
            {float(x),8.75f,0},{2,1.5f,1.5f},top);
    }
    for (int z=-6; z>=-94; z-=6) {
        for (int side : {-1,1})
            add("BATTLEMENT_"+std::to_string(++block),"Boundary","Side top block",
                {side*30.0f,8.75f,float(z)},{2,1.5f,1.5f},top,90);
    }
    int corner = 0;
    for (int x : {-30,30}) for (int z : {-100,0}) {
        const auto suffix = std::to_string(++corner);
        add("CORNER_"+suffix,"Boundary","Thick corner tower",
            {float(x),4.75f,float(z)},{3.5f,9.5f,3.5f},stone);
        add("CORNER_CAP_"+suffix,"Boundary","Corner cap",
            {float(x),10,float(z)},{4.2f,1,4.2f},top);
    }
    // Leaning stone supports demonstrate non-identity shear and all rotation axes.
    // The enclosing walls above stay vertical and continuous on all four sides.
    auto support = [&](const std::string& id, Vec3 position, Vec3 rotation) {
        Transform t;
        t.position=position;
        t.scale={1.5f,6,2};
        t.shear[0]=0.22f; // x' = x + 0.22*y, applied after scaling.
        t.rotation=rotation;
        // Anchor the lowest transformed cube corner exactly to ground level.
        t.position.y=0;
        float lowest=0;
        const Mat4 model=composeModelMatrix(t);
        for (float x : {-0.5f,0.5f}) for (float y : {-0.5f,0.5f}) for (float z : {-0.5f,0.5f}) {
            const float height=transformPoint(model,{x,y,z,1}).y;
            if (height<lowest) lowest=height;
        }
        t.position.y=-lowest;
        objects.push_back({id,"Boundary","Sheared stone support",t,{0.66f,0.59f,0.46f},
            "Scale -> XY shear -> Rx -> Ry -> Rz -> translation; lowest corner at Y = 0"});
    };
    for (int z : {-18,-42,-66,-90}) {
        support("SUPPORT_E_"+std::to_string(-z),{28,0,float(z)},{0,0,-8});
        support("SUPPORT_W_"+std::to_string(-z),{-28,0,float(z)},{0,180,8});
    }
    for (int x : {-20,-10,0,10,20}) {
        support("SUPPORT_N_"+std::to_string(x),{float(x),0,-98},{-8,90,0});
        support("SUPPORT_S_"+std::to_string(x),{float(x),0,-2},{8,-90,0});
    }
    return objects;
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
