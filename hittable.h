
#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

struct hit_record {         // 记录交点的位置、法向、t值
    Vector3f p;
    Vector3f n;
    double t;
    bool front_face;        // 待修改

    inline void set_face_normal(const ray& r, const Vector3f& outward_normal) {
        front_face = r.direction().dot(outward_normal) < 0;
        n = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};

#endif