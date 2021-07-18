
#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "rtweekend.h"

////////////////
Vector3f reflect(const Vector3f& v, const Vector3f& n) {    //反射
    return v - 2 * v.dot(n) * n;
}
Vector3f refract(const Vector3f& uv, const Vector3f& n, double etai_over_etat) { //待修改
    auto cos_theta = fmin(-uv.dot(n), 1.0);
    Vector3f r_out_perp = etai_over_etat * (uv + cos_theta * n);
    Vector3f r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.dot(r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
}
////////////////

class material {
public:
    virtual bool scatter(
        const ray& r_in, Vector3f& attenuation, ray& scattered, const hit_record& rec
    ) const = 0;
};

class lambertian : public material {        //漫反射材质
public:
    lambertian(const Vector3f& a) : albedo(a) {}

    virtual bool scatter(
        const ray& r_in, Vector3f& attenuation, ray& scattered, const hit_record& rec
    ) const override {
        Vector3f scatter_direction = rec.n + random_unit_vector();

        if (scatter_direction.norm() < 1E-5)
            scatter_direction = rec.n;          //防止方向向量太小导致奇异

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

private:
    Vector3f albedo;
};

class metal : public material {
public:
    metal(const Vector3f& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const ray& r_in, Vector3f& attenuation, ray& scattered, const hit_record& rec
    ) const override {
        Vector3f reflected = reflect(r_in.direction() / r_in.direction().norm(), rec.n);
        scattered = ray(rec.p, reflected + fuzz * Vector3f::Random());
        attenuation = albedo;
        return (scattered.direction().dot(rec.n) > 0);
    }

private:
    Vector3f albedo;
    double fuzz;        // 模糊度
};

class dielectric : public material {            // 玻璃材质
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter(
        const ray& r_in, Vector3f& attenuation, ray& scattered, const hit_record& rec
    ) const override {
        attenuation = Vector3f(1.0, 1.0, 1.0);
        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        Vector3f unit_direction = r_in.direction() / r_in.direction().norm();
        double cos_theta = fmin(-unit_direction.dot(rec.n), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        Vector3f direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, rec.n);
        else
            direction = refract(unit_direction, rec.n, refraction_ratio);

        scattered = ray(rec.p, direction);

        return true;
    }

private:
    double ir; // Index of Refraction
    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};

#endif