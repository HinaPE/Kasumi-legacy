#ifndef HINAPE_MATERIAL_H
#define HINAPE_MATERIAL_H

#include "../geometry/spline.h"
#include "../lib/spectrum.h"

enum class Material_Type : int
{
    lambertian, mirror, refract, glass, diffuse_light, count
};
extern const char *Material_Type_Names[(int) Material_Type::count];

class Material
{
public:
    Material() = default;

    Material(Material_Type type)
    {
        opt.type = type;
    }

    Material(const Material &src) = delete;
    Material(Material &&src) = default;
    ~Material() = default;

    void operator=(const Material &src) = delete;
    auto operator=(Material &&src) -> Material & = default;

    // TODO: decide how to handle texture resources. should probably use a texture repository
    // and also integrate that into the envmaps
    auto copy() const -> Material;

    auto emissive() const -> Spectrum;
    auto layout_color() const -> Vec3;

    struct Options
    {
        Material_Type type = Material_Type::lambertian;
        Spectrum albedo = Spectrum(1.0f);
        Spectrum reflectance = Spectrum(1.0f);
        Spectrum transmittance = Spectrum(1.0f);
        Spectrum emissive = Spectrum(1.0f);
        float intensity = 1.0f;
        float ior = 1.2f;
    };

    struct Anim_Material
    {
        void at(float t, Options &o) const;
        void set(float t, Options o);
        Splines<Spectrum, Spectrum, Spectrum, Spectrum, float, float> splines;
    };

    Options opt;
    Anim_Material anim;
};

auto operator!=(const Material::Options &l, const Material::Options &r) -> bool;

#endif
