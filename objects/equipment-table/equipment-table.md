# equipment-table

Practice startup, simulation time 0; cargo seed 2107042. First encountered instance per assembly type.

All dimensions are meters; all rotations are degrees. Values below use nine significant digits (enough to recover float values). The renderer uses column vectors and column-major matrix storage. The mathematical application order is:

$$p_w=T R_z R_y R_x H S p_l,\qquad p_{clip}=P V p_w.$$

Scale: (sx*x, sy*y, sz*z). Shear: (x+hxy*y+hxz*z, hyx*x+y+hyz*z, hzx*x+hzy*y+z). For a=degrees*pi/180, c=cos(a), s=sin(a): Rx maps to (x, cy-sz, sy+cz); Ry to (cx+sz, y, -sx+cz); Rz to (cx-sy, sx+cy, z). Translation adds (tx,ty,tz). Each stage uses the previous stage's output. No parent transform is implicitly applied by Renderer: builders have already placed every part in world coordinates. Source listings at the end give exact construction, offset, animation and random-value formulas.

Assembly view eye=(4.16868877, 9.13089752, 5.16868877), center=(-9, 0.571250021, -8), FOV=45 degrees, aspect=4/3, near=0.0310549233, far=84.5318146.

![Complete assembly](complete.png)

![Opposite view of the same assembly](reverse.png)

## Cube assembly order

Each image adds one real component, preserving builder order and world transforms.

### Add 1: Equipment table

![Assembly 1](assembly-1.png)

### Add 2: Equipment mat

![Assembly 2](assembly-2.png)

### Add 3: Display identification plate

![Assembly 3](assembly-3.png)

### Add 4: Display identification plate

![Assembly 4](assembly-4.png)

### Add 5: Display identification plate

![Assembly 5](assembly-5.png)

## Every component transformation

### Equipment table — `EQUIPMENT_TABLE`

1 unit = 1 m; shared cube transform pipeline

Position T = (-9, 0.550000012, -8) m; scale S = (12, 1.10000002, 3); rotation (Rx,Ry,Rz) = (0, 0, 0) degrees.

Shear (xy,xz,yx,yz,zx,zy) = (0, 0, 0, 0, 0, 0). Color = (0.280000001, 0.319999993, 0.340000004); specular = 0.119999997; shininess = 24; emission = 0; flash = 0; material layer = 5; texture scale = 3. Pattern enabled = 0, pattern scale = (1, 1, 1), offset = (0, 0, 0).

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
[ 12 0 0 0 ]
[ 0 1.10000002 0 0 ]
[ 0 0 3 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.5, -0.5, -0.5) | (-6, -0.550000012, -1.5) |
| (-0.5, -0.5, 0.5) | (-6, -0.550000012, 1.5) |
| (-0.5, 0.5, -0.5) | (-6, 0.550000012, -1.5) |
| (-0.5, 0.5, 0.5) | (-6, 0.550000012, 1.5) |
| (0.5, -0.5, -0.5) | (6, -0.550000012, -1.5) |
| (0.5, -0.5, 0.5) | (6, -0.550000012, 1.5) |
| (0.5, 0.5, -0.5) | (6, 0.550000012, -1.5) |
| (0.5, 0.5, 0.5) | (6, 0.550000012, 1.5) |

#### H

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-6, -0.550000012, -1.5) | (-6, -0.550000012, -1.5) |
| (-6, -0.550000012, 1.5) | (-6, -0.550000012, 1.5) |
| (-6, 0.550000012, -1.5) | (-6, 0.550000012, -1.5) |
| (-6, 0.550000012, 1.5) | (-6, 0.550000012, 1.5) |
| (6, -0.550000012, -1.5) | (6, -0.550000012, -1.5) |
| (6, -0.550000012, 1.5) | (6, -0.550000012, 1.5) |
| (6, 0.550000012, -1.5) | (6, 0.550000012, -1.5) |
| (6, 0.550000012, 1.5) | (6, 0.550000012, 1.5) |

#### Rx

