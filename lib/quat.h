#ifndef HINAPE_QUAT_H
#define HINAPE_QUAT_H

#include <algorithm>
#include <cmath>

#include "log.h"
#include "mat4.h"
#include "vec3.h"
#include "vec4.h"

#define EPS_F 0.00001f
#define PI_F 3.14159265358979323846264338327950288f
#define Radians(v) ((v) * (PI_F / 180.0f))
#define Degrees(v) ((v) * (180.0f / PI_F))

struct Quat
{

    Quat()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
        w = 1.0f;
    }

    explicit Quat(float _x, float _y, float _z, float _w)
    {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    }

    explicit Quat(Vec3 complex, float real)
    {
        x = complex.x;
        y = complex.y;
        z = complex.z;
        w = real;
    }

    explicit Quat(const Vec4 &src)
    {
        x = src.x;
        y = src.y;
        z = src.z;
        w = src.w;
    }

    Quat(const Quat &) = default;
    auto operator=(const Quat &) -> Quat & = default;
    ~Quat() = default;

    /// Create unit quaternion representing given axis-angle rotation
    static auto axis_angle(Vec3 axis, float angle) -> Quat
    {
        axis.normalize();
        angle = Radians(angle) / 2.0f;
        float sin = std::sin(angle);
        float x = sin * axis.x;
        float y = sin * axis.y;
        float z = sin * axis.z;
        float w = std::cos(angle);
        return Quat(x, y, z, w).unit();
    }

    /// Create unit quaternion representing given euler angles (XYZ)
    static auto euler(Vec3 angles) -> Quat
    {
        if (angles == Vec3{0.0f, 0.0f, 180.0f} || angles == Vec3{180.0f, 0.0f, 0.0f})
            return Quat{0.0f, 0.0f, -1.0f, 0.0f};
        float c1 = std::cos(Radians(angles[2] * 0.5f));
        float c2 = std::cos(Radians(angles[1] * 0.5f));
        float c3 = std::cos(Radians(angles[0] * 0.5f));
        float s1 = std::sin(Radians(angles[2] * 0.5f));
        float s2 = std::sin(Radians(angles[1] * 0.5f));
        float s3 = std::sin(Radians(angles[0] * 0.5f));
        float x = c1 * c2 * s3 - s1 * s2 * c3;
        float y = c1 * s2 * c3 + s1 * c2 * s3;
        float z = s1 * c2 * c3 - c1 * s2 * s3;
        float w = c1 * c2 * c3 + s1 * s2 * s3;
        return Quat(x, y, z, w);
    }

    auto operator[](int idx) -> float &
    {
        assert(idx >= 0 && idx <= 3);
        return data[idx];
    }

    auto operator[](int idx) const -> float
    {
        assert(idx >= 0 && idx <= 3);
        return data[idx];
    }

    auto conjugate() const -> Quat
    {
        return Quat(-x, -y, -z, w);
    }

    auto inverse() const -> Quat
    {
        return conjugate().unit();
    }

    auto complex() const -> Vec3
    {
        return Vec3(x, y, z);
    }

    auto real() const -> float
    {
        return w;
    }

    auto norm_squared() const -> float
    {
        return x * x + y * y + z * z + w * w;
    }

    auto norm() const -> float
    {
        return std::sqrt(norm_squared());
    }

    auto unit() const -> Quat
    {
        float n = norm();
        return Quat(x / n, y / n, z / n, w / n);
    }

    auto operator*(const Quat &r) const -> Quat
    {
        return Quat(y * r.z - z * r.y + x * r.w + w * r.x, z * r.x - x * r.z + y * r.w + w * r.y, x * r.y - y * r.x + z * r.w + w * r.z,
                    w * r.w - x * r.x - y * r.y - z * r.z);
    }

    auto operator*(float s) const -> Quat
    {
        return Quat(s * x, s * y, s * z, s * w);
    }

    auto operator+(const Quat &r) const -> Quat
    {
        return Quat(x + r.x, y + r.y, z + r.z, w + r.w);
    }

    auto operator-(const Quat &r) const -> Quat
    {
        return Quat(x - r.x, y - r.y, z - r.z, w - r.w);
    }

    auto operator-() const -> Quat
    {
        return Quat(-x, -y, -z, -w);
    }

    /// Convert quaternion to equivalent euler angle rotation (XYZ)
    auto to_euler() const -> Vec3
    {
        return unit().to_mat().to_euler();
    }

    /// Convert quaternion to equivalent rotation matrix (orthonormal, 3x3)
    auto to_mat() const -> Mat4
    {
        return Mat4{Vec4{1 - 2 * y * y - 2 * z * z, 2 * x * y + 2 * z * w, 2 * x * z - 2 * y * w, 0.0f},
                    Vec4{2 * x * y - 2 * z * w, 1 - 2 * x * x - 2 * z * z, 2 * y * z + 2 * x * w, 0.0f},
                    Vec4{2 * x * z + 2 * y * w, 2 * y * z - 2 * x * w, 1 - 2 * x * x - 2 * y * y, 0.0f}, Vec4{0.0f, 0.0f, 0.0f, 1.0f}};
    }

    /// Apply rotation to given vector
    auto rotate(Vec3 v) const -> Vec3
    {
        return (((*this) * Quat(v, 0)) * conjugate()).complex();
    }

    auto operator==(const Quat &v) const -> bool
    {
        return x == v.x && y == v.y && z == v.z && w == v.w;
    }

    auto operator!=(const Quat &v) const -> bool
    {
        return x != v.x || y != v.y || z != v.z || w != v.w;
    }

    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };
        float data[4] = {};
    };
};

inline auto dot(const Quat &q0, const Quat &q1) -> float
{
    return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}

inline auto operator<<(std::ostream &out, Quat q) -> std::ostream &
{
    out << "Quat{" << q.x << "," << q.y << "," << q.z << "," << q.w << "}";
    return out;
}

inline auto operator*(float s, const Quat &q) -> Quat
{
    return Quat(s * q.x, s * q.y, s * q.z, s * q.w);
}

inline auto operator+(float s, const Quat &q) -> Quat
{
    return Quat(q.x, q.y, q.z, s + q.w);
}

inline auto slerp(const Quat &q0, const Quat &q1, float t) -> Quat
{

    float hcos = dot(q0, q1);

    Quat shortest = hcos < 0 ? -q0 : q0;

    if (std::abs(hcos) >= 1.0f - EPS_F)
    {
        return (1.0f - t) * shortest + t * q1;
    }

    float a = std::acos(std::abs(hcos));
    return (std::sin((1.0f - t) * a) * shortest + std::sin(t * a) * q1) * (1.0f / std::sin(a));
}

#endif
