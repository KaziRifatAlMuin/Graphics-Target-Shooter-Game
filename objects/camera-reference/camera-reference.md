# camera-reference

Bird's-Eye reset observation camera.

All dimensions are meters; all rotations are degrees. Values below use nine significant digits (enough to recover float values). The renderer uses column vectors and column-major matrix storage. The mathematical application order is:

$$p_w=T R_z R_y R_x H S p_l,\qquad p_{clip}=P V p_w.$$

Scale: (sx*x, sy*y, sz*z). Shear: (x+hxy*y+hxz*z, hyx*x+y+hyz*z, hzx*x+hzy*y+z). For a=degrees*pi/180, c=cos(a), s=sin(a): Rx maps to (x, cy-sz, sy+cz); Ry to (cx+sz, y, -sx+cz); Rz to (cx-sy, sx+cy, z). Translation adds (tx,ty,tz). Each stage uses the previous stage's output. No parent transform is implicitly applied by Renderer: builders have already placed every part in world coordinates. Source listings at the end give exact construction, offset, animation and random-value formulas.

Assembly view eye=(1.62270856, 1.1497606, -3.53979182), center=(0, 0.0949999988, -5.16250038), FOV=45 degrees, aspect=4/3, near=0.00382673554, far=19.184166.

![Complete assembly](complete.png)

![Opposite view of the same assembly](reverse.png)

## Cube assembly order

Each image adds one real component, preserving builder order and world transforms.

### Add 1: Observation ground marker

![Assembly 1](assembly-1.png)

### Add 2: Observation heading

![Assembly 2](assembly-2.png)

## Every component transformation

### Observation ground marker — `OBSERVATION_MARKER`

Actual observation camera at (0.000000,1.700000,-5.000000); yaw=-90.000000; pitch=0.000000

Position T = (0, 0.0450000018, -5) m; scale S = (0.899999976, 0.0799999982, 0.899999976); rotation (Rx,Ry,Rz) = (0, 0, 0) degrees.

Shear (xy,xz,yx,yz,zx,zy) = (0, 0, 0, 0, 0, 0). Color = (0.200000003, 0.899999976, 0.699999988); specular = 0.119999997; shininess = 24; emission = 0; flash = 0; material layer = 0; texture scale = 1. Pattern enabled = 0, pattern scale = (1, 1, 1), offset = (0, 0, 0).

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
[ 0.899999976 0 0 0 ]
[ 0 0.0799999982 0 0 ]
[ 0 0 0.899999976 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.5, -0.5, -0.5) | (-0.449999988, -0.0399999991, -0.449999988) |
| (-0.5, -0.5, 0.5) | (-0.449999988, -0.0399999991, 0.449999988) |
| (-0.5, 0.5, -0.5) | (-0.449999988, 0.0399999991, -0.449999988) |
| (-0.5, 0.5, 0.5) | (-0.449999988, 0.0399999991, 0.449999988) |
| (0.5, -0.5, -0.5) | (0.449999988, -0.0399999991, -0.449999988) |
| (0.5, -0.5, 0.5) | (0.449999988, -0.0399999991, 0.449999988) |
| (0.5, 0.5, -0.5) | (0.449999988, 0.0399999991, -0.449999988) |
| (0.5, 0.5, 0.5) | (0.449999988, 0.0399999991, 0.449999988) |

#### H

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.449999988, -0.0399999991, -0.449999988) | (-0.449999988, -0.0399999991, -0.449999988) |
| (-0.449999988, -0.0399999991, 0.449999988) | (-0.449999988, -0.0399999991, 0.449999988) |
| (-0.449999988, 0.0399999991, -0.449999988) | (-0.449999988, 0.0399999991, -0.449999988) |
| (-0.449999988, 0.0399999991, 0.449999988) | (-0.449999988, 0.0399999991, 0.449999988) |
| (0.449999988, -0.0399999991, -0.449999988) | (0.449999988, -0.0399999991, -0.449999988) |
| (0.449999988, -0.0399999991, 0.449999988) | (0.449999988, -0.0399999991, 0.449999988) |
| (0.449999988, 0.0399999991, -0.449999988) | (0.449999988, 0.0399999991, -0.449999988) |
| (0.449999988, 0.0399999991, 0.449999988) | (0.449999988, 0.0399999991, 0.449999988) |

