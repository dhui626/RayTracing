
#ifndef RTWEEKEND_H
#define RTWEEKEND_H


// Common Headers

#include <Eigen/Dense>

#include "ray.h"

// Usings

using namespace std;
using namespace Eigen;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}
inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}
inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}
inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
inline Vector3f random_unit_vector() {  // 生成随机单位向量
    auto p = Vector3f::Random();
    return p / p.norm();
}


#endif