```text
[ 1 0 0 0 ]
[ 0 1 -0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-6, -0.550000012, -1.5) | (-6, -0.550000012, -1.5) |
| (-6, -0.550000012, 1.5) | (-6, -0.550000012, 1.5) |
| (-6, 0.550000012, -1.5) | (-6, 0.550000012, -1.5) |
| (-6, 0.550000012, 1.5) | (-6, 0.550000012, 1.5) |
| (6, -0.550000012, -1.5) | (6, -0.550000012, -1.5) |
| (6, -0.550000012, 1.5) | (6, -0.550000012, 1.5) |
| (6, 0.550000012, -1.5) | (6, 0.550000012, -1.5) |
| (6, 0.550000012, 1.5) | (6, 0.550000012, 1.5) |

#### Ry

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ -0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-6, -0.550000012, -1.5) | (-6, -0.550000012, -1.5) |
| (-6, -0.550000012, 1.5) | (-6, -0.550000012, 1.5) |
| (-6, 0.550000012, -1.5) | (-6, 0.550000012, -1.5) |
| (-6, 0.550000012, 1.5) | (-6, 0.550000012, 1.5) |
| (6, -0.550000012, -1.5) | (6, -0.550000012, -1.5) |
| (6, -0.550000012, 1.5) | (6, -0.550000012, 1.5) |
| (6, 0.550000012, -1.5) | (6, 0.550000012, -1.5) |
| (6, 0.550000012, 1.5) | (6, 0.550000012, 1.5) |

#### Rz

```text
[ 1 -0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-6, -0.550000012, -1.5) | (-6, -0.550000012, -1.5) |
| (-6, -0.550000012, 1.5) | (-6, -0.550000012, 1.5) |
| (-6, 0.550000012, -1.5) | (-6, 0.550000012, -1.5) |
| (-6, 0.550000012, 1.5) | (-6, 0.550000012, 1.5) |
| (6, -0.550000012, -1.5) | (6, -0.550000012, -1.5) |
| (6, -0.550000012, 1.5) | (6, -0.550000012, 1.5) |
| (6, 0.550000012, -1.5) | (6, 0.550000012, -1.5) |
| (6, 0.550000012, 1.5) | (6, 0.550000012, 1.5) |

#### T