#### Rx

```text
[ 1 0 0 0 ]
[ 0 1 -0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.449999988, -0.0399999991, -0.449999988) | (-0.449999988, -0.0399999991, -0.449999988) |
| (-0.449999988, -0.0399999991, 0.449999988) | (-0.449999988, -0.0399999991, 0.449999988) |
| (-0.449999988, 0.0399999991, -0.449999988) | (-0.449999988, 0.0399999991, -0.449999988) |
| (-0.449999988, 0.0399999991, 0.449999988) | (-0.449999988, 0.0399999991, 0.449999988) |
| (0.449999988, -0.0399999991, -0.449999988) | (0.449999988, -0.0399999991, -0.449999988) |
| (0.449999988, -0.0399999991, 0.449999988) | (0.449999988, -0.0399999991, 0.449999988) |
| (0.449999988, 0.0399999991, -0.449999988) | (0.449999988, 0.0399999991, -0.449999988) |
| (0.449999988, 0.0399999991, 0.449999988) | (0.449999988, 0.0399999991, 0.449999988) |

#### Ry

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ -0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.449999988, -0.0399999991, -0.449999988) | (-0.449999988, -0.0399999991, -0.449999988) |
| (-0.449999988, -0.0399999991, 0.449999988) | (-0.449999988, -0.0399999991, 0.449999988) |
| (-0.449999988, 0.0399999991, -0.449999988) | (-0.449999988, 0.0399999991, -0.449999988) |
| (-0.449999988, 0.0399999991, 0.449999988) | (-0.449999988, 0.0399999991, 0.449999988) |
| (0.449999988, -0.0399999991, -0.449999988) | (0.449999988, -0.0399999991, -0.449999988) |
| (0.449999988, -0.0399999991, 0.449999988) | (0.449999988, -0.0399999991, 0.449999988) |
| (0.449999988, 0.0399999991, -0.449999988) | (0.449999988, 0.0399999991, -0.449999988) |
| (0.449999988, 0.0399999991, 0.449999988) | (0.449999988, 0.0399999991, 0.449999988) |

#### Rz

```text
[ 1 -0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.449999988, -0.0399999991, -0.449999988) | (-0.449999988, -0.0399999991, -0.449999988) |
| (-0.449999988, -0.0399999991, 0.449999988) | (-0.449999988, -0.0399999991, 0.449999988) |
| (-0.449999988, 0.0399999991, -0.449999988) | (-0.449999988, 0.0399999991, -0.449999988) |
| (-0.449999988, 0.0399999991, 0.449999988) | (-0.449999988, 0.0399999991, 0.449999988) |
| (0.449999988, -0.0399999991, -0.449999988) | (0.449999988, -0.0399999991, -0.449999988) |
| (0.449999988, -0.0399999991, 0.449999988) | (0.449999988, -0.0399999991, 0.449999988) |
| (0.449999988, 0.0399999991, -0.449999988) | (0.449999988, 0.0399999991, -0.449999988) |
| (0.449999988, 0.0399999991, 0.449999988) | (0.449999988, 0.0399999991, 0.449999988) |

#### T

