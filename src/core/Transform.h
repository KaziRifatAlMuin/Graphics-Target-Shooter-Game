#pragma once

#include <array>
#include <cmath>

namespace shooter {
constexpr float pi = 3.14159265358979323846f;
// Convert degrees to radians for trigonometry: radians = degrees * pi / 180.
inline float radians(float degrees) { return degrees * pi / 180.0f; }

// A vector stores three coordinates; here X is sideways, Y is up, and forward is usually -Z.
struct Vec3 { float x = 0, y = 0, z = 0; };
// The extra coordinate w distinguishes a position (w=1) from a direction (w=0).
struct Vec4 { float x, y, z, w; };
// Add corresponding coordinates: a + b = (ax+bx, ay+by, az+bz).
inline Vec3 operator+(Vec3 a, Vec3 b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
// Subtract coordinates to get the displacement from b to a.
inline Vec3 operator-(Vec3 a, Vec3 b) { return {a.x-b.x, a.y-b.y, a.z-b.z}; }
// Multiply each coordinate by s to change a vector's length.
inline Vec3 operator*(Vec3 a, float s) { return {a.x*s, a.y*s, a.z*s}; }
// Dot product a.b = ax*bx + ay*by + az*bz; for unit vectors it equals cos(angle).
inline float dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
// Cross product a x b is perpendicular to both vectors; its order determines its direction.
inline Vec3 cross(Vec3 a, Vec3 b) {
    return {a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x};
}
// Unit direction = v / sqrt(v.v); return zero when v has no length.
inline Vec3 normalize(Vec3 v) {
    const float length = std::sqrt(dot(v, v));
    return length > 0 ? v * (1.0f / length) : Vec3{};
}

// Column-major storage, column vectors: upload directly with GL_FALSE.
struct Mat4 {
    std::array<float, 16> data{};
    // Column-major index = column*4 + row; this overload lets callers change an entry.
    float& at(int row, int col) { return data[col*4+row]; }
    // Read a matrix entry using the same column-major index.
    float at(int row, int col) const { return data[col*4+row]; }
    // Identity has diagonal entries 1 and all others 0, so I*p = p.
    static Mat4 identity() {
        Mat4 result;
        for (int i = 0; i < 4; ++i) result.at(i, i) = 1;
        return result;
    }
};
// Matrix product C[row,col] = sum(A[row,k]*B[k,col]); B acts on a point before A.
inline Mat4 operator*(const Mat4& a, const Mat4& b) {
    Mat4 result;
    for (int row = 0; row < 4; ++row)
        for (int col = 0; col < 4; ++col)
            for (int k = 0; k < 4; ++k)
                result.at(row, col) += a.at(row, k) * b.at(k, col);
    return result;
}
// Multiply p' = M*p; w=1 includes translation, while w=0 ignores it.
inline Vec4 transformPoint(const Mat4& m, const Vec4& p) {
    return {
        m.at(0,0)*p.x + m.at(0,1)*p.y + m.at(0,2)*p.z + m.at(0,3)*p.w,
        m.at(1,0)*p.x + m.at(1,1)*p.y + m.at(1,2)*p.z + m.at(1,3)*p.w,
        m.at(2,0)*p.x + m.at(2,1)*p.y + m.at(2,2)*p.z + m.at(2,3)*p.w,
        m.at(3,0)*p.x + m.at(3,1)*p.y + m.at(3,2)*p.z + m.at(3,3)*p.w
    };
}
// Move a point: (x',y',z') = (x+tx, y+ty, z+tz).
inline Mat4 makeTranslation(float x, float y, float z) {
    // Assign a stable example name and group from an object's type, component, and ID.
    Mat4 m = Mat4::identity();
    m.at(0,3)=x; m.at(1,3)=y; m.at(2,3)=z;
    return m;
}
// Resize each axis: (x',y',z') = (sx*x, sy*y, sz*z).
inline Mat4 makeScale(float x, float y, float z) {
    // Assign a stable example name and group from an object's type, component, and ID.
    Mat4 m = Mat4::identity();
    m.at(0,0)=x; m.at(1,1)=y; m.at(2,2)=z;
    return m;
}
// Rotate about X: y'=y*cos(a)-z*sin(a), z'=y*sin(a)+z*cos(a); x stays fixed.
inline Mat4 makeRotationX(float degrees) {
    // Assign a stable example name and group from an object's type, component, and ID.
    Mat4 m = Mat4::identity();
    const float c=std::cos(radians(degrees)), s=std::sin(radians(degrees));
    m.at(1,1)=c; m.at(1,2)=-s; m.at(2,1)=s; m.at(2,2)=c;
    return m;
}
// Rotate about Y: x'=x*cos(a)+z*sin(a), z'=-x*sin(a)+z*cos(a); y stays fixed.
inline Mat4 makeRotationY(float degrees) {
    // Assign a stable example name and group from an object's type, component, and ID.
    Mat4 m = Mat4::identity();
    const float c=std::cos(radians(degrees)), s=std::sin(radians(degrees));
    m.at(0,0)=c; m.at(0,2)=s; m.at(2,0)=-s; m.at(2,2)=c;
    return m;
}
// Rotate about Z: x'=x*cos(a)-y*sin(a), y'=x*sin(a)+y*cos(a); z stays fixed.
inline Mat4 makeRotationZ(float degrees) {
    // Assign a stable example name and group from an object's type, component, and ID.
    Mat4 m = Mat4::identity();
    const float c=std::cos(radians(degrees)), s=std::sin(radians(degrees));
    m.at(0,0)=c; m.at(0,1)=-s; m.at(1,0)=s; m.at(1,1)=c;
    return m;
}
// Slant axes: x'=x+xy*y+xz*z, y'=yx*x+y+yz*z, z'=zx*x+zy*y+z.
inline Mat4 makeShear(float xy, float xz, float yx, float yz, float zx, float zy) {
    // Assign a stable example name and group from an object's type, component, and ID.
    Mat4 m = Mat4::identity();
    m.at(0,1)=xy; m.at(0,2)=xz; m.at(1,0)=yx;
    m.at(1,2)=yz; m.at(2,0)=zx; m.at(2,1)=zy;
    return m;
}
// Store position, rotation, size, and slant separately before combining them into a matrix.
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
// List operations in the order a local cube point experiences them: scale, shear, rotate X/Y/Z, move.
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
// World point = T*Rz*Ry*Rx*H*S*local point; read right to left, because order changes the result.
inline Mat4 composeModelMatrix(const Transform& t) {
    // Assign a stable example name and group from an object's type, component, and ID.
    Mat4 model = Mat4::identity();
    // Left-multiply each stage: the resulting matrix is T * Rz * Ry * Rx * H * S.
    for (const auto& stage : modelTransformStages(t)) model = stage.matrix * model;
    return model;
}
// Record the point after each operation so exported calculations can show every intermediate step.
inline std::array<Vec4, 7> traceTransformPoint(const Transform& t, Vec4 localPoint) {
    std::array<Vec4, 7> points{};
    points[0] = localPoint;
    const auto stages = modelTransformStages(t);
    for (std::size_t i = 0; i < stages.size(); ++i)
        points[i+1] = transformPoint(stages[i].matrix, points[i]);
    return points; // local, scaled, sheared, rotated X/Y/Z, world.
}
// Perspective makes distant objects smaller: f=1/tan(FOV/2), x_clip=f*x/aspect, y_clip=f*y.
inline Mat4 makePerspective(float fovDegrees, float aspect, float nearPlane, float farPlane) {
    Mat4 m;
    const float f = 1.0f / std::tan(radians(fovDegrees) / 2);
    m.at(0,0)=f/aspect; m.at(1,1)=f;
    m.at(2,2)=(farPlane+nearPlane)/(nearPlane-farPlane);
    m.at(2,3)=2*farPlane*nearPlane/(nearPlane-farPlane);
    // The GPU divides clip coordinates by w=-z; near/far map visible depth to the OpenGL range.
    m.at(3,2)=-1;
    return m;
}
// Build camera axes: forward=unit(center-eye), right=unit(forward x up), correctedUp=right x forward.
inline Mat4 makeLookAt(Vec3 eye, Vec3 center, Vec3 up) {
    const Vec3 f=normalize(center-eye), s=normalize(cross(f,up)), u=cross(s,f);
    // Assign a stable example name and group from an object's type, component, and ID.
    Mat4 m = Mat4::identity();
    // Project onto camera axes and subtract the eye position, making the camera the new origin.
    m.at(0,0)=s.x; m.at(0,1)=s.y; m.at(0,2)=s.z; m.at(0,3)=-dot(s,eye);
    m.at(1,0)=u.x; m.at(1,1)=u.y; m.at(1,2)=u.z; m.at(1,3)=-dot(u,eye);
    m.at(2,0)=-f.x; m.at(2,1)=-f.y; m.at(2,2)=-f.z; m.at(2,3)=dot(f,eye);
    return m;
}
} // namespace shooter
