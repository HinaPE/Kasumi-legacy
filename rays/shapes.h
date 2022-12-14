#ifndef HINAPE_SHAPES_H
#define HINAPE_SHAPES_H

#include "../geometry/util.h"
#include "../lib/mathlib.h"
#include "../platform/gl.h"

#include "trace.h"
#include <variant>

namespace PT
{

enum class Shape_Type : int
{
    none, sphere, count
};
extern const char *Shape_Type_Names[(int) Shape_Type::count];

class Sphere
{
public:
    Sphere() = default;

    Sphere(float radius) : radius(radius)
    {
    }

    BBox bbox() const;
    Trace hit(const Ray &ray) const;

    float radius = 1.0f;

    bool operator!=(const Sphere &s) const
    {
        return radius != s.radius;
    }
};

class Shape
{
public:
    Shape() = default;

    Shape(Sphere &&sphere) : underlying(std::move(sphere))
    {
    }

    Shape(const Shape &src) = default;
    Shape &operator=(const Shape &src) = default;
    Shape &operator=(Shape &&src) = default;
    Shape(Shape &&src) = default;

    BBox bbox() const
    {
        return std::visit(overloaded{[](const auto &o) { return o.bbox(); }}, underlying);
    }

    Trace hit(Ray ray) const
    {
        return std::visit(overloaded{[&ray](const auto &o) { return o.hit(ray); }}, underlying);
    }

    template<typename T>
    T &get()
    {
        return std::get<T>(underlying);
    }

    template<typename T>
    const T &get() const
    {
        return std::get<T>(underlying);
    }

    bool operator!=(const Shape &c) const
    {
        return underlying != c.underlying;
    }

    GL::Mesh mesh() const
    {
        return std::visit([](const PT::Sphere &sphere) { return Util::sphere_mesh(sphere.radius, 2); }, underlying);
    }

private:
    std::variant<Sphere> underlying;
};

} // namespace PT

#endif
