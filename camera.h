
#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/core>

#include "ray.h"

using namespace Eigen;

class camera {

public:
    camera() {
        auto aspect_ratio = 16.0 / 9.0;
        auto viewport_height = 2.0;
        auto viewport_width = aspect_ratio * viewport_height;
        auto focal_length = 1.0;                //����

        origin = Vector3f(0, 0, 0);
        horizontal = Vector3f(viewport_width, 0, 0);
        vertical = Vector3f(0, viewport_height, 0);
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vector3f(0, 0, focal_length);
    }

    ray get_ray(double u, double v) const {
        return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }

private:
    Vector3f origin;
    Vector3f lower_left_corner;
    Vector3f horizontal;
    Vector3f vertical;
};
#endif