```text
[ 1 0 0 -9 ]
[ 0 1 0 0.550000012 ]
[ 0 0 1 -8 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-6, -0.550000012, -1.5) | (-15, 0, -9.5) |
| (-6, -0.550000012, 1.5) | (-15, 0, -6.5) |
| (-6, 0.550000012, -1.5) | (-15, 1.10000002, -9.5) |
| (-6, 0.550000012, 1.5) | (-15, 1.10000002, -6.5) |
| (6, -0.550000012, -1.5) | (-3, 0, -9.5) |
| (6, -0.550000012, 1.5) | (-3, 0, -6.5) |
| (6, 0.550000012, -1.5) | (-3, 1.10000002, -9.5) |
| (6, 0.550000012, 1.5) | (-3, 1.10000002, -6.5) |

Final M = T Rz Ry Rx H S:

```text
[ 12 0 0 -9 ]
[ 0 1.10000002 0 0.550000012 ]
[ 0 0 3 -8 ]
[ 0 0 0 1 ]
```

### Equipment mat — `EQUIPMENT_SURFACE`

1 unit = 1 m; shared cube transform pipeline

Position T = (-9, 1.11000001, -8) m; scale S = (11.8000002, 0.0199999996, 2.79999995); rotation (Rx,Ry,Rz) = (0, 0, 0) degrees.

Shear (xy,xz,yx,yz,zx,zy) = (0, 0, 0, 0, 0, 0). Color = (0.159999996, 0.209999993, 0.230000004); specular = 0.119999997; shininess = 24; emission = 0; flash = 0; material layer = 8; texture scale = 8. Pattern enabled = 0, pattern scale = (1, 1, 1), offset = (0, 0, 0).

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
[ 11.8000002 0 0 0 ]
[ 0 0.0199999996 0 0 ]
[ 0 0 2.79999995 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.5, -0.5, -0.5) | (-5.9000001, -0.00999999978, -1.39999998) |
| (-0.5, -0.5, 0.5) | (-5.9000001, -0.00999999978, 1.39999998) |
| (-0.5, 0.5, -0.5) | (-5.9000001, 0.00999999978, -1.39999998) |
| (-0.5, 0.5, 0.5) | (-5.9000001, 0.00999999978, 1.39999998) |
| (0.5, -0.5, -0.5) | (5.9000001, -0.00999999978, -1.39999998) |
| (0.5, -0.5, 0.5) | (5.9000001, -0.00999999978, 1.39999998) |
| (0.5, 0.5, -0.5) | (5.9000001, 0.00999999978, -1.39999998) |
| (0.5, 0.5, 0.5) | (5.9000001, 0.00999999978, 1.39999998) |

#### H

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-5.9000001, -0.00999999978, -1.39999998) | (-5.9000001, -0.00999999978, -1.39999998) |
| (-5.9000001, -0.00999999978, 1.39999998) | (-5.9000001, -0.00999999978, 1.39999998) |
| (-5.9000001, 0.00999999978, -1.39999998) | (-5.9000001, 0.00999999978, -1.39999998) |
| (-5.9000001, 0.00999999978, 1.39999998) | (-5.9000001, 0.00999999978, 1.39999998) |
| (5.9000001, -0.00999999978, -1.39999998) | (5.9000001, -0.00999999978, -1.39999998) |
| (5.9000001, -0.00999999978, 1.39999998) | (5.9000001, -0.00999999978, 1.39999998) |
| (5.9000001, 0.00999999978, -1.39999998) | (5.9000001, 0.00999999978, -1.39999998) |
| (5.9000001, 0.00999999978, 1.39999998) | (5.9000001, 0.00999999978, 1.39999998) |

#### Rx

```text
[ 1 0 0 0 ]
[ 0 1 -0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-5.9000001, -0.00999999978, -1.39999998) | (-5.9000001, -0.00999999978, -1.39999998) |
| (-5.9000001, -0.00999999978, 1.39999998) | (-5.9000001, -0.00999999978, 1.39999998) |
| (-5.9000001, 0.00999999978, -1.39999998) | (-5.9000001, 0.00999999978, -1.39999998) |
| (-5.9000001, 0.00999999978, 1.39999998) | (-5.9000001, 0.00999999978, 1.39999998) |
| (5.9000001, -0.00999999978, -1.39999998) | (5.9000001, -0.00999999978, -1.39999998) |
| (5.9000001, -0.00999999978, 1.39999998) | (5.9000001, -0.00999999978, 1.39999998) |
| (5.9000001, 0.00999999978, -1.39999998) | (5.9000001, 0.00999999978, -1.39999998) |
| (5.9000001, 0.00999999978, 1.39999998) | (5.9000001, 0.00999999978, 1.39999998) |

#### Ry

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ -0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-5.9000001, -0.00999999978, -1.39999998) | (-5.9000001, -0.00999999978, -1.39999998) |
| (-5.9000001, -0.00999999978, 1.39999998) | (-5.9000001, -0.00999999978, 1.39999998) |
| (-5.9000001, 0.00999999978, -1.39999998) | (-5.9000001, 0.00999999978, -1.39999998) |
| (-5.9000001, 0.00999999978, 1.39999998) | (-5.9000001, 0.00999999978, 1.39999998) |
| (5.9000001, -0.00999999978, -1.39999998) | (5.9000001, -0.00999999978, -1.39999998) |
| (5.9000001, -0.00999999978, 1.39999998) | (5.9000001, -0.00999999978, 1.39999998) |
| (5.9000001, 0.00999999978, -1.39999998) | (5.9000001, 0.00999999978, -1.39999998) |
| (5.9000001, 0.00999999978, 1.39999998) | (5.9000001, 0.00999999978, 1.39999998) |

#### Rz

