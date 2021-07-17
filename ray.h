
#ifndef RAY_H
#define RAY_H

#include <Eigen/core>

using namespace Eigen;

class ray {

public:
    ray() {}
    ray(const Vector3f& origin, const Vector3f& direction): orig(origin), dir(direction) {}

    Vector3f origin() const { return orig; }
    Vector3f direction() const { return dir; }

    Vector3f at(double t) const {
        return orig + t * dir;
    }

private:
    Vector3f orig;
    Vector3f dir;
};

#endif