```text
[ 1 0 0 0 ]
[ 0 1 0 0.0450000018 ]
[ 0 0 1 -5 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.449999988, -0.0399999991, -0.449999988) | (-0.449999988, 0.00500000268, -5.44999981) |
| (-0.449999988, -0.0399999991, 0.449999988) | (-0.449999988, 0.00500000268, -4.55000019) |
| (-0.449999988, 0.0399999991, -0.449999988) | (-0.449999988, 0.0850000009, -5.44999981) |
| (-0.449999988, 0.0399999991, 0.449999988) | (-0.449999988, 0.0850000009, -4.55000019) |
| (0.449999988, -0.0399999991, -0.449999988) | (0.449999988, 0.00500000268, -5.44999981) |
| (0.449999988, -0.0399999991, 0.449999988) | (0.449999988, 0.00500000268, -4.55000019) |
| (0.449999988, 0.0399999991, -0.449999988) | (0.449999988, 0.0850000009, -5.44999981) |
| (0.449999988, 0.0399999991, 0.449999988) | (0.449999988, 0.0850000009, -4.55000019) |

Final M = T Rz Ry Rx H S:

```text
[ 0.899999976 0 0 0 ]
[ 0 0.0799999982 0 0.0450000018 ]
[ 0 0 0.899999976 -5 ]
[ 0 0 0 1 ]
```

### Observation heading — `OBSERVATION_HEADING`

Actual observation camera at (0.000000,1.700000,-5.000000); yaw=-90.000000; pitch=0.000000

Position T = (-1.7484556e-08, 0.135000005, -5.4000001) m; scale S = (0.119999997, 0.100000001, 0.75); rotation (Rx,Ry,Rz) = (0, 0, 0) degrees.

Shear (xy,xz,yx,yz,zx,zy) = (0, 0, 0, 0, 0, 0). Color = (1, 0.699999988, 0.200000003); specular = 0.119999997; shininess = 24; emission = 0; flash = 0; material layer = 0; texture scale = 1. Pattern enabled = 0, pattern scale = (1, 1, 1), offset = (0, 0, 0).

| Step | Actual renderer image |
| --- | --- |
| Unit cube | ![Unit cube](part-1-0.png) |
| Scale | ![Scale](part-1-1.png) |
| Shear | ![Shear](part-1-2.png) |
| Rotate X | ![Rotate X](part-1-3.png) |
| Rotate Y | ![Rotate Y](part-1-4.png) |
| Rotate Z | ![Rotate Z](part-1-5.png) |
| Translate to world | ![Translate to world](part-1-6.png) |

Steps 1–5 share one camera; the unit cube has its own close view, and step 6 is reframed at the actual world location to keep small objects visible. Reframing changes the view, never the model coordinates. Identity steps deliberately remain visible. Intermediate shapes are instructional states, while step 6 uses the unmodified game object.

Each stage below gives its exact numeric matrix and all eight corner multiplications. For each output row r: p'[r] = A[r,0]p.x + A[r,1]p.y + A[r,2]p.z + A[r,3]p.w.

![Signed translation components](part-1-translation.svg)


#### S

```text
[ 0.119999997 0 0 0 ]
[ 0 0.100000001 0 0 ]
[ 0 0 0.75 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.5, -0.5, -0.5) | (-0.0599999987, -0.0500000007, -0.375) |
| (-0.5, -0.5, 0.5) | (-0.0599999987, -0.0500000007, 0.375) |
| (-0.5, 0.5, -0.5) | (-0.0599999987, 0.0500000007, -0.375) |
| (-0.5, 0.5, 0.5) | (-0.0599999987, 0.0500000007, 0.375) |
| (0.5, -0.5, -0.5) | (0.0599999987, -0.0500000007, -0.375) |
| (0.5, -0.5, 0.5) | (0.0599999987, -0.0500000007, 0.375) |
| (0.5, 0.5, -0.5) | (0.0599999987, 0.0500000007, -0.375) |
| (0.5, 0.5, 0.5) | (0.0599999987, 0.0500000007, 0.375) |

#### H

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.0599999987, -0.0500000007, -0.375) | (-0.0599999987, -0.0500000007, -0.375) |
| (-0.0599999987, -0.0500000007, 0.375) | (-0.0599999987, -0.0500000007, 0.375) |
| (-0.0599999987, 0.0500000007, -0.375) | (-0.0599999987, 0.0500000007, -0.375) |
| (-0.0599999987, 0.0500000007, 0.375) | (-0.0599999987, 0.0500000007, 0.375) |
| (0.0599999987, -0.0500000007, -0.375) | (0.0599999987, -0.0500000007, -0.375) |
| (0.0599999987, -0.0500000007, 0.375) | (0.0599999987, -0.0500000007, 0.375) |
| (0.0599999987, 0.0500000007, -0.375) | (0.0599999987, 0.0500000007, -0.375) |
| (0.0599999987, 0.0500000007, 0.375) | (0.0599999987, 0.0500000007, 0.375) |

#### Rx

