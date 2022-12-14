#ifndef HINAPE_LIGHT_H
#define HINAPE_LIGHT_H

#include <variant>

#include "../lib/mathlib.h"
#include "../lib/spectrum.h"
#include "../scene/object.h"
#include "../util/hdr_image.h"

#include "samplers.h"

namespace PT
{

struct Light_Sample
{

    Spectrum radiance;
    Vec3 direction;
    float distance = 0.0f;

    void transform(const Mat4 &T)
    {
        direction = T.rotate(direction);
    }
};

struct Directional_Light
{

    Directional_Light(Spectrum r) : radiance(r)
    {
    }

    Light_Sample sample(Vec3 from) const;
    Spectrum radiance;
};

struct Point_Light
{

    Point_Light(Spectrum r) : radiance(r)
    {
    }

    Light_Sample sample(Vec3 from) const;
    Spectrum radiance;
};

struct Spot_Light
{

    Spot_Light(Spectrum r, Vec2 a) : radiance(r), angle_bounds(a)
    {
    }

    Light_Sample sample(Vec3 from) const;
    Spectrum radiance;
    Vec2 angle_bounds;
};

class Delta_Light
{
public:
    Delta_Light(Directional_Light &&l, Scene_ID id, const Mat4 &T = Mat4::I) : trans(T), itrans(T.inverse()), _id(id), underlying(std::move(l))
    {
        has_trans = trans != Mat4::I;
    }

    Delta_Light(Point_Light &&l, Scene_ID id, const Mat4 &T = Mat4::I) : trans(T), itrans(T.inverse()), _id(id), underlying(std::move(l))
    {
        has_trans = trans != Mat4::I;
    }

    Delta_Light(Spot_Light &&l, Scene_ID id, const Mat4 &T = Mat4::I) : trans(T), itrans(T.inverse()), _id(id), underlying(std::move(l))
    {
        has_trans = trans != Mat4::I;
    }

    Delta_Light(const Delta_Light &src) = delete;
    Delta_Light &operator=(const Delta_Light &src) = delete;
    Delta_Light &operator=(Delta_Light &&src) = default;
    Delta_Light(Delta_Light &&src) = default;

    Light_Sample sample(Vec3 from) const
    {
        if (has_trans)
            from = itrans * from;
        Light_Sample ret = std::visit([from](const auto &l) { return l.sample(from); }, underlying);
        if (has_trans)
            ret.transform(trans);
        return ret;
    }

    Scene_ID id() const
    {
        return _id;
    }

    void set_trans(const Mat4 &T)
    {
        trans = T;
        itrans = T.inverse();
        has_trans = trans != Mat4::I;
    }

private:
    bool has_trans;
    Mat4 trans, itrans;
    Scene_ID _id;
    std::variant<Directional_Light, Point_Light, Spot_Light> underlying;
};

} // namespace PT

#endif
