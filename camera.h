
#ifndef CAMERA_H
#define CAMERA_H

#include "ray.h"
#include "rtweekend.h"


class camera {

public:
    camera(
        Vector3f lookfrom,
        Vector3f lookat,
        Vector3f vup,
        double vfov, // vertical field-of-view in degrees
        double aspect_ratio,
        double aperture,
        double focus_dist
    ) {
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2.0 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        w = lookfrom - lookat;
        w /= w.norm();
        u = vup.cross(w);     // 为什么只有<Eigen/Dense>里有cross啊啊，<Eigen/core>里没有
        u /= u.norm();
        v = w.cross(u);

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

        lens_radius = aperture / 2;
    }

    ray get_ray(double s, double t) const {
        Vector3f rd = lens_radius * Vector3f(random_double(-1, 1), random_double(-1, 1), 0);
        Vector3f offset = u * rd.x() + v * rd.y();

        return ray(
            origin + offset,
            lower_left_corner + s * horizontal + t * vertical - origin - offset
        );
    }

private:
    Vector3f origin;
    Vector3f lower_left_corner;
    Vector3f horizontal;
    Vector3f vertical;
    Vector3f u, v, w;
    double lens_radius;
};
#endif

