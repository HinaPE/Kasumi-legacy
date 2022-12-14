#ifndef HINAPE_LINE_H
#define HINAPE_LINE_H

#include <algorithm>
#include <cmath>
#include <ostream>

#include "vec3.h"

struct Line
{

    Line()
    = default;

    /// Create line from point and unit direction
    explicit Line(Vec3 point, Vec3 dir) : point(point), dir(dir.unit())
    {
    }

    Line(const Line &) = default;
    auto operator=(const Line &) -> Line & = default;
    ~Line() = default;

    /// Get point on line at time t
    auto at(float t) const -> Vec3
    {
        return point + t * dir;
    }

    /// Get closest point on line to pt
    auto closest(Vec3 pt) const -> Vec3
    {
        Vec3 p = pt - point;
        float t = dot(p, dir);
        return at(t);
    }

    /// Get closest point on line to another line.
    /// Returns false if the closest point is 'backward' relative to the line's direction
    auto closest(Line other, Vec3 &pt) const -> bool
    {
        Vec3 p0 = point - other.point;
        float a = dot(dir, other.dir);
        float b = dot(dir, p0);
        float c = dot(other.dir, p0);
        float t0 = (a * c - b) / (1.0f - a * a);
        float t1 = (c - a * b) / (1.0f - a * a);
        pt = at(t0);
        return t1 >= 0.0f;
    }

    Vec3 point, dir;
};

inline auto operator<<(std::ostream &out, Line l) -> std::ostream &
{
    out << "Line{" << l.point << "," << l.dir << "}";
    return out;
}

#endif