```text
[ 1 -0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-5.9000001, -0.00999999978, -1.39999998) | (-5.9000001, -0.00999999978, -1.39999998) |
| (-5.9000001, -0.00999999978, 1.39999998) | (-5.9000001, -0.00999999978, 1.39999998) |
| (-5.9000001, 0.00999999978, -1.39999998) | (-5.9000001, 0.00999999978, -1.39999998) |
| (-5.9000001, 0.00999999978, 1.39999998) | (-5.9000001, 0.00999999978, 1.39999998) |
| (5.9000001, -0.00999999978, -1.39999998) | (5.9000001, -0.00999999978, -1.39999998) |
| (5.9000001, -0.00999999978, 1.39999998) | (5.9000001, -0.00999999978, 1.39999998) |
| (5.9000001, 0.00999999978, -1.39999998) | (5.9000001, 0.00999999978, -1.39999998) |
| (5.9000001, 0.00999999978, 1.39999998) | (5.9000001, 0.00999999978, 1.39999998) |

#### T

```text
[ 1 0 0 -9 ]
[ 0 1 0 1.11000001 ]
[ 0 0 1 -8 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-5.9000001, -0.00999999978, -1.39999998) | (-14.8999996, 1.10000002, -9.39999962) |
| (-5.9000001, -0.00999999978, 1.39999998) | (-14.8999996, 1.10000002, -6.5999999) |
| (-5.9000001, 0.00999999978, -1.39999998) | (-14.8999996, 1.12, -9.39999962) |
| (-5.9000001, 0.00999999978, 1.39999998) | (-14.8999996, 1.12, -6.5999999) |
| (5.9000001, -0.00999999978, -1.39999998) | (-3.0999999, 1.10000002, -9.39999962) |
| (5.9000001, -0.00999999978, 1.39999998) | (-3.0999999, 1.10000002, -6.5999999) |
| (5.9000001, 0.00999999978, -1.39999998) | (-3.0999999, 1.12, -9.39999962) |
| (5.9000001, 0.00999999978, 1.39999998) | (-3.0999999, 1.12, -6.5999999) |

Final M = T Rz Ry Rx H S:

```text
[ 11.8000002 0 0 -9 ]
[ 0 0.0199999996 0 1.11000001 ]
[ 0 0 2.79999995 -8 ]
[ 0 0 0 1 ]
```

### Display identification plate — `EQUIPMENT_LABEL_0`

1 unit = 1 m; shared cube transform pipeline

Position T = (-13, 1.13, -6.80000019) m; scale S = (1.10000002, 0.0250000004, 0.219999999); rotation (Rx,Ry,Rz) = (0, 0, 0) degrees.

Shear (xy,xz,yx,yz,zx,zy) = (0, 0, 0, 0, 0, 0). Color = (0.819999993, 0.649999976, 0.280000001); specular = 0.119999997; shininess = 24; emission = 0; flash = 0; material layer = 5; texture scale = 3. Pattern enabled = 0, pattern scale = (1, 1, 1), offset = (0, 0, 0).

| Step | Actual renderer image |
| --- | --- |
| Unit cube | ![Unit cube](part-2-0.png) |
| Scale | ![Scale](part-2-1.png) |
| Shear | ![Shear](part-2-2.png) |
| Rotate X | ![Rotate X](part-2-3.png) |
| Rotate Y | ![Rotate Y](part-2-4.png) |
| Rotate Z | ![Rotate Z](part-2-5.png) |
| Translate to world | ![Translate to world](part-2-6.png) |

Steps 1–5 share one camera; the unit cube has its own close view, and step 6 is reframed at the actual world location to keep small objects visible. Reframing changes the view, never the model coordinates. Identity steps deliberately remain visible. Intermediate shapes are instructional states, while step 6 uses the unmodified game object.

Each stage below gives its exact numeric matrix and all eight corner multiplications. For each output row r: p'[r] = A[r,0]p.x + A[r,1]p.y + A[r,2]p.z + A[r,3]p.w.

![Signed translation components](part-2-translation.svg)


#### S