```text
[ 1 0 0 0 ]
[ 0 1 -0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.0599999987, -0.0500000007, -0.375) | (-0.0599999987, -0.0500000007, -0.375) |
| (-0.0599999987, -0.0500000007, 0.375) | (-0.0599999987, -0.0500000007, 0.375) |
| (-0.0599999987, 0.0500000007, -0.375) | (-0.0599999987, 0.0500000007, -0.375) |
| (-0.0599999987, 0.0500000007, 0.375) | (-0.0599999987, 0.0500000007, 0.375) |
| (0.0599999987, -0.0500000007, -0.375) | (0.0599999987, -0.0500000007, -0.375) |
| (0.0599999987, -0.0500000007, 0.375) | (0.0599999987, -0.0500000007, 0.375) |
| (0.0599999987, 0.0500000007, -0.375) | (0.0599999987, 0.0500000007, -0.375) |
| (0.0599999987, 0.0500000007, 0.375) | (0.0599999987, 0.0500000007, 0.375) |

#### Ry

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ -0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.0599999987, -0.0500000007, -0.375) | (-0.0599999987, -0.0500000007, -0.375) |
| (-0.0599999987, -0.0500000007, 0.375) | (-0.0599999987, -0.0500000007, 0.375) |
| (-0.0599999987, 0.0500000007, -0.375) | (-0.0599999987, 0.0500000007, -0.375) |
| (-0.0599999987, 0.0500000007, 0.375) | (-0.0599999987, 0.0500000007, 0.375) |
| (0.0599999987, -0.0500000007, -0.375) | (0.0599999987, -0.0500000007, -0.375) |
| (0.0599999987, -0.0500000007, 0.375) | (0.0599999987, -0.0500000007, 0.375) |
| (0.0599999987, 0.0500000007, -0.375) | (0.0599999987, 0.0500000007, -0.375) |
| (0.0599999987, 0.0500000007, 0.375) | (0.0599999987, 0.0500000007, 0.375) |

#### Rz

```text
[ 1 -0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.0599999987, -0.0500000007, -0.375) | (-0.0599999987, -0.0500000007, -0.375) |
| (-0.0599999987, -0.0500000007, 0.375) | (-0.0599999987, -0.0500000007, 0.375) |
| (-0.0599999987, 0.0500000007, -0.375) | (-0.0599999987, 0.0500000007, -0.375) |
| (-0.0599999987, 0.0500000007, 0.375) | (-0.0599999987, 0.0500000007, 0.375) |
| (0.0599999987, -0.0500000007, -0.375) | (0.0599999987, -0.0500000007, -0.375) |
| (0.0599999987, -0.0500000007, 0.375) | (0.0599999987, -0.0500000007, 0.375) |
| (0.0599999987, 0.0500000007, -0.375) | (0.0599999987, 0.0500000007, -0.375) |
| (0.0599999987, 0.0500000007, 0.375) | (0.0599999987, 0.0500000007, 0.375) |

#### T

