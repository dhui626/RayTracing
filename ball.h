
#ifndef BALL_H
#define BALL_H

#include "hittable.h"

class ball : public hittable {

public:
    ball() {}
    ball(Vector3f cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;

private:
    Vector3f center;
    double radius;
    shared_ptr<material> mat_ptr;
};

bool ball::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {

    Vector3f oc = r.origin() - center;
    auto a = r.direction().dot(r.direction());
    auto b = oc.dot(r.direction());       //b不乘以2.0，可简化后面的计算
    auto c = oc.dot(oc) - radius * radius;
    auto delta = b * b - a * c;

    if (delta < 0) {
        return false;
    }
    else {
        auto sol = (-b - sqrt(delta)) / a;

        if (sol < t_min || t_max < sol) {
            sol = (-b + sqrt(delta)) / a;
            if (sol < t_min || t_max < sol)
                return false;
        }

        rec.t = sol;
        rec.p = r.at(rec.t);

        Vector3f outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);
        //rec.n = (rec.p - center) / radius;
        rec.mat_ptr = mat_ptr;

        return true;
    }
    
}

#endif