```text
[ 1.10000002 0 0 0 ]
[ 0 0.0250000004 0 0 ]
[ 0 0 0.219999999 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.5, -0.5, -0.5) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.5, -0.5, 0.5) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.5, 0.5, -0.5) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.5, 0.5, 0.5) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.5, -0.5, -0.5) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.5, -0.5, 0.5) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.5, 0.5, -0.5) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.5, 0.5, 0.5) | (0.550000012, 0.0125000002, 0.109999999) |

#### H

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.550000012, -0.0125000002, -0.109999999) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.550000012, -0.0125000002, 0.109999999) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.550000012, 0.0125000002, -0.109999999) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.550000012, 0.0125000002, 0.109999999) | (0.550000012, 0.0125000002, 0.109999999) |

#### Rx

```text
[ 1 0 0 0 ]
[ 0 1 -0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.550000012, -0.0125000002, -0.109999999) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.550000012, -0.0125000002, 0.109999999) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.550000012, 0.0125000002, -0.109999999) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.550000012, 0.0125000002, 0.109999999) | (0.550000012, 0.0125000002, 0.109999999) |

#### Ry

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ -0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.550000012, -0.0125000002, -0.109999999) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.550000012, -0.0125000002, 0.109999999) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.550000012, 0.0125000002, -0.109999999) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.550000012, 0.0125000002, 0.109999999) | (0.550000012, 0.0125000002, 0.109999999) |

#### Rz

```text
[ 1 -0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.550000012, -0.0125000002, -0.109999999) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.550000012, -0.0125000002, 0.109999999) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.550000012, 0.0125000002, -0.109999999) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.550000012, 0.0125000002, 0.109999999) | (0.550000012, 0.0125000002, 0.109999999) |

#### T

```text
[ 1 0 0 -13 ]
[ 0 1 0 1.13 ]
[ 0 0 1 -6.80000019 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-13.5500002, 1.11749995, -6.91000032) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-13.5500002, 1.11749995, -6.69000006) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-13.5500002, 1.14250004, -6.91000032) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-13.5500002, 1.14250004, -6.69000006) |
| (0.550000012, -0.0125000002, -0.109999999) | (-12.4499998, 1.11749995, -6.91000032) |
| (0.550000012, -0.0125000002, 0.109999999) | (-12.4499998, 1.11749995, -6.69000006) |
| (0.550000012, 0.0125000002, -0.109999999) | (-12.4499998, 1.14250004, -6.91000032) |
| (0.550000012, 0.0125000002, 0.109999999) | (-12.4499998, 1.14250004, -6.69000006) |

Final M = T Rz Ry Rx H S:

```text
[ 1.10000002 0 0 -13 ]
[ 0 0.0250000004 0 1.13 ]
[ 0 0 0.219999999 -6.80000019 ]
[ 0 0 0 1 ]
```

### Display identification plate — `EQUIPMENT_LABEL_1`

1 unit = 1 m; shared cube transform pipeline

Position T = (-9, 1.13, -6.80000019) m; scale S = (1.10000002, 0.0250000004, 0.219999999); rotation (Rx,Ry,Rz) = (0, 0, 0) degrees.

Shear (xy,xz,yx,yz,zx,zy) = (0, 0, 0, 0, 0, 0). Color = (0.819999993, 0.649999976, 0.280000001); specular = 0.119999997; shininess = 24; emission = 0; flash = 0; material layer = 5; texture scale = 3. Pattern enabled = 0, pattern scale = (1, 1, 1), offset = (0, 0, 0).

| Step | Actual renderer image |
| --- | --- |
| Unit cube | ![Unit cube](part-3-0.png) |
| Scale | ![Scale](part-3-1.png) |
| Shear | ![Shear](part-3-2.png) |
| Rotate X | ![Rotate X](part-3-3.png) |
| Rotate Y | ![Rotate Y](part-3-4.png) |
| Rotate Z | ![Rotate Z](part-3-5.png) |
| Translate to world | ![Translate to world](part-3-6.png) |

Steps 1–5 share one camera; the unit cube has its own close view, and step 6 is reframed at the actual world location to keep small objects visible. Reframing changes the view, never the model coordinates. Identity steps deliberately remain visible. Intermediate shapes are instructional states, while step 6 uses the unmodified game object.

Each stage below gives its exact numeric matrix and all eight corner multiplications. For each output row r: p'[r] = A[r,0]p.x + A[r,1]p.y + A[r,2]p.z + A[r,3]p.w.

![Signed translation components](part-3-translation.svg)


#### S