```text
[ 1 0 0 -1.7484556e-08 ]
[ 0 1 0 0.135000005 ]
[ 0 0 1 -5.4000001 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.0599999987, -0.0500000007, -0.375) | (-0.0600000173, 0.0850000083, -5.7750001) |
| (-0.0599999987, -0.0500000007, 0.375) | (-0.0600000173, 0.0850000083, -5.0250001) |
| (-0.0599999987, 0.0500000007, -0.375) | (-0.0600000173, 0.185000002, -5.7750001) |
| (-0.0599999987, 0.0500000007, 0.375) | (-0.0600000173, 0.185000002, -5.0250001) |
| (0.0599999987, -0.0500000007, -0.375) | (0.05999998, 0.0850000083, -5.7750001) |
| (0.0599999987, -0.0500000007, 0.375) | (0.05999998, 0.0850000083, -5.0250001) |
| (0.0599999987, 0.0500000007, -0.375) | (0.05999998, 0.185000002, -5.7750001) |
| (0.0599999987, 0.0500000007, 0.375) | (0.05999998, 0.185000002, -5.0250001) |

Final M = T Rz Ry Rx H S:

```text
[ 0.119999997 0 0 -1.7484556e-08 ]
[ 0 0.100000001 0 0.135000005 ]
[ 0 0 0.75 -5.4000001 ]
[ 0 0 0 1 ]
```

## Exact construction implementation

The following files are copied from this build's source, not hand-maintained snippets.

### Exact source: `src/camera/BirdEyeCamera.cpp`

```cpp
#include "camera/BirdEyeCamera.h"
#include "core/Collision.h"
#include <algorithm>
namespace shooter {
BirdEyeCamera::BirdEyeCamera() { reset(); }
void BirdEyeCamera::reset() {
    overhead.position={0,120,-50}; overhead.yaw=-90; overhead.pitch=-89.9f;
    observation.position={0,1.7f,-5}; observation.yaw=-90; observation.pitch=0; observing=false;
}
void BirdEyeCamera::overview() { observing=false; }
void BirdEyeCamera::pan(float f,float r,float dt) {
    if (observing) return;
    overhead.position.x=std::clamp(overhead.position.x+r*25*dt,-25.f,25.f);
    overhead.position.z=std::clamp(overhead.position.z-f*25*dt,-90.f,-10.f);
}
void BirdEyeCamera::zoom(float steps) {
    if (!observing) overhead.position.y=std::clamp(overhead.position.y-steps*8,65.f,180.f);
}
std::optional<Vec3> BirdEyeCamera::groundPoint(float u,float v,float aspect) const {
    if (u<0 || u>1 || v<0 || v>1 || aspect<=0) return {};
    // Invert the same 60-degree perspective/view mapping used by Renderer.
    const auto forward=overhead.forward(),right=normalize(cross(forward,{0,1,0})),up=cross(right,forward);
    const float half=std::tan(radians(60)*.5f);
    const auto ray=normalize(forward+right*((2*u-1)*aspect*half)+up*((1-2*v)*half));
    if (ray.y>=-.0001f) return {};
    const auto p=overhead.position+ray*(-overhead.position.y/ray.y);
    if (!std::isfinite(p.x) || !std::isfinite(p.z)) return {};
    return Vec3{p.x,0,p.z};
}
bool BirdEyeCamera::validPosition(Vec3 p,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets) const {
    if (!canStandAt(p,obstacles)) return false;
    for (const auto& t:targets) if (std::abs(p.x-t.position.x)<1.45f && std::abs(p.z-t.position.z)<1.25f) return false;
    return true;
}
bool BirdEyeCamera::observeAt(float u,float v,float aspect,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets) {
    const auto point=groundPoint(u,v,aspect);
    if (!point || !validPosition(*point,obstacles,targets)) return false;
    const auto direction=normalize(*point-overhead.position); const float distance=length(*point-overhead.position);
    for (const auto& o:obstacles) if (o.type!="Arena" && intersectCube(overhead.position,direction,o.transform,distance)<distance) return false;
    observation.position=*point+Vec3{0,1.7f,0}; observation.yaw=overhead.yaw; observation.pitch=0;
    observing=true; return true;
}
void BirdEyeCamera::moveObservation(float f,float r,float dt,bool fast,const std::vector<SceneObject>& obstacles,const std::vector<Target>& targets) {
    auto forward=observation.forward(); forward.y=0; forward=normalize(forward);
    const auto delta=normalize(forward*f+cross(forward,{0,1,0})*r)*((fast?9.f:5.f)*dt);
    const int count=std::max(1,int(std::ceil(length(delta)/.15f)));
    for (int i=0;i<count;++i) {
        auto next=observation.position; next.x+=delta.x/count;
        if (validPosition(next,obstacles,targets)) observation.position=next;
        next=observation.position; next.z+=delta.z/count;
        if (validPosition(next,obstacles,targets)) observation.position=next;
    }
}
std::vector<SceneObject> BirdEyeCamera::referenceObjects() const {
    // This visible marker is anchored to the real observation camera, not a CSV-only sample.
    auto base=makeCube("OBSERVATION_MARKER","Camera reference","Observation ground marker",
        {observation.position.x,.045f,observation.position.z},{.9f,.08f,.9f},{.2f,.9f,.7f});
    base.notes="Actual observation camera at ("+std::to_string(observation.position.x)+","+
        std::to_string(observation.position.y)+","+std::to_string(observation.position.z)+
        "); yaw="+std::to_string(observation.yaw)+"; pitch="+std::to_string(observation.pitch);
    auto heading=makeCube("OBSERVATION_HEADING","Camera reference","Observation heading",
        base.transform.position+observation.forward()*.4f+Vec3{0,.09f,0},{.12f,.1f,.75f},{1,.7f,.2f},-observation.yaw-90);
    heading.transform.rotation.x=observation.pitch; heading.notes=base.notes;
    return {base,heading};
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