```text
[ 1.10000002 0 0 0 ]
[ 0 0.0250000004 0 0 ]
[ 0 0 0.219999999 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.5, -0.5, -0.5) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.5, -0.5, 0.5) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.5, 0.5, -0.5) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.5, 0.5, 0.5) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.5, -0.5, -0.5) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.5, -0.5, 0.5) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.5, 0.5, -0.5) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.5, 0.5, 0.5) | (0.550000012, 0.0125000002, 0.109999999) |

#### H

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.550000012, -0.0125000002, -0.109999999) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.550000012, -0.0125000002, 0.109999999) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.550000012, 0.0125000002, -0.109999999) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.550000012, 0.0125000002, 0.109999999) | (0.550000012, 0.0125000002, 0.109999999) |

#### Rx

```text
[ 1 0 0 0 ]
[ 0 1 -0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.550000012, -0.0125000002, -0.109999999) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.550000012, -0.0125000002, 0.109999999) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.550000012, 0.0125000002, -0.109999999) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.550000012, 0.0125000002, 0.109999999) | (0.550000012, 0.0125000002, 0.109999999) |

#### Ry

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ -0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.550000012, -0.0125000002, -0.109999999) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.550000012, -0.0125000002, 0.109999999) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.550000012, 0.0125000002, -0.109999999) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.550000012, 0.0125000002, 0.109999999) | (0.550000012, 0.0125000002, 0.109999999) |

#### Rz

```text
[ 1 -0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.550000012, -0.0125000002, -0.109999999) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.550000012, -0.0125000002, 0.109999999) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.550000012, 0.0125000002, -0.109999999) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.550000012, 0.0125000002, 0.109999999) | (0.550000012, 0.0125000002, 0.109999999) |

#### T

```text
[ 1 0 0 -9 ]
[ 0 1 0 1.13 ]
[ 0 0 1 -6.80000019 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-9.55000019, 1.11749995, -6.91000032) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-9.55000019, 1.11749995, -6.69000006) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-9.55000019, 1.14250004, -6.91000032) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-9.55000019, 1.14250004, -6.69000006) |
| (0.550000012, -0.0125000002, -0.109999999) | (-8.44999981, 1.11749995, -6.91000032) |
| (0.550000012, -0.0125000002, 0.109999999) | (-8.44999981, 1.11749995, -6.69000006) |
| (0.550000012, 0.0125000002, -0.109999999) | (-8.44999981, 1.14250004, -6.91000032) |
| (0.550000012, 0.0125000002, 0.109999999) | (-8.44999981, 1.14250004, -6.69000006) |

Final M = T Rz Ry Rx H S:

```text
[ 1.10000002 0 0 -9 ]
[ 0 0.0250000004 0 1.13 ]
[ 0 0 0.219999999 -6.80000019 ]
[ 0 0 0 1 ]
```

### Display identification plate — `EQUIPMENT_LABEL_2`

1 unit = 1 m; shared cube transform pipeline

Position T = (-5, 1.13, -6.80000019) m; scale S = (1.10000002, 0.0250000004, 0.219999999); rotation (Rx,Ry,Rz) = (0, 0, 0) degrees.

Shear (xy,xz,yx,yz,zx,zy) = (0, 0, 0, 0, 0, 0). Color = (0.819999993, 0.649999976, 0.280000001); specular = 0.119999997; shininess = 24; emission = 0; flash = 0; material layer = 5; texture scale = 3. Pattern enabled = 0, pattern scale = (1, 1, 1), offset = (0, 0, 0).

| Step | Actual renderer image |
| --- | --- |
| Unit cube | ![Unit cube](part-4-0.png) |
| Scale | ![Scale](part-4-1.png) |
| Shear | ![Shear](part-4-2.png) |
| Rotate X | ![Rotate X](part-4-3.png) |
| Rotate Y | ![Rotate Y](part-4-4.png) |
| Rotate Z | ![Rotate Z](part-4-5.png) |
| Translate to world | ![Translate to world](part-4-6.png) |

Steps 1–5 share one camera; the unit cube has its own close view, and step 6 is reframed at the actual world location to keep small objects visible. Reframing changes the view, never the model coordinates. Identity steps deliberately remain visible. Intermediate shapes are instructional states, while step 6 uses the unmodified game object.

Each stage below gives its exact numeric matrix and all eight corner multiplications. For each output row r: p'[r] = A[r,0]p.x + A[r,1]p.y + A[r,2]p.z + A[r,3]p.w.

![Signed translation components](part-4-translation.svg)


#### S

```text
[ 1.10000002 0 0 0 ]
[ 0 0.0250000004 0 0 ]
[ 0 0 0.219999999 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.5, -0.5, -0.5) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.5, -0.5, 0.5) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.5, 0.5, -0.5) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.5, 0.5, 0.5) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.5, -0.5, -0.5) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.5, -0.5, 0.5) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.5, 0.5, -0.5) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.5, 0.5, 0.5) | (0.550000012, 0.0125000002, 0.109999999) |

#### H

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.550000012, -0.0125000002, -0.109999999) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.550000012, -0.0125000002, 0.109999999) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.550000012, 0.0125000002, -0.109999999) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.550000012, 0.0125000002, 0.109999999) | (0.550000012, 0.0125000002, 0.109999999) |

#### Rx

```text
[ 1 0 0 0 ]
[ 0 1 -0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.550000012, -0.0125000002, -0.109999999) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.550000012, -0.0125000002, 0.109999999) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.550000012, 0.0125000002, -0.109999999) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.550000012, 0.0125000002, 0.109999999) | (0.550000012, 0.0125000002, 0.109999999) |

#### Ry

```text
[ 1 0 0 0 ]
[ 0 1 0 0 ]
[ -0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.550000012, -0.0125000002, -0.109999999) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.550000012, -0.0125000002, 0.109999999) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.550000012, 0.0125000002, -0.109999999) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.550000012, 0.0125000002, 0.109999999) | (0.550000012, 0.0125000002, 0.109999999) |

#### Rz

```text
[ 1 -0 0 0 ]
[ 0 1 0 0 ]
[ 0 0 1 0 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-0.550000012, -0.0125000002, -0.109999999) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-0.550000012, -0.0125000002, 0.109999999) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-0.550000012, 0.0125000002, -0.109999999) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-0.550000012, 0.0125000002, 0.109999999) |
| (0.550000012, -0.0125000002, -0.109999999) | (0.550000012, -0.0125000002, -0.109999999) |
| (0.550000012, -0.0125000002, 0.109999999) | (0.550000012, -0.0125000002, 0.109999999) |
| (0.550000012, 0.0125000002, -0.109999999) | (0.550000012, 0.0125000002, -0.109999999) |
| (0.550000012, 0.0125000002, 0.109999999) | (0.550000012, 0.0125000002, 0.109999999) |

#### T

```text
[ 1 0 0 -5 ]
[ 0 1 0 1.13 ]
[ 0 0 1 -6.80000019 ]
[ 0 0 0 1 ]
```

| Input corner (w=1) | Output corner (w=1) |
| --- | --- |
| (-0.550000012, -0.0125000002, -0.109999999) | (-5.55000019, 1.11749995, -6.91000032) |
| (-0.550000012, -0.0125000002, 0.109999999) | (-5.55000019, 1.11749995, -6.69000006) |
| (-0.550000012, 0.0125000002, -0.109999999) | (-5.55000019, 1.14250004, -6.91000032) |
| (-0.550000012, 0.0125000002, 0.109999999) | (-5.55000019, 1.14250004, -6.69000006) |
| (0.550000012, -0.0125000002, -0.109999999) | (-4.44999981, 1.11749995, -6.91000032) |
| (0.550000012, -0.0125000002, 0.109999999) | (-4.44999981, 1.11749995, -6.69000006) |
| (0.550000012, 0.0125000002, -0.109999999) | (-4.44999981, 1.14250004, -6.91000032) |
| (0.550000012, 0.0125000002, 0.109999999) | (-4.44999981, 1.14250004, -6.69000006) |

Final M = T Rz Ry Rx H S:

```text
[ 1.10000002 0 0 -5 ]
[ 0 0.0250000004 0 1.13 ]
[ 0 0 0.219999999 -6.80000019 ]